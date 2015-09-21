#include <map>
#include <cassert>
#include "mutex.h"
#include "audio_codec.h"

extern "C" 
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavutil/samplefmt.h"
};

#ifndef ROUND
#define ROUND(x) ((uint64_t)((x) + 0.5))
#endif

//==============================================================================
typedef struct codec_handle
{
	AVCodecContext* m_codec_context;
	SwrContext*		m_resample_context;
}codec_handle_t;

//==============================================================================
static audio_encoder_t g_audio_encoder_id = rand();
static audio_decoder_t g_audio_decoder_id = rand();
static std::map<audio_encoder_t, codec_handle_t*> g_audio_encoder_map;
static std::map<audio_decoder_t, codec_handle_t*> g_audio_decoder_map;
static mutex g_audio_encoder_mutex; 
static mutex g_audio_decoder_mutex;
static bool g_init_audio_codec = false;

//==============================================================================
#ifdef __cplusplus
extern "C"
{
#endif

static codec_handle_t* create_encoder_handle(audio_codec_type_t codec_type, 
	uint16_t sample_rate, uint16_t channel, 
	uint16_t sample_size, uint16_t bitrate)
{
	assert(codec_type == AUDIO_CODEC_AAC);
	if(codec_type != AUDIO_CODEC_AAC)
	{
		av_log(NULL, AV_LOG_ERROR, "Audio codec only support aac encoder\n");
		return NULL;
	}

	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	assert(codec);
	if(NULL == codec)
		return NULL;

	codec_handle_t* codec_handle = 
		(codec_handle_t*)malloc(sizeof(codec_handle_t));
	assert(codec_handle);
	if(NULL == codec_handle)
		return NULL;

	memset(codec_handle, 0, sizeof(codec_handle_t));
	bool result = false;
	do 
	{
		// alloc context
		codec_handle->m_codec_context = avcodec_alloc_context3(codec);
		assert(codec_handle->m_codec_context);
		if(NULL == codec_handle->m_codec_context)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot alloc aac encoder context\n");
			break;
		}

		codec_handle->m_codec_context->codec_id = AV_CODEC_ID_AAC;
		codec_handle->m_codec_context->sample_rate = sample_rate;
		codec_handle->m_codec_context->channels = channel;
		codec_handle->m_codec_context->channel_layout = 
			av_get_default_channel_layout(channel);
		codec_handle->m_codec_context->bit_rate = bitrate * 1000;

		switch(sample_size)
		{
		case 8:
			codec_handle->m_codec_context->sample_fmt = AV_SAMPLE_FMT_U8;
			break;
		case 16:
			codec_handle->m_codec_context->sample_fmt = AV_SAMPLE_FMT_S16;
			break;
		case 32:
			codec_handle->m_codec_context->sample_fmt = AV_SAMPLE_FMT_S32;
			break;
		}

		// 
		codec_handle->m_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;

		/* open the codec */
		int ret = avcodec_open2(codec_handle->m_codec_context, codec, NULL);
		if (ret < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot open aac encoder context\n");
			break;
		}	
		result = true;
	}
	while(false);

	if(!result)
	{
		if(NULL != codec_handle->m_codec_context)
		{
			avcodec_free_context(&codec_handle->m_codec_context);
			codec_handle->m_codec_context = NULL;
		}

		free(codec_handle);
		codec_handle = NULL;
	}

	return codec_handle;
}

