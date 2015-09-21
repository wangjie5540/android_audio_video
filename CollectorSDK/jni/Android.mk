# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#include all libs

#cmd-strip :=
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := depends/ffmpeg/lib/libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#cmd-strip :=
LOCAL_MODULE := lsu
LOCAL_SRC_FILES := depends/liblsu/lib/liblsu.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)


#build all cpp/c files
LOCAL_MODULE := streamuploader
CODECER_UPLOADER_FILES := $(wildcard $(LOCAL_PATH)/depends/codecer/src/*.cpp)
LOCAL_SRC_FILES := $(CODECER_UPLOADER_FILES)
#format the root folder
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
#add root folder files
LOCAL_SRC_FILES += streamuploader.cpp
#$(warning $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES += $(LOCAL_PATH)/depends/codecer/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/depends/ffmpeg/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/depends/liblsu/include
LOCAL_LDLIBS := -lm -llog
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS

LOCAL_SHARED_LIBRARIES := ffmpeg
LOCAL_SHARED_LIBRARIES += lsu

include $(BUILD_SHARED_LIBRARY)
