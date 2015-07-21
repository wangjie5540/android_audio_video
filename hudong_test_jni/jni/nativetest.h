/*
 * nativetest.h
 *
 *  Created on: 2015-6-26
 *      Author: wang
 */

#ifndef NATIVETEST_H_
#define NATIVETEST_H_

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL stringFromJNI(JNIEnv* env, jclass clasz, jint num);


#ifdef __cplusplus
}
#endif

#endif /* NATIVETEST_H_ */