static codec_handle_t* create_decoder_handle(audio_codec_type_t codec_type, 
	uint16_t sample_rate, uint16_t channel, uint16_t sample_size)
{
	enum AVCodecID codec_id;
	switch(codec_type)
	{
	case AUDIO_CODEC_AAC:
		codec_id = AV_CODEC_ID_AAC;
		break;
	case AUDIO_CODEC_MP3:
		codec_id = AV_CODEC_ID_MP3;
		break;
	default:
		av_log(NULL, AV_LOG_ERROR, "Audio decoder only support aac and mp3\n");
		return NULL;
	}

	AVCodec* codec = avcodec_find_decoder(codec_id);
	assert(codec);
	if(NULL == codec)
		return NULL;

	codec_handle_t* codec_handle = 
		(codec_handle_t*)malloc(sizeof(codec_handle_t));
	assert(codec_handle);
	if(NULL == codec_handle)
		return NULL;

	memset(codec_handle, 0, sizeof(codec_handle_t));
	bool result = false;
	do 
	{
		// alloc context
		codec_handle->m_codec_context = avcodec_alloc_context3(codec);
		assert(codec_handle->m_codec_context);
		if(NULL == codec_handle->m_codec_context)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot alloc audio decoder context\n");
			break;
		}

		/* open the codec */
		int ret = avcodec_open2(codec_handle->m_codec_context, codec, NULL);
		if (ret < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder context\n");
			break;
		}

		// 创建重采样句柄
		int64_t channel_layout = av_get_default_channel_layout(channel);
		int     sample_rate = codec_handle->m_codec_context->sample_rate;
		enum AVSampleFormat in_format  = codec_handle->m_codec_context->sample_fmt;
		enum AVSampleFormat out_format = AV_SAMPLE_FMT_S16;

		codec_handle->m_resample_context = swr_alloc_set_opts(
			NULL, channel_layout, out_format, sample_rate, 
			channel_layout, in_format, sample_rate, 0, NULL);
		if(NULL == codec_handle->m_resample_context)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot alloc resample context\n");
			break;
		}

		if(swr_init(codec_handle->m_resample_context) < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Init resample context failed\n");
			break;
		}

		result = true;
	} while (false);

	if(!result)
	{
		if(NULL != codec_handle->m_resample_context)
		{
			avcodec_close(codec_handle->m_codec_context);
			swr_free(&codec_handle->m_resample_context); 
			codec_handle->m_resample_context = NULL;
		}

		if(NULL != codec_handle->m_codec_context)
		{
			avcodec_free_context(&codec_handle->m_codec_context);
			codec_handle->m_codec_context = NULL;
		}

		free(codec_handle);
		codec_handle = NULL;
	}

	return codec_handle;
}

static void destroy_codec_handle(codec_handle_t* codec_handle)
{
	assert(codec_handle);
	if(NULL != codec_handle)
	{
		if(NULL != codec_handle->m_resample_context)
		{
			swr_free(&codec_handle->m_resample_context); 
			codec_handle->m_resample_context = NULL;
		}

		if(NULL != codec_handle->m_codec_context)
		{
			avcodec_close(codec_handle->m_codec_context);
			avcodec_free_context(&codec_handle->m_codec_context);
			codec_handle->m_codec_context = NULL;
		}

		free(codec_handle);
		codec_handle = NULL;
	}
}

static uint32_t codec_handle_encode(codec_handle_t* codec_handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	assert(codec_handle);
	if(NULL == codec_handle || NULL == codec_handle->m_codec_context)
		return 0;

	// 数据长度检查	
	uint32_t size = av_samples_get_buffer_size(NULL, 
		codec_handle->m_codec_context->channels,
		codec_handle->m_codec_context->frame_size, 
		codec_handle->m_codec_context->sample_fmt, 1); 
	assert(size == in_size);
	if(size != in_size)
		return 0;

	// 分配帧内存
	AVFrame* frame = av_frame_alloc();//avcodec_alloc_frame();
	assert(frame);
	if(NULL == frame)
		return 0;

	frame->nb_samples= codec_handle->m_codec_context->frame_size;  
	frame->format= codec_handle->m_codec_context->sample_fmt;  

	// 填充音频帧
	avcodec_fill_audio_frame(frame, codec_handle->m_codec_context->channels, 
		codec_handle->m_codec_context->sample_fmt, 
		(const uint8_t*)in, in_size, 1); 

	AVPacket packet;  
	av_init_packet(&packet);  

	packet.data = (uint8_t*)out;
	packet.size = out_size;

	uint32_t result = 0;
	do 
	{
		// 音频编码 
		int got_frame = 0;  
		int ret = avcodec_encode_audio2(codec_handle->m_codec_context, 
			&packet, frame, &got_frame);  
		if(ret < 0)  
		{
			av_log(NULL, AV_LOG_ERROR, "audio encode data failed\n");
			break;
		}

		if (got_frame)  
		{  
			result = packet.size;
		}

	} while (false);

	av_free(frame);
	av_free_packet(&packet);
	return result;
}

