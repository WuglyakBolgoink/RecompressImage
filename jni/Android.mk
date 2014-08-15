LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := nDCrotate
LOCAL_SRC_FILES := native_rotate.cpp
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDLIBS += -ljnigraphics
include $(BUILD_SHARED_LIBRARY)