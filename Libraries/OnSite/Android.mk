LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := OSFClient

JNI_PATH := ../../GameClient/GameClient/JNI
SDL_PATH := $(JNI_PATH)/SDL
LUA_PATH := $(JNI_PATH)/lua-5.1.5
PROTOB_PATH := $(LOCAL_PATH)/../protobuf-2.5.0

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
$(PROTOB_PATH)/src \
$(SDL_PATH)/include \
$(LUA_PATH)/src


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	ClientGameEngine.cpp \
	loaddroidlua.c \
	ClientInputManager.cpp \
	ClientLuaManager.cpp \
	ClientNetworkManager.cpp \
	ClientOutputManager.cpp \
	ClientRenderManager.cpp \
	ConcurrentQueue.cpp \
	GameObject.cpp \
	InputBuffer.cpp \
	MessagePacket.cpp \
	NetworkData.cpp \
	OutputBuffer.cpp \
	Texture.cpp \
	PPacket.pb.cc
#	Scene.cpp \
	
	
	
LOCAL_SHARED_LIBRARIES := SDL2 \
lua \
protobuf

LOCAL_CFLAGS += 
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