static uint32_t codec_handle_decode(codec_handle_t* codec_handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	assert(codec_handle);
	if(NULL == codec_handle || NULL == codec_handle->m_codec_context)
		return 0;

	AVPacket packet;
	av_init_packet(&packet);
	packet.data = (uint8_t*)in;
	packet.size = in_size;

	// 分配帧内存
	AVFrame* frame = av_frame_alloc();//avcodec_alloc_frame();
	assert(frame);
	if(NULL == frame)
		return 0;

	uint32_t result = 0;
	do 
	{
		// 音频解码
		int got_frame = 0;
		int ret = avcodec_decode_audio4(codec_handle->m_codec_context, frame, 
			&got_frame, &packet);
		if(ret < 0)  
		{
			av_log(NULL, AV_LOG_ERROR, "audio decode data failed\n");
			break;
		}

		if(got_frame)  
		{  
			int nb_samples = frame->nb_samples;
			int channels = frame->channels;
			int sample_size = nb_samples * channels * 2;

			uint8_t** buffer = NULL;
			int buffer_size = 0;
			if(av_samples_alloc_array_and_samples(&buffer, &buffer_size, 
				channels, nb_samples, AV_SAMPLE_FMT_S16, 0) < 0)
			{
				av_log(NULL, AV_LOG_ERROR, "audio decode alloc data failed\n");
				break;
			}

			if(swr_convert(codec_handle->m_resample_context, buffer, nb_samples, 
				(const uint8_t **)&frame->data, nb_samples) < 0)
			{
				av_freep(&buffer[0]);
				av_freep(&buffer);
				av_log(NULL, AV_LOG_ERROR, "audio decode resample data failed\n");
				break;
			}

			assert(out_size >= buffer_size);
			if(out_size >= buffer_size)
			{
				memcpy(out, buffer[0], buffer_size);
				av_freep(&buffer[0]);
				av_freep(&buffer);
				result = buffer_size; 
			}
		}
	} while (false);

	av_free(frame);
	av_free_packet(&packet);
	return result;
}

audio_encoder_t AUDIO_CODEC_API 
	create_audio_encoder(audio_codec_type_t codec_type, uint16_t sample_rate, 
	uint16_t channel, uint16_t sample_size, uint16_t bitrate)
{
	if(!g_init_audio_codec)
	{
		/* register all the codecs */  
		avcodec_register_all();  
		g_init_audio_codec = true;
	}

	audio_encoder_t encoder_id = INVALID_AUDIO_CODEC;
	codec_handle_t* codec_handle = create_encoder_handle(codec_type,
		sample_rate, channel, sample_size, bitrate);
	if(NULL != codec_handle)
	{
		auto_lock lock(g_audio_encoder_mutex);
		// 此处未检查id是否重复，是基于id回绕的可能性几乎为零的前提下
		encoder_id = ++g_audio_encoder_id;
		g_audio_encoder_map[encoder_id] = codec_handle;
	}
	return encoder_id;
}

int32_t AUDIO_CODEC_API 
	destroy_audio_encoder(audio_encoder_t handle)
{
	int32_t result = -1;

	auto_lock lock(g_audio_encoder_mutex);
	std::map<audio_encoder_t, codec_handle_t*>::iterator iter = 
		g_audio_encoder_map.find(handle);
	if(iter != g_audio_encoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			destroy_codec_handle(codec_handle);
			codec_handle = NULL;
		}
		g_audio_encoder_map.erase(iter);
	}
	return result;
}

uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata_size(audio_encoder_t handle)
{
	uint32_t result = 0;

	auto_lock lock(g_audio_encoder_mutex);
	std::map<audio_encoder_t, codec_handle_t*>::iterator iter = 
		g_audio_encoder_map.find(handle);
	if(iter != g_audio_encoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			result = codec_handle->m_codec_context->extradata_size;
		}
	}
	return result;
}

uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata(audio_encoder_t handle, 
	uint8_t* extradata, uint32_t extradata_size)
{
	assert(extradata);
	assert(extradata_size);
	if(NULL == extradata || 0 == extradata_size)
		return 0;

	uint32_t result = 0;

	auto_lock lock(g_audio_encoder_mutex);
	std::map<audio_encoder_t, codec_handle_t*>::iterator iter = 
		g_audio_encoder_map.find(handle);
	if(iter != g_audio_encoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			uint32_t size = codec_handle->m_codec_context->extradata_size;
			if(size <= extradata_size)
			{
				memcpy(extradata, codec_handle->m_codec_context->extradata, size);
				result = size;
			}
		}
	}
	return result;
}

uint32_t AUDIO_CODEC_API 
	audio_encoder_encode(audio_encoder_t handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	uint32_t result = 0;

	auto_lock lock(g_audio_encoder_mutex);
	std::map<audio_encoder_t, codec_handle_t*>::iterator iter = 
		g_audio_encoder_map.find(handle);
	if(iter != g_audio_encoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			result = codec_handle_encode(codec_handle, in, in_size, out, out_size);
		}
	}
	return result;
}

audio_decoder_t AUDIO_CODEC_API 
	create_audio_decoder(audio_codec_type_t codec_type, uint16_t sample_rate, 
	uint16_t channel, uint16_t sample_size)
{
	if(!g_init_audio_codec)
	{
		/* register all the codecs */  
		avcodec_register_all();  
		g_init_audio_codec = true;
	}

	audio_decoder_t decoder_id = INVALID_AUDIO_CODEC;
	codec_handle_t* codec_handle = create_decoder_handle(
		codec_type, sample_rate, channel, sample_size);
	if(NULL != codec_handle)
	{
		auto_lock lock(g_audio_decoder_mutex);
		// 此处未检查id是否重复，是基于id回绕的可能性几乎为零的前提下
		decoder_id = ++g_audio_decoder_id;
		g_audio_decoder_map[decoder_id] = codec_handle;
	}
	return decoder_id;
}

int32_t AUDIO_CODEC_API 
	destroy_audio_decoder(audio_decoder_t handle)
{
	int32_t result = -1;

	auto_lock lock(g_audio_decoder_mutex);
	std::map<audio_decoder_t, codec_handle_t*>::iterator iter = 
		g_audio_decoder_map.find(handle);
	if(iter != g_audio_decoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			destroy_codec_handle(codec_handle);
			codec_handle = NULL;
		}
		g_audio_decoder_map.erase(iter);
	}
	return result;
}

uint32_t AUDIO_CODEC_API 
	audio_decoder_decode(audio_decoder_t handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	uint32_t result = 0;

	auto_lock lock(g_audio_decoder_mutex);
	std::map<audio_decoder_t, codec_handle_t*>::iterator iter = 
		g_audio_decoder_map.find(handle);
	if(iter != g_audio_decoder_map.end())
	{
		codec_handle_t* codec_handle = iter->second;
		assert(codec_handle);
		if(NULL != codec_handle)
		{
			result = codec_handle_decode(codec_handle, 
				in, in_size, out, out_size);
		}
	}
	return result;
}
//==============================================================================
#ifdef __cplusplus
}
#endif
