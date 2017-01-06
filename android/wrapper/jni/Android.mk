LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -O2

#ref tilengine
LOCAL_MODULE := tilengine
LOCAL_SRC_FILES := D:/Tilengine/lib/armeabi-v7a/libtilengine.so
LOCAL_EXPORT_C_INCLUDES := D:/Tilengine/lib
include $(PREBUILT_SHARED_LIBRARY)

#build tilengineJNI
include $(CLEAR_VARS)
LOCAL_MODULE    := tilengineJNI
LOCAL_SRC_FILES := D:/Tilengine/bindings/java/src_jni/TilengineJNI.c
LOCAL_SHARED_LIBRARIES := tilengine
include $(BUILD_SHARED_LIBRARY)
