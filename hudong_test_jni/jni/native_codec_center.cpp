#include <jni.h>
#include "native_codec_center.h"
#include <cstddef>
#include "assert.h"
#include "stdio.h"
#include "audio_codec.h"
#include "video_codec.h"
#include "native_common_header.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JNIREG_CLASS
#define JNIREG_CLASS "com/example/hudong_test_jni/CodecCenter"
#endif

#ifndef SUB_CLASS
#define SUB_CLASS "com/example/hudong_test_jni/CodecCenter$AUDIO_CODEC_TYPE"
#endif

static int registerNatives(JNIEnv*);
static JNINativeMethod gMethods[] = {
		{ "stringFromJNI", "(I)Ljava/lang/String;", (void*) stringFromJNI },
		{"nativeCreateAudioEncoder","(Lcom/example/hudong_test_jni/CodecCenter$AUDIO_CODEC_TYPE;IIII)J",(void*) createAudioEncoder },
		{"nativeDestroyAudioEncoder", "(J)I", (void*)destroyAudioEncoder},
		{"nativeAudioEncoderExtradataSize", "(J)J", (void*)audioEncoderExtradataSize},
		{"nativeAudioEncoderExtradata", "(J[SJ)J", (void*)audioEncoderExtradata},
		{"nativeAudioEncoderEncode", "(J[SJ[SJ)J", (void*)audioEncoderEncode},
		{"nativeCreateAudioDecoder", "(Lcom/example/hudong_test_jni/CodecCenter$AUDIO_CODEC_TYPE;III)J", (void*)createAudioDecoder},
		{"nativeDestroyAudioDecoder", "(J)I", (void*)destroyAudioDecoder},
		{"nativeAudioDecoderDecode", "(J[SJ[SJ)J", (void*)audioDecoderDecode},
		{"nativeCreateVideoEncoder", "(Lcom/example/hudong_test_jni/CodecCenter$VIDEO_CODEC_TYPE;IIII)J", (void*)createVideoEncoder},
		{"nativeDestroyVideoEncoder", "(J)I", (void*)destroyVideoEncoder},
		{"nativeVideoEncoderExtradataSize", "(J)J", (void*)videoEncoderExtradataSize},
		{"nativeVideoEncoderExtradata", "(J[SJ)J", (void*)videoEncoderExtradata},
		{"nativeVideoEncoderEncode", "(J[SJ[SJLcom/example/hudong_test_jni/CodecCenter$VIDEO_FRAME_TYPE;)J", (void*)videoEncoderEncode},
		{"nativeCreateVideoDecoder", "(JII)J", (void*)createVideoDecoder},
		{"nativeDestroyVideoDecoder", "(J)I", (void*)destroyVideoDecoder},
		{"nativeVideoDecoderDecode", "(J[SJ[SJ)J", (void*)videoDecoderDecode},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
		return JNI_FALSE;
	assert(env != NULL);

	if (registerNatives(env) != JNI_OK)
		return JNI_FALSE;

	return JNI_VERSION_1_4;
}

static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* gMethods, int numMethods) {
	jclass clazz = NULL;
	clazz = (env)->FindClass(className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}

	if ((env)->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
	if (registerNativeMethods(env, JNIREG_CLASS, gMethods,
			(int) sizeof(gMethods) / sizeof(JNINativeMethod)))
		return JNI_FALSE;
	return JNI_TRUE;
}

static audio_codec_type_t getAudioCodecType(int type)
{
	switch (type) {
	case 0:
		return AUDIO_CODEC_AAC;
		break;
	case 1:
		return AUDIO_CODEC_MP3;
		break;
	default:
		return AUDIO_CODEC_AAC;
		break;
	}
}

static video_codec_type_t getVideoCodecType(int type)
{
	switch (type) {
	case 1:
		return VIDEO_CODEC_H264;
		break;
	default:
		return VIDEO_CODEC_H264;
		break;
	}
}

static video_frame_type_t getFrameCodecType(int type)
{
	switch (type) {
	case 0:
		return VIDEO_FRAME_UNKNOWN;
		break;
	case 1:
		return VIDEO_FRAME_I;
		break;
	case 2:
		return VIDEO_FRAME_P;
		break;
	case 3:
		return VIDEO_FRAME_B;
		break;
	default:
		return VIDEO_FRAME_UNKNOWN;
		break;
	}
}



//native methods are blow

JNIEXPORT jstring JNICALL stringFromJNI(JNIEnv* env, jclass clasz, jint num) {
	jclass clazz = NULL;
	clazz = (env)->FindClass(SUB_CLASS);
	if (clazz == NULL) {
		LOGD("wangtonggui test find sub_class error");
		return JNI_FALSE;
	}

//	jobject type = env->GetStaticObjectField(clazz, );
	return env->NewStringUTF("haha, wo you zhucele" + num);
}

JNIEXPORT jlong JNICALL createAudioEncoder(JNIEnv* env, jclass clasz,
		jobject typeObj, jint sample_rate, jint channel, jint sample_szie,
		jint bitrate) {
	jclass clazz = NULL;
	clazz = (env)->GetObjectClass(typeObj);
	if (clazz == NULL) {
		LOGD("GetObjectClass error");
		return JNI_FALSE;
	}
	jmethodID getType = (env)->GetMethodID(clazz, "type", "()I");
	jint type = env->CallIntMethod(typeObj, getType);
	audio_codec_type_t nativeType = getAudioCodecType(type);
	return create_audio_encoder(nativeType, sample_rate, channel, sample_szie, bitrate);
}

JNIEXPORT jint JNICALL destroyAudioEncoder(JNIEnv* env, jclass clasz,
							jlong handle)
{
	destroy_audio_encoder(handle);
}

JNIEXPORT jlong JNICALL audioEncoderExtradataSize(JNIEnv* env, jclass clasz,
							jlong handle)
{
	audio_encoder_extradata_size(handle);
}

JNIEXPORT jlong JNICALL audioEncoderExtradata(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray extradataArray, jlong size)
{
	jshort* extradataP = env->GetShortArrayElements(extradataArray, NULL);
	return audio_encoder_extradata(handle, (unsigned char *)extradataP, size);
}

JNIEXPORT jlong JNICALL audioEncoderEncode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size)
{
	jshort* in = env->GetShortArrayElements(inArray, NULL);
	jshort* out = env->GetShortArrayElements(outArray, NULL);
	return audio_encoder_encode(handle, (unsigned char *)in, in_size, (unsigned char *)out, out_size);
}

JNIEXPORT jlong JNICALL createAudioDecoder(JNIEnv* env, jclass clasz,
							jobject typeObj, jint sample_rate, jint channel, jint sample_size)
{
	jclass clazz = env->GetObjectClass(typeObj);
	if (clazz == NULL){
		LOGD("GetObjectClass error");
		return JNI_FALSE;
	}
	jmethodID getTypeId = env->GetMethodID(clazz, "type", "()I");
	jint type = env->CallIntMethod(typeObj, getTypeId);
	return create_audio_decoder(getAudioCodecType(type), sample_rate, channel, sample_size);
}

JNIEXPORT jint JNICALL destroyAudioDecoder(JNIEnv* env, jclass clasz,
							jint handle)
{
	return destroy_audio_decoder(handle);;
}

JNIEXPORT jlong JNICALL audioDecoderDecode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size)
{
	jshort* in = env->GetShortArrayElements(inArray, NULL);
	jshort* out = env->GetShortArrayElements(outArray, NULL);
	return audio_decoder_decode(handle, (unsigned char *)in, in_size, (unsigned char *)out, out_size);
}

JNIEXPORT jlong JNICALL createVideoEncoder(JNIEnv* env, jclass clasz,
							jobject typeObj, jint width, jint height, jint bitrate, jint framerate)
{
	jclass clazz = NULL;
	clazz = env->GetObjectClass(typeObj);
	if (clazz == NULL){
		LOGD("GetObjectClass error");
		return JNI_FALSE;
	}
	jmethodID getTypeMethodID = env->GetMethodID(clazz, "type", "()I");
	jint type = env->CallIntMethod(typeObj, getTypeMethodID);
	return create_video_encoder(getVideoCodecType(type), width, height, bitrate, framerate);
}


JNIEXPORT jint JNICALL destroyVideoEncoder(JNIEnv* env, jclass clasz,
							jlong handle)
{
	return destroy_video_encoder(handle);
}

JNIEXPORT jlong JNICALL videoEncoderExtradataSize(JNIEnv* env, jclass clasz,
							jlong handle)
{
	return video_encoder_extradata_size(handle);
}

JNIEXPORT jlong JNICALL videoEncoderExtradata(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray extradataArray, jlong extradata_size)
{
	jshort* extradata = env->GetShortArrayElements(extradataArray, NULL);
	return video_encoder_extradata(handle, (unsigned char*)extradata, extradata_size);
}

JNIEXPORT jlong JNICALL videoEncoderEncode(JNIEnv* env, jclass clasz,
									jlong handle, jshortArray inArray, jlong in_size,
									jshortArray outArray, jlong out_size, jobject typeObj)
{
	jshort* in = env->GetShortArrayElements(inArray, NULL);
	jshort* out = env->GetShortArrayElements(outArray, NULL);
	jclass clazz = env->GetObjectClass(typeObj);
	if (clazz == NULL){
		LOGD("GetObjectClass error");
		return JNI_FALSE;
	}
	jmethodID setTypeMethodId = env->GetMethodID(clazz, "setType", "(I)V");
	jmethodID typeMethodId = env->GetMethodID(clazz, "type", "()I");
	jint type = env->CallIntMethod(typeObj, typeMethodId);
	video_frame_type_t video_frame_type = getFrameCodecType(type);
	jlong ret = video_encoder_encode(handle, (unsigned char*)in, in_size, (unsigned char*)out, out_size, &video_frame_type);
	env->CallVoidMethod(typeObj,setTypeMethodId,video_frame_type);
	return ret;
}

JNIEXPORT jlong JNICALL createVideoDecoder(JNIEnv* env, jclass clasz,
							jlong codec_type, jint width, jint height)
{
	return create_video_decoder(codec_type, width, height);
}

JNIEXPORT jint JNICALL destroyVideoDecoder(JNIEnv* env, jclass clasz,
							jlong handle)
{
	return destroy_video_decoder(handle);
}

JNIEXPORT jlong JNICALL videoDecoderDecode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size)
{
	jshort* in = env->GetShortArrayElements(inArray, NULL);
	jshort* out = env->GetShortArrayElements(outArray, NULL);
	return video_decoder_decode(handle, (unsigned char*)in, in_size, (unsigned char*)out, out_size);
}

#ifdef __cplusplus
}
#endif
