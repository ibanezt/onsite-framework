#include "loaddroidlua.h"

#if defined(__ANDROID__)
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif

//Lua - C Libraries
#if defined(__ANDROID__)
//extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
//}
#else
//extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
//}
#endif

#if defined(__ANDROID__)
//extern "C"
//{
	#include <android/log.h>
	#include <android/asset_manager.h>
	#include <android\asset_manager_jni.h>
	#include <jni.h>
//}
#endif


#define LUA_ERRNONE 0
#define LUA_NOARGS 0
#define LUA_NOERRFUNC 0

#if defined(__ANDROID__)
int loadLuaDroidR(lua_State *L, const char * filename)
{
	jmethodID mid;

	jobject context = (jobject)SDL_AndroidGetActivity();
	JNIEnv* env = Android_JNI_GetEnv();
	jobject assetManager;

	if(context != NULL)
	{
		__android_log_write(ANDROID_LOG_INFO, "TEST", "Context isn't null\n");
		mid = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, context), "getAssets", "()Landroid/content/res/AssetManager;");
		assetManager = (*env)->CallObjectMethod(env, context, mid);
	
		AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
	
		
		AAssetDir* assetDir = AAssetManager_openDir(mgr, "");
	
		/*
		while((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
		{
			__android_log_write(ANDROID_LOG_INFO, "TEST-fname", filename);
		}
		*/
	
		AAsset* asset = NULL;
		asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);

		char* luastr = malloc(sizeof(char)*100);
		int luastrc = 0;
		int luastrs = 100;
		char buf[20];
		int nb_read = 0;
		//empty str
		luastr[0] = '\0';

		while ((nb_read = AAsset_read(asset, buf, 20)) > 0)
		{
			__android_log_write(ANDROID_LOG_INFO, "LUATEST", buf);
			if(luastrs <= nb_read + luastrc)
			{
				luastr = realloc(luastr, sizeof(char)*(luastrs + nb_read +1));
				luastrs = (luastrs + nb_read +1);
			}
			int i = 0;
			for(i = 0; i < nb_read; i++)
			{
				sprintf(luastr, "%s%c", luastr, buf[i]);
			}
			luastrc += nb_read;
		}

		__android_log_write(ANDROID_LOG_INFO, "LUATEST", luastr);
		
		int rCode = luaL_loadstring (L, luastr);

		if(asset != NULL)
			AAsset_close(asset);
		if(assetDir != NULL)
			AAssetDir_close(assetDir);
		if(luastr != NULL)
			free(luastr);
		(*env)->DeleteLocalRef(env, context);
		
		return rCode;
	}
	return -1;
}
#else
int loadLuaDroidR(lua_State *L, const char * filename)
{
	return -1;
}
#endif