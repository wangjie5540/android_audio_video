#include <jni.h>
#include "nativetest.h"
#include <cstddef>
#include "assert.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef JNIREG_CLASS
#define JNIREG_CLASS "com/example/testjniapp/MainActivity"
#endif

//alert functions
static int registerNatives(JNIEnv*);


static JNINativeMethod gMethods[] = {
	{ "stringFromJNI", "(I)Ljava/lang/String;", (void*)stringFromJNI },//绑定
};

JNIEXPORT jstring JNICALL
Java_com_example_testjniapp_MainActivity_stringFromJNI(JNIEnv* env, jobject thiz, jint a)
{
	char str[100] = "hello, this is the first time to type it";
	char buf[100] = {0};
	sprintf(buf, "%d",a);
	return env->NewStringUTF(buf);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
		return JNI_FALSE;
	assert(env != NULL);

	if (registerNatives(env) != JNI_OK)
		return JNI_FALSE;

	return JNI_VERSION_1_4;
}

static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	clazz = (env)->FindClass(className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}

	if ((env)->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
	if (registerNativeMethods(env, JNIREG_CLASS, gMethods, (int)sizeof(gMethods)/sizeof(JNINativeMethod)))
		return JNI_FALSE;
	return JNI_TRUE;
}


//native methods are blow
JNIEXPORT jstring JNICALL stringFromJNI(JNIEnv* env, jclass clasz, jint num)
{
	return env->NewStringUTF("haha, wo you zhucele" + num);
}



#ifdef __cplusplus
}
#endif
