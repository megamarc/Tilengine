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

include $(CLEAR_VARS)

LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := libtilengine
LOCAL_SRC_FILES := $(LOCAL_PATH)/../lib/android/armeabi-v7a/libtilengine.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH := $(call my-dir)

LOCAL_MODULE     := libTilengineJNI
LOCAL_SRC_FILES  := ../../jni/TilengineJNI.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../lib
LOCAL_SHARED_LIBRARIES := libtilengine
include $(BUILD_SHARED_LIBRARY)
