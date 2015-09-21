#ifndef __ANNEXB_TO_MP4_H__
#define __ANNEXB_TO_MP4_H__

#include <cstdint>

// annexbģʽתmp4ģʽ
int annexb2mp4(const uint8_t *buf_in, uint8_t **buf_out,
	int *size, int filter_ps, int* ps_count);

// annexbģʽתavccģʽ
int annexb2avcc(const uint8_t *buf_in, uint8_t **buf_out,
	int *size);

#endif //__ANNEXB_TO_MP4_H__
