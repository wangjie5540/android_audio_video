

#include <cstdint>
#include "audio_codec.h"
#include "native_common_header.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AUDIO_CODEC_EXTERN
#define AUDIO_CODEC_API


//==============================================================================
/// @brief: 创建音频编码器
/// @param: uint32_t codec_type 类型
/// @param: uint16_t sample_rate 采样频率
/// @param: uint16_t channel 频道数量
/// @param: uint16_t sample_size 采样精度
/// @param: uint16_t bitrate 码率
/// @return:音频编码器句柄
///=============================================================================
AUDIO_CODEC_EXTERN audio_encoder_t AUDIO_CODEC_API 
	create_audio_encoder(audio_codec_type_t codec_type, uint16_t sample_rate, 
	uint16_t channel, uint16_t sample_size, uint16_t bitrate)
{
	LOGD("wangtonggui test create_audio_encoder %d", codec_type);
}

//==============================================================================
/// @brief: 销毁音频编码器
/// @param: audio_encoder_t handle 音频编码器句柄
/// @return:0-成功 other-失败
///=============================================================================
AUDIO_CODEC_EXTERN int32_t AUDIO_CODEC_API 
	destroy_audio_encoder(audio_encoder_t handle)
{
	LOGD("wangtonggui test create_audio_encoder %d", handle);
}

//==============================================================================
/// @brief: 获得扩展数据长度
/// @param: audio_encoder_t handle 音频编码器句柄
/// @return:扩展数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata_size(audio_encoder_t handle)
{
	LOGD("wangtonggui test audio_encoder_extradata_size %d", handle);
}

//==============================================================================
/// @brief: 获得扩展数据
/// @param: video_encoder_t handle 音频编码器句柄
/// @param: uint8_t* extradata 扩展数据指针
/// @param: uint32_t extradata_size 扩展数据长度
/// @return:扩展数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata(audio_encoder_t handle, 
	uint8_t* extradata, uint32_t extradata_size)
{
	LOGD("wangtonggui test audio_encoder_extradata %d/%d/%d", handle, extradata[0], extradata_size);
	extradata[0] = 100;
}

//==============================================================================
/// @brief: 音频编码
/// @param: audio_encoder_t handle 音频编码器句柄
/// @param: const uint8_t* in 待编码的数据
/// @param: uint32_t in_size 待编码的数据长度
/// @param: uint8_t* out 编码后的数据缓冲区
/// @param: uint32_t out_size 编码后的数据缓冲区长度
/// @return:编码后数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_encoder_encode(audio_encoder_t handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	out[0] = 125;
	LOGD("audio_encoder_encode %d/%d/%d/%d/%d", handle, in[0], in_size, out[0], out_size);
}

//==============================================================================
/// @brief: 创建音频解码器
/// @param: uint32_t codec_type 类型
/// @param: uint16_t sample_rate 采样频率
/// @param: uint16_t channel 频道数量
/// @param: uint16_t sample_size 采样精度
/// @return:音频解码器句柄
///=============================================================================
AUDIO_CODEC_EXTERN audio_decoder_t AUDIO_CODEC_API 
	create_audio_decoder(audio_codec_type_t codec_type, uint16_t sample_rate, 
	uint16_t channel, uint16_t sample_size)
{
	
}

//==============================================================================
/// @brief: 销毁音频解码器
/// @param: audio_encoder_t handle 音频解码器句柄
/// @return:0-成功 other-失败
///=============================================================================
AUDIO_CODEC_EXTERN int32_t AUDIO_CODEC_API 
	destroy_audio_decoder(audio_decoder_t handle)
{
	
}

//==============================================================================
/// @brief: 音频解码
/// @param: audio_decoder_t handle 音频解码器句柄
/// @param: const uint8_t* in 待解码的数据
/// @param: uint32_t in_size 待解码的数据长度
/// @param: uint8_t* out 解码后的数据缓冲区
/// @param: uint32_t out_size 解码后的数据缓冲区长度
/// @return:解码后数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_decoder_decode(audio_decoder_t handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	
}

#ifdef __cplusplus
};
#endif
