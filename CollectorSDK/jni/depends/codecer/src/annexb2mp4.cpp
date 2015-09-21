#include "annexb2mp4.h"

extern "C"
{
#include <libavformat/avio.h>
#include <libavutil/intreadwrite.h>
};

#define UINT16_MAX	0xffff
/**
 * Table 7-3: NAL unit type codes
 */
enum NALUnitType {
    NAL_TRAIL_N    = 0,
    NAL_TRAIL_R    = 1,
    NAL_TSA_N      = 2,
    NAL_TSA_R      = 3,
    NAL_STSA_N     = 4,
    NAL_STSA_R     = 5,
    NAL_RADL_N     = 6,
    NAL_RADL_R     = 7,
    NAL_RASL_N     = 8,
    NAL_RASL_R     = 9,
    NAL_BLA_W_LP   = 16,
    NAL_BLA_W_RADL = 17,
    NAL_BLA_N_LP   = 18,
    NAL_IDR_W_RADL = 19,
    NAL_IDR_N_LP   = 20,
    NAL_CRA_NUT    = 21,
    NAL_VPS        = 32,
    NAL_SPS        = 33,
    NAL_PPS        = 34,
    NAL_AUD        = 35,
    NAL_EOS_NUT    = 36,
    NAL_EOB_NUT    = 37,
    NAL_FD_NUT     = 38,
    NAL_SEI_PREFIX = 39,
    NAL_SEI_SUFFIX = 40,
};



//==============================================================================
static const uint8_t *ff_avc_find_startcode_internal(const uint8_t *p, const uint8_t *end)
{
	const uint8_t *a = p + 4 - ((intptr_t)p & 3);

	for (end -= 3; p < a && p < end; p++) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 1)
			return p;
	}

	for (end -= 3; p < end; p += 4) {
		uint32_t x = *(const uint32_t*)p;
		//      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
		//      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
		if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
			if (p[1] == 0) {
				if (p[0] == 0 && p[2] == 1)
					return p;
				if (p[2] == 0 && p[3] == 1)
					return p+1;
			}
			if (p[3] == 0) {
				if (p[2] == 0 && p[4] == 1)
					return p+2;
				if (p[4] == 0 && p[5] == 1)
					return p+3;
			}
		}
	}

	for (end += 3; p < end; p++) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 1)
			return p;
	}

	return end + 3;
}

const uint8_t *ff_avc_find_startcode(const uint8_t *p, const uint8_t *end){
	const uint8_t *out= ff_avc_find_startcode_internal(p, end);
	if(p<out && out<end && !out[-1]) out--;
	return out;
}

int ff_avc_parse_nal_units(AVIOContext *pb, const uint8_t *buf_in, int size)
{
	const uint8_t *p = buf_in;
	const uint8_t *end = p + size;
	const uint8_t *nal_start, *nal_end;

	size = 0;
	nal_start = ff_avc_find_startcode(p, end);
	for (;;) {
		while (nal_start < end && !*(nal_start++));
		if (nal_start == end)
			break;

		nal_end = ff_avc_find_startcode(nal_start, end);
		avio_wb32(pb, nal_end - nal_start);
		avio_write(pb, nal_start, nal_end - nal_start);
		size += 4 + nal_end - nal_start;
		nal_start = nal_end;
	}
	return size;
}

int ff_avc_parse_nal_units_buf(const uint8_t *buf_in, uint8_t **buf, int *size)
{
	AVIOContext *pb;
	int ret = avio_open_dyn_buf(&pb);
	if(ret < 0)
		return ret;

	ff_avc_parse_nal_units(pb, buf_in, *size);

	av_freep(buf);
	*size = avio_close_dyn_buf(pb, buf);
	return 0;
}

