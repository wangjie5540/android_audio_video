/*
 * native_codec_center.h
 *
 *  Created on: 2015-9-14
 *      Author: wang
 */

#ifndef __NATIVE_H_
#define __NATIVE_H_

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL init(JNIEnv* env, jclass clasz,
							jstring logDir, jobject cb_object);
JNIEXPORT void JNICALL openStream(JNIEnv* env, jclass clasz,
					jstring channelId, jint videoWidth,
					jint videoHeight, jint videoFrameRate, jint videoDataRate,
					jint audioSampleRate, jint audioDataRate, jint audioChannelCount,
					jint audioSampleSize);
JNIEXPORT void JNICALL uploadVideoSample(JNIEnv* env, jclass clasz,
					jcharArray data, jint size,
					jlong timestamp);
JNIEXPORT void JNICALL uploadAudioSample(JNIEnv* env, jclass clasz,
					jcharArray data, jint size,
					jlong timestamp);
JNIEXPORT void JNICALL closeStream(JNIEnv* env, jclass clasz);
JNIEXPORT void JNICALL uninit(JNIEnv* env, jclass clasz);
JNIEXPORT void JNICALL setCallbackObject(JNIEnv* env, jclass clasz,
							jobject cb_object);

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_CODEC_CENTER_H */
