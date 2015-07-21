LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := native_codec_center
LOCAL_SRC_FILES := native_codec_center.cpp
LOCAL_SRC_FILES += audio_codec.cpp
LOCAL_SRC_FILES += video_codec.cpp

LOCAL_LDLIBS    := -lm -llog

LOCAL_C_INCLUDES := sources/foo
include $(BUILD_SHARED_LIBRARY)