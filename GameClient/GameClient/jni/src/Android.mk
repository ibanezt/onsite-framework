LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
LUA_PATH := ../lua-5.1.5
OSF_PATH := ../../../../Libraries/OnSite
#back 4


LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
$(LOCAL_PATH)/$(LUA_PATH)/src  \
$(LOCAL_PATH)/$(OSF_PATH)

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
$(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/src/*.cpp)) \
$(patsubst $(LOCAL_PATH)/%, %, $(wildcard $(LOCAL_PATH)/src/*.c)) \
main.c

LOCAL_SHARED_LIBRARIES := SDL2 \
lua \
OSFClient

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
include $(LOCAL_PATH)/$(OSF_PATH)/Android.mk
#include $(LOCAL_PATH)/../link.mk