/*
 * native_codec_center.h
 *
 *  Created on: 2015-6-26
 *      Author: wang
 */

#ifndef NATIVE_CODEC_CENTER_H
#define NATIVE_CODEC_CENTER_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL stringFromJNI(JNIEnv* env, jclass clasz, jint num);
JNIEXPORT jlong JNICALL createAudioEncoder(JNIEnv* env, jclass clasz,
			jobject type, jint sample_rate, jint channel, jint sample_szie, jint bitrate);
JNIEXPORT jint JNICALL destroyAudioEncoder(JNIEnv* env, jclass clasz,
							jlong handle);
JNIEXPORT jlong JNICALL audioEncoderExtradataSize(JNIEnv* env, jclass clasz,
							jlong handle);
JNIEXPORT jlong JNICALL audioEncoderExtradata(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray extradataArray, jlong size);
JNIEXPORT jlong JNICALL audioEncoderEncode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray in, jlong in_size,
							jshortArray out, jlong out_size);
JNIEXPORT jlong JNICALL createAudioDecoder(JNIEnv* env, jclass clasz,
							jobject typeObj, jint sample_rate, jint channel, jint sample_size);
JNIEXPORT jint JNICALL destroyAudioDecoder(JNIEnv* env, jclass clasz,
							jint handle);
JNIEXPORT jlong JNICALL audioDecoderDecode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size);

JNIEXPORT jlong JNICALL createVideoEncoder(JNIEnv* env, jclass clasz,
							jobject typeObj, jint width, jint height, jint bitrate, jint framerate);
JNIEXPORT jint JNICALL destroyVideoEncoder(JNIEnv* env, jclass clasz,
							jlong handle);
JNIEXPORT jlong JNICALL videoEncoderExtradataSize(JNIEnv* env, jclass clasz,
							jlong handle);
JNIEXPORT jlong JNICALL videoEncoderExtradata(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray extradataArray, jlong extradata_size);
JNIEXPORT jlong JNICALL videoEncoderEncode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size, jobject typeObj);
JNIEXPORT jlong JNICALL createVideoDecoder(JNIEnv* env, jclass clasz,
							jlong codec_type, jint width, jint height);
JNIEXPORT jint JNICALL destroyVideoDecoder(JNIEnv* env, jclass clasz,
							jlong handle);
JNIEXPORT jlong JNICALL videoDecoderDecode(JNIEnv* env, jclass clasz,
							jlong handle, jshortArray inArray, jlong in_size,
							jshortArray outArray, jlong out_size);

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_CODEC_CENTER_H */