static int ff_hevc_annexb2mp4(AVIOContext *pb, const uint8_t *buf_in,
	int size, int filter_ps, int *ps_count)
{
	int num_ps = 0, ret = 0;
	uint8_t *buf, *end, *start = NULL;

	if (!filter_ps) {
		ret = ff_avc_parse_nal_units(pb, buf_in, size);
		goto end;
	}

	ret = ff_avc_parse_nal_units_buf(buf_in, &start, &size);
	if (ret < 0)
		goto end;

	ret = 0;
	buf = start;
	end = start + size;

	while (end - buf > 4) {
		uint32_t len = FFMIN(AV_RB32(buf), end - buf - 4);
		uint8_t type = (buf[4] >> 1) & 0x3f;

		buf += 4;

		switch (type) {
		case NAL_VPS:
		case NAL_SPS:
		case NAL_PPS:
			num_ps++;
			break;
		default:
			ret += 4 + len;
			avio_wb32(pb, len);
			avio_write(pb, buf, len);
			break;
		}

		buf += len;
	}

end:
	av_free(start);
	if (ps_count)
		*ps_count = num_ps;
	return ret;
}

int ff_isom_write_avcc(AVIOContext *pb, const uint8_t *data, int len)
{
	if (len > 6) {
		/* check for h264 start code */
		if (AV_RB32(data) == 0x00000001 ||
			AV_RB24(data) == 0x000001) {
				uint8_t *buf=NULL, *end, *start;
				uint32_t sps_size=0, pps_size=0;
				uint8_t *sps=0, *pps=0;

				int ret = ff_avc_parse_nal_units_buf(data, &buf, &len);
				if (ret < 0)
					return ret;
				start = buf;
				end = buf + len;

				/* look for sps and pps */
				while (end - buf > 4) {
					uint32_t size;
					uint8_t nal_type;
					size = FFMIN(AV_RB32(buf), end - buf - 4);
					buf += 4;
					nal_type = buf[0] & 0x1f;

					if (nal_type == 7) { /* SPS */
						sps = buf;
						sps_size = size;
					} else if (nal_type == 8) { /* PPS */
						pps = buf;
						pps_size = size;
					}

					buf += size;
				}

				if (!sps || !pps || sps_size < 4 || sps_size > UINT16_MAX || pps_size > UINT16_MAX)
					return AVERROR_INVALIDDATA;

				avio_w8(pb, 1); /* version */
				avio_w8(pb, sps[1]); /* profile */
				avio_w8(pb, sps[2]); /* profile compat */
				avio_w8(pb, sps[3]); /* level */
				avio_w8(pb, 0xff); /* 6 bits reserved (111111) + 2 bits nal size length - 1 (11) */
				avio_w8(pb, 0xe1); /* 3 bits reserved (111) + 5 bits number of sps (00001) */

				avio_wb16(pb, sps_size);
				avio_write(pb, sps, sps_size);
				avio_w8(pb, 1); /* number of pps */
				avio_wb16(pb, pps_size);
				avio_write(pb, pps, pps_size);
				av_free(start);
		} else {
			avio_write(pb, data, len);
		}
	}
	return 0;
}

//==============================================================================
int annexb2mp4(const uint8_t *buf_in, uint8_t **buf_out,
	int *size, int filter_ps, int* ps_count)
{
	AVIOContext *pb;
	int ret;

	ret = avio_open_dyn_buf(&pb);
	if (ret < 0)
		return ret;

	ret   = ff_hevc_annexb2mp4(pb, buf_in, *size, filter_ps, ps_count);
	*size = avio_close_dyn_buf(pb, buf_out);

	return ret;
}

int annexb2avcc(const uint8_t *buf_in, uint8_t **buf_out,
	int *size)
{
	AVIOContext *pb;
	int ret;

	ret = avio_open_dyn_buf(&pb);
	if (ret < 0)
		return ret;

	ret   = ff_isom_write_avcc(pb, buf_in, *size);
	*size = avio_close_dyn_buf(pb, buf_out);

	return ret;
}