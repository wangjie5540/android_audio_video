#include <unistd.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>

#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

#ifdef __ANDROID__

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#include "live_stream_uploader.h"
#include "video_codec.h"
#include "audio_codec.h"

///////////////////////////////////////////////////////////////////////////////

#define USE_LOG
#ifdef USE_LOG
  #define LOG_TAG "streamuploader"
  #define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
  #define LOGD(...)
#endif

///////////////////////////////////////////////////////////////////////////////
// Global Variables

const int MAX_BUFFER_SIZE = 1024*1024*10;

JavaVM *_jvm = NULL;
jclass _callback_class = NULL;
jmethodID _on_error_mid = NULL;

static video_encoder_t _video_encoder_handle = INVALID_VIDEO_CODEC;
static audio_encoder_t _audio_encoder_handle = INVALID_AUDIO_CODEC;

static int _video_width = 0;
static int _video_height = 0;

static vector<char> _video_out_buffer;
static vector<char> _video_convert_buffer;
static vector<char> _audio_out_buffer;

static bool _is_ready = false;

SwsContext *_convert_context = NULL;

///////////////////////////////////////////////////////////////////////////////
// class jni_callback_env

class jni_callback_env
{
public:
	jni_callback_env(JavaVM *jvm)
	{
		_jvm = jvm;
		_attached = false;
		_env = get_current_env();
	}

	~jni_callback_env()
	{
		release();
	}

	JNIEnv* env() { return _env; }

private:
	JNIEnv* get_current_env()
	{
		if (_jvm == NULL)
			return NULL;

		JNIEnv *result = NULL;
		bool attached = false;

		int status = _jvm->GetEnv((void**)&result, JNI_VERSION_1_4);
		if (status == JNI_EDETACHED)
		{
			status = _jvm->AttachCurrentThread(&result, NULL);
			attached = (status >= 0);
		}

		_attached = attached;
		return result;
	}

	void release()
	{
		if (_jvm != NULL && _attached)
			_jvm->DetachCurrentThread();
		_attached = false;
	}

private:
	JavaVM *_jvm;
	bool _attached;
	JNIEnv *_env;
};

///////////////////////////////////////////////////////////////////////////////
// Callbacks

static void liblsu_status_cb(LIBLSU_STATUS status, void *arg)
{
	switch (status)
	{
	case LIBLSU_STATUS_OPENED:
		{
			LOGD("ready!");
			_is_ready = true;
			break;
		}

	case LIBLSU_STATUS_CLOSED:
		{
			LOGD("closed!");
			_is_ready = false;
			break;
		}

	case LIBLSU_STATUS_ERROR:
		{
			int error_code = liblsu_get_last_error();
			LOGD("error: %d", error_code);

			_is_ready = false;

			jni_callback_env cb_env(_jvm);
			JNIEnv *env = cb_env.env();

			env->CallStaticVoidMethod(_callback_class, _on_error_mid, error_code);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

static bool init_callback_methods(JNIEnv *env)
{
	_callback_class = env->FindClass("com/example/lsudemo/VideoUploader");
	if (_callback_class != NULL)
		_on_error_mid = env->GetStaticMethodID(_callback_class, "onError", "(I)V");

	bool result = (_callback_class && _on_error_mid);
	if (result)
		LOGD("init jni callback ok!");
	else
		LOGD("init jni callback error!");

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// JNI Routines

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	_jvm = vm;

	JNIEnv *env;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
		return -1;

	init_callback_methods(env);

	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeInit(
	JNIEnv* env, jclass cls,
	jstring log_dir)
{
	const char *log_dir_str = env->GetStringUTFChars(log_dir, NULL);

	liblsu_init(
		MAX_BUFFER_SIZE,
		log_dir_str,
		liblsu_status_cb,
		NULL
		);

	env->ReleaseStringUTFChars(log_dir, log_dir_str);
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeOpen(
	JNIEnv* env, jclass cls,
	jstring channel_id,
	jint video_width,
	jint video_height,
	jint video_frame_rate,
	jint video_data_rate,
	jint audio_sample_rate,
	jint audio_data_rate,
	jint audio_channel_count,
	jint audio_sample_size)
{
	const char *channel_id_str = env->GetStringUTFChars(channel_id, NULL);

	_video_width = video_width;
	_video_height = video_height;

	// create encoders

	if (_video_encoder_handle != INVALID_VIDEO_CODEC)
		destroy_video_encoder(_video_encoder_handle);
	_video_encoder_handle = create_video_encoder(
		VIDEO_CODEC_H264, video_width, video_height, video_data_rate, video_frame_rate);
	if (_video_encoder_handle == INVALID_VIDEO_CODEC)
	{
		LOGD("create video encoder failed!");
		return;
	}

	if (_audio_encoder_handle != INVALID_VIDEO_CODEC)
		destroy_audio_encoder(_audio_encoder_handle);
	_audio_encoder_handle = create_audio_encoder(
		AUDIO_CODEC_AAC, audio_sample_rate, audio_channel_count, audio_sample_size, audio_data_rate);
	if (_audio_encoder_handle == INVALID_VIDEO_CODEC)
	{
		LOGD("create audio encoder failed!");
		return;
	}

	// open

	liblsu_video_meta_data vmd;
	liblsu_audio_meta_data amd;
	memset(&vmd, 0, sizeof(vmd));
	memset(&amd, 0, sizeof(amd));

	vmd.codec_type = LIBLSU_VCT_H264;
	vmd.width = video_width;
	vmd.height = video_height;
	vmd.frame_rate = video_frame_rate;
	vmd.data_rate = video_data_rate;
	vmd.extra_data_size = video_encoder_extradata_size(_video_encoder_handle);
	if (vmd.extra_data_size > 0)
	{
		vmd.extra_data = new char[vmd.extra_data_size];
		video_encoder_extradata(_video_encoder_handle, (uint8_t*)vmd.extra_data, vmd.extra_data_size);
	}

	amd.codec_type = LIBLSU_ACT_AAC;
	amd.sample_rate = audio_sample_rate;
	amd.data_rate = audio_data_rate;
	amd.channel_count = audio_channel_count;
	amd.sample_size = audio_sample_size;
	amd.extra_data_size = audio_encoder_extradata_size(_audio_encoder_handle);
	if (amd.extra_data_size > 0)
	{
		amd.extra_data = new char[amd.extra_data_size];
		audio_encoder_extradata(_audio_encoder_handle, (uint8_t*)amd.extra_data, amd.extra_data_size);
	}

	liblsu_open(
		channel_id_str,
		&amd,
		&vmd
		);

	delete[] vmd.extra_data;
	delete[] amd.extra_data;

	env->ReleaseStringUTFChars(channel_id, channel_id_str);
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeUploadVideoSample(
	JNIEnv* env, jclass cls,
	jcharArray data, jint size, jlong timestamp)
{
	if (!_is_ready)
		return;

	jchar *data_ptr = env->GetCharArrayElements(data, NULL);
	video_frame_type_t video_frame_type = VIDEO_FRAME_UNKNOWN;

	if (_video_out_buffer.size() < size)
		_video_out_buffer.resize(size);

	int image_size = avpicture_get_size(AV_PIX_FMT_YUV420P, _video_width, _video_height);

	if (_video_convert_buffer.size() < image_size)
		_video_convert_buffer.resize(image_size);

	AVPicture in_pic, out_pic;
	avpicture_fill(&in_pic, (unsigned char*)data_ptr, AV_PIX_FMT_NV21, _video_width, _video_height);
	avpicture_fill(&out_pic, (unsigned char*)&_video_convert_buffer[0], AV_PIX_FMT_YUV420P, _video_width, _video_height);

	if (!_convert_context)
	{
		_convert_context = sws_getContext(
			_video_width, _video_height, AV_PIX_FMT_NV21,
			_video_width, _video_height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	}

	sws_scale(_convert_context, (const uint8_t* const*)in_pic.data, in_pic.linesize, 0, _video_height,
			out_pic.data, out_pic.linesize);

	int out_size = video_encoder_encode(_video_encoder_handle, (unsigned char*)&_video_convert_buffer[0], image_size,
			(unsigned char*)&_video_out_buffer[0], _video_out_buffer.size(), &video_frame_type);
	bool is_key_frame = (video_frame_type == VIDEO_FRAME_I);

	liblsu_upload_video_frame(
		is_key_frame ? LIBLSU_FRAME_KEY : LIBLSU_FRAME_NON_KEY,
		timestamp,
		timestamp,
		(char*)&_video_out_buffer[0],
		out_size
		);

	//LOGD("write video. keyframe:%d, dts:%lld, size:%d", (int)is_key_frame, timestamp, out_size);

	env->ReleaseCharArrayElements(data, data_ptr, 0);
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeUploadAudioSample(
	JNIEnv* env, jclass cls,
	jcharArray data, jint size, jlong timestamp)
{
	if (!_is_ready)
		return;

	jchar *data_ptr = env->GetCharArrayElements(data, NULL);

	if (_audio_out_buffer.size() < size)
		_audio_out_buffer.resize(size);

	int out_size = audio_encoder_encode(_audio_encoder_handle, (unsigned char*)data_ptr, size,
		(unsigned char*)&_audio_out_buffer[0], _audio_out_buffer.size());

	liblsu_upload_audio_frame(timestamp, &_audio_out_buffer[0], out_size);

	//LOGD("write audio. size:%d", out_size);
	
	env->ReleaseCharArrayElements(data, data_ptr, 0);
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeClose(
	JNIEnv* env, jclass cls)
{
	liblsu_close();
}

JNIEXPORT void JNICALL Java_com_example_lsudemo_VideoUploader_nativeUninit(
	JNIEnv* env, jclass cls)
{
	_is_ready = false;

	if (_video_encoder_handle != INVALID_VIDEO_CODEC)
	{
		destroy_video_encoder(_video_encoder_handle);
		_video_encoder_handle = INVALID_VIDEO_CODEC;
	}

	if (_audio_encoder_handle != INVALID_VIDEO_CODEC)
	{
		destroy_audio_encoder(_audio_encoder_handle);
		_audio_encoder_handle = INVALID_VIDEO_CODEC;
	}

	liblsu_uninit();
}

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* ANDROID */
