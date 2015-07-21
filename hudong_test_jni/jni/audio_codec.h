///=============================================================================
/// @file :		audio_codec.h
///	@brief :	音频编解码器接口头文件
/// @version :	1.0
/// @author :	letion
/// @date :		2014-10-29
///=============================================================================
#ifndef __MATRIX_AUDIO_CODEC_H__
#define __MATRIX_AUDIO_CODEC_H__

#include <cstdint>

//==============================================================================
#ifdef __cplusplus
extern "C"
{
#endif

//==============================================================================
// 定义调用约定
#ifdef _WIN32
#if defined(BUILDING_AUDIO_CODEC_SHARED) || defined(USING_AUDIO_CODEC_SHARED)
#define AUDIO_CODEC_API __stdcall
#else
#define AUDIO_CODEC_API
#endif
#else
#define AUDIO_CODEC_API
#endif

//==============================================================================
// 定义导出方式
#ifdef _WIN32
/* Windows - set up dll import/export decorators. */
# if defined(BUILDING_AUDIO_CODEC_SHARED)
/* Building shared library. */
#   define AUDIO_CODEC_EXTERN	__declspec(dllexport) 
# elif defined(USING_AUDIO_CODEC_SHARED)
/* Using shared library. */
#   define AUDIO_CODEC_EXTERN	__declspec(dllimport) 
# else
/* Building static library. */
#   define AUDIO_CODEC_EXTERN /* nothing */
# endif
#else //_WIN32
#   define AUDIO_CODEC_EXTERN /* nothing */
#endif 

//==============================================================================
/// 音频编码句柄定义
typedef uint32_t audio_encoder_t;
/// 音频解码句柄定义
typedef uint32_t audio_decoder_t;
/// 定义无效的媒体句柄
const int32_t INVALID_AUDIO_CODEC = 0;

//==============================================================================
/// 音频编解码器类型
typedef enum AUDIO_CODEC_TYPE
{
	AUDIO_CODEC_AAC,			///< AAC编解码器
	AUDIO_CODEC_MP3,			///< MP3编解码器,only decoder
}audio_codec_type_t;

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
	uint16_t channel, uint16_t sample_size, uint16_t bitrate);

//==============================================================================
/// @brief: 销毁音频编码器
/// @param: audio_encoder_t handle 音频编码器句柄
/// @return:0-成功 other-失败
///=============================================================================
AUDIO_CODEC_EXTERN int32_t AUDIO_CODEC_API 
	destroy_audio_encoder(audio_encoder_t handle);

//==============================================================================
/// @brief: 获得扩展数据长度
/// @param: audio_encoder_t handle 音频编码器句柄
/// @return:扩展数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata_size(audio_encoder_t handle);

//==============================================================================
/// @brief: 获得扩展数据
/// @param: video_encoder_t handle 音频编码器句柄
/// @param: uint8_t* extradata 扩展数据指针
/// @param: uint32_t extradata_size 扩展数据长度
/// @return:扩展数据长度
///=============================================================================
AUDIO_CODEC_EXTERN uint32_t AUDIO_CODEC_API 
	audio_encoder_extradata(audio_encoder_t handle, 
	uint8_t* extradata, uint32_t extradata_size);

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
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size);

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
	uint16_t channel, uint16_t sample_size);

//==============================================================================
/// @brief: 销毁音频解码器
/// @param: audio_encoder_t handle 音频解码器句柄
/// @return:0-成功 other-失败
///=============================================================================
AUDIO_CODEC_EXTERN int32_t AUDIO_CODEC_API 
	destroy_audio_decoder(audio_decoder_t handle);

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
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size);

#ifdef __cplusplus
};
#endif

#endif //__MATRIX_AUDIO_CODEC_H__
