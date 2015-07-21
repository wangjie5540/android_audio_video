///=============================================================================
/// @file :		video_codec.h
///	@brief :	视频编解码器接口头文件
/// @version :	1.0
/// @author :	letion
/// @date :		2014-10-29
///=============================================================================
#ifndef __MATRIX_VIDEO_CODEC_H__
#define __MATRIX_VIDEO_CODEC_H__

#include <cstdint>

//==============================================================================
#ifdef __cplusplus
extern "C"
{
#endif

//==============================================================================
// 定义调用约定
#ifdef _WIN32
#if defined(BUILDING_VIDEO_CODEC_SHARED) || defined(USING_VIDEO_CODEC_SHARED)
#define VIDEO_CODEC_API __stdcall
#else
#define VIDEO_CODEC_API
#endif
#else
#define VIDEO_CODEC_API
#endif

//==============================================================================
// 定义导出方式
#ifdef _WIN32
/* Windows - set up dll import/export decorators. */
# if defined(BUILDING_VIDEO_CODEC_SHARED)
/* Building shared library. */
#   define VIDEO_CODEC_EXTERN	__declspec(dllexport) 
# elif defined(USING_VIDEO_CODEC_SHARED)
/* Using shared library. */
#   define VIDEO_CODEC_EXTERN	__declspec(dllimport) 
# else
/* Building static library. */
#   define VIDEO_CODEC_EXTERN /* nothing */
# endif
#else //_WIN32
#   define VIDEO_CODEC_EXTERN /* nothing */
#endif

//==============================================================================
/// 视频编码句柄定义
typedef uint32_t video_encoder_t;
/// 视频解码句柄定义
typedef uint32_t video_decoder_t;
/// 定义无效的媒体句柄
const int32_t INVALID_VIDEO_CODEC = 0;

//==============================================================================
/// 视频编解码器类型
typedef enum VIDEO_CODEC_TYPE
{
	VIDEO_CODEC_H264 = 1,				///< H264编解码器
}video_codec_type_t;

/// 视频帧类型
typedef enum VIDEO_FRAME_TYPE
{
	VIDEO_FRAME_UNKNOWN = 0,			///< 未知
	VIDEO_FRAME_I = 1,					///< I帧
	VIDEO_FRAME_P = 2,					///< P帧
	VIDEO_FRAME_B = 3,					///< B帧
}video_frame_type_t;

//==============================================================================
/// @brief: 创建视频编码器
/// @param: video_codec_type_t codec_type 类型
/// @param: uint16_t width 视频宽度
/// @param: uint16_t height 视频高度
/// @param: uint16_t bitrate 编码码率
/// @param: uint16_t framerate 帧率
/// @return:视频编码器句柄
///=============================================================================
VIDEO_CODEC_EXTERN video_encoder_t VIDEO_CODEC_API 
	create_video_encoder(video_codec_type_t codec_type,
	uint16_t width, uint16_t height, uint16_t bitrate, uint16_t framerate);

//==============================================================================
/// @brief: 销毁视频编码器
/// @param: video_encoder_t handle 视频编码器句柄
/// @return:0-成功 other-失败
///=============================================================================
VIDEO_CODEC_EXTERN int32_t VIDEO_CODEC_API 
	destroy_video_encoder(video_encoder_t handle);

//==============================================================================
/// @brief: 获得扩展数据长度
/// @param: video_encoder_t handle 视频编码器句柄
/// @return:扩展数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_encoder_extradata_size(video_encoder_t handle);

//==============================================================================
/// @brief: 获得扩展数据
/// @param: video_encoder_t handle 视频编码器句柄
/// @param: uint8_t* extradata 扩展数据指针
/// @param: uint32_t extradata_size 扩展数据长度
/// @return:扩展数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_encoder_extradata(video_encoder_t handle, 
	uint8_t* extradata, uint32_t extradata_size);

//==============================================================================
/// @brief: 视频编码
/// @param: video_encoder_t handle 视频编码器句柄
/// @param: const uint8_t* in 待编码的数据
/// @param: uint32_t in_size 待编码的数据长度
/// @param: uint8_t* out 编码后的数据缓冲区
/// @param: uint32_t out_size 编码后的数据缓冲区长度
/// @param: bool* got_frame 指示是否获取了视频帧
/// @param: video_frame_type_t* type 视频帧类型
/// @return:编码后数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_encoder_encode(video_encoder_t handle, 
	const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size, 
	video_frame_type_t* type);

//==============================================================================
/// @brief: 创建视频解码器
/// @param: uint32_t codec_type 类型
/// @param: color_space_t color 输出数据的色彩空间类型
/// @param: uint16_t width 视频宽度
/// @param: uint16_t height 视频高度
/// @return:视频解码器句柄
///=============================================================================
VIDEO_CODEC_EXTERN video_decoder_t VIDEO_CODEC_API 
	create_video_decoder(uint32_t codec_type, uint16_t width, uint16_t height);

//==============================================================================
/// @brief: 销毁视频解码器
/// @param: video_encoder_t handle 视频解码器句柄
/// @return:0-成功 other-失败
///=============================================================================
VIDEO_CODEC_EXTERN int32_t VIDEO_CODEC_API 
	destroy_video_decoder(video_decoder_t handle);

//==============================================================================
/// @brief: 视频解码
/// @param: video_decoder_t handle 视频解码器句柄
/// @param: const uint8_t* in 待解码的数据
/// @param: uint32_t in_size 待解码的数据长度
/// @param: uint8_t* out 解码后的数据缓冲区
/// @param: uint32_t out_size 解码后的数据缓冲区长度
/// @return:解码后数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_decoder_decode(video_decoder_t handle, 
	const uint8_t* in, uint32_t in_size, 
	uint8_t* out, uint32_t out_size);

#ifdef __cplusplus
};
#endif

#endif //__MATRIX_VIDEO_CODEC_H__
