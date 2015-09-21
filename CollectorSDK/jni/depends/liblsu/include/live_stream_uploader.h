///============================================================================
/// @file :     live_stream_uploader.h
///	@brief :    暴风云视频直播上传模块接口
/// @version :  1.0
/// @date :     2015-08-19
///============================================================================

#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

//=============================================================================
// 调用约定及导出方式定义

#ifdef _WIN32
#  if defined(LIBLSU_BUILDING_SHARED) || defined(LIBLSU_USING_SHARED)
#    define LIBLSU_API __stdcall
#  else
#    define LIBLSU_API
#  endif
#else
#  define LIBLSU_API
#endif

#ifdef _WIN32
#  if defined(LIBLSU_BUILDING_SHARED)
#    define LIBLSU_EXTERN  __declspec(dllexport)
#  elif defined(LIBLSU_USING_SHARED)
#    define LIBLSU_EXTERN  __declspec(dllimport)
#  else
#    define LIBLSU_EXTERN /* nothing */
#  endif
#elif __GNUC__ >= 4
#  define LIBLSU_EXTERN __attribute__((visibility("default")))
#else
#  define LIBLSU_EXTERN /* nothing */
#endif

//=============================================================================
// 错误码定义

#define LIBLSU_NO_ERROR                    0
#define LIBLSU_INVALID_PARAM             (-1)   // 非法参数
#define LIBLSU_APPLY_UPLOAD_ADDR_FAILED  (-2)   // 申请上传地址失败
#define LIBLSU_CONNECT_UPLOAD_SRV_FAILED (-3)   // 连接上传服务器失败
#define LIBLSU_OPEN_CHANNEL_FAILED       (-4)   // 打开通道失败

//=============================================================================
// 类型定义

// 状态值
enum LIBLSU_STATUS
{
	LIBLSU_STATUS_OPENED = 1,           // 打开通道成功
	LIBLSU_STATUS_CLOSED = 2,           // 关闭通道成功
	LIBLSU_STATUS_ERROR  = 3,           // 发生错误
};

// 音频编码格式
enum LIBLSU_AUDIO_CODEC_TYPE
{
	LIBLSU_ACT_AAC = 0,                 // 音频 AAC 编码格式
	LIBLSU_ACT_MP3 = 1,                 // 音频 MP3 编码格式
};

// 视频编码格式
enum LIBLSU_VIDEO_CODEC_TYPE
{
	LIBLSU_VCT_H264 = 0,                // 视频 H264 编码格式
};

// 帧类型
enum LIBLSU_FRAME_TYPE
{
	LIBLSU_FRAME_NON_KEY = 0,           // 非关键帧
	LIBLSU_FRAME_KEY = 1,               // 关键帧
};

// 音频格式
struct liblsu_audio_meta_data
{
	LIBLSU_AUDIO_CODEC_TYPE codec_type; // 编码格式
	int32_t sample_rate;                // 音频采样率
	int32_t data_rate;                  // 音频码率 (bps)
	int16_t channel_count;              // 音频通道个数 (单声道/多声道)
	int8_t sample_size;                 // 音频采样大小 (即采样位数或采样精度)
	int16_t extra_data_size;            // 扩展数据长度
	char *extra_data;                   // 扩展数据 (仅AAC音频才有该字段)
};

// 视频格式
struct liblsu_video_meta_data
{
	LIBLSU_VIDEO_CODEC_TYPE codec_type; // 编码格式
	int16_t width;                      // 视频宽度
	int16_t height;                     // 视频高度
	int16_t frame_rate;                 // 视频帧率
	int32_t data_rate;                  // 视频码率 (bps)
	int16_t extra_data_size;            // 扩展数据长度
	char *extra_data;                   // 扩展数据 (仅H264视频才有该字段)
};

// 状态回调定义
typedef void (LIBLSU_API *liblsu_status_callback)(LIBLSU_STATUS status, void *arg);

//=============================================================================
// 接口定义

/**
 * 直播上传初始化
 * @param[in] max_buffer_size 最大缓存字节数 (超过将启用丢包策略, 为0表示无限制)
 * @param[in] log_dir 指定日志输出目录 (若目录不存在将自动创建)
 * @param[in] status_cb 状态回调 (打开关闭通道和出错等都通过状态回调通知调用者)
 * @param[in] status_cb_arg 状态回调的附加参数
 *
 * @remarks
 *  初始化过程立即完成，不会出错，也不会产生回调。
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_init(
	int max_buffer_size,
	const char *log_dir,
	liblsu_status_callback status_cb,
	void *status_cb_arg
	);

/**
 * 打开上传通道
 * @param[in] channel_id 直播频道ID
 * @param[in] audio_format 音频格式 (若不启用音频，可为 NULL)
 * @param[in] video_format 视频格式 (不可为 NULL)
 *
 * @remarks
 *  1. 调用此函数将执行上传前的准备工作 (选择服务器、连接服务器、打开通道)。
 *  2. 打开通道完成后通过 opened_cb 回调通知调用者，任何时候发生错误将触发 error_cb 回调。
 *  3. 调用者负责管理 audio_format 和 video_format 的内存分配和释放。函数返回后即可释放它们。
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_open(
	const char *channel_id,
	const liblsu_audio_meta_data *audio_format,
	const liblsu_video_meta_data *video_format
	);

/**
 * 上传音频帧数据
 * @param[in] dts  解码时间戳 (毫秒)
 * @param[in] data 帧数据
 * @param[in] size 帧数据字节数
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_upload_audio_frame(
	int dts, const char *data, int size);

/**
 * 上传视频帧数据
 * @param[in] frame_type 视频帧类型 (见 enum LIBLSU_FRAME_TYPE)
 * @param[in] dts  解码时间戳 (毫秒)
 * @param[in] pts  播放时间戳 (毫秒)
 * @param[in] data 帧数据
 * @param[in] size 帧数据字节数
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_upload_video_frame(
	LIBLSU_FRAME_TYPE frame_type, int dts, int pts, const char *data, int size);

/**
 * 关闭上传通道
 *
 * @remarks
 *  1. 调用此函数将关闭上传通道，不再接受后续上传数据。
 *  2. 关闭通道完成后通过 closed_cb 回调通知调用者，任何时候发生错误将触发 error_cb 回调。
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_close();

/**
 * 取得错误码
 */
LIBLSU_EXTERN int LIBLSU_API liblsu_get_last_error();

/**
 * 直播上传结束化
 */
LIBLSU_EXTERN void LIBLSU_API liblsu_uninit();

//=============================================================================

#ifdef __cplusplus
};
#endif
