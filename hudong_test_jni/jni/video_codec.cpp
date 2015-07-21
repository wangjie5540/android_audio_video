#include "video_codec.h"

#ifdef __cplusplus
extern "C"{
#endif

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
	uint16_t width, uint16_t height, uint16_t bitrate, uint16_t framerate)
{

}

//==============================================================================
/// @brief: 销毁视频编码器
/// @param: video_encoder_t handle 视频编码器句柄
/// @return:0-成功 other-失败
///=============================================================================
VIDEO_CODEC_EXTERN int32_t VIDEO_CODEC_API 
	destroy_video_encoder(video_encoder_t handle)
{
	
}	

//==============================================================================
/// @brief: 获得扩展数据长度
/// @param: video_encoder_t handle 视频编码器句柄
/// @return:扩展数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_encoder_extradata_size(video_encoder_t handle)
{
	
}	

//==============================================================================
/// @brief: 获得扩展数据
/// @param: video_encoder_t handle 视频编码器句柄
/// @param: uint8_t* extradata 扩展数据指针
/// @param: uint32_t extradata_size 扩展数据长度
/// @return:扩展数据长度
///=============================================================================
VIDEO_CODEC_EXTERN uint32_t VIDEO_CODEC_API 
	video_encoder_extradata(video_encoder_t handle, 
	uint8_t* extradata, uint32_t extradata_size)
{
	
}	

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
	video_frame_type_t* type)
{
	*type = VIDEO_FRAME_B;
}	

//==============================================================================
/// @brief: 创建视频解码器
/// @param: uint32_t codec_type 类型
/// @param: color_space_t color 输出数据的色彩空间类型
/// @param: uint16_t width 视频宽度
/// @param: uint16_t height 视频高度
/// @return:视频解码器句柄
///=============================================================================
VIDEO_CODEC_EXTERN video_decoder_t VIDEO_CODEC_API 
	create_video_decoder(uint32_t codec_type, uint16_t width, uint16_t height)
{
	
}	

//==============================================================================
/// @brief: 销毁视频解码器
/// @param: video_encoder_t handle 视频解码器句柄
/// @return:0-成功 other-失败
///=============================================================================
VIDEO_CODEC_EXTERN int32_t VIDEO_CODEC_API 
	destroy_video_decoder(video_decoder_t handle)
{
	
}	

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
	uint8_t* out, uint32_t out_size)
{
	
}	

#ifdef __cplusplus
}
#endif
