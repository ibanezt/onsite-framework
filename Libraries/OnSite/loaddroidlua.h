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
//extern "C" {
	#include <android/log.h>
	#include <android/asset_manager.h>
	#include <android\asset_manager_jni.h>
	#include <jni.h>
//}
#endif


#define LUA_ERRNONE 0
#define LUA_NOARGS 0
#define LUA_NOERRFUNC 0


int loadLuaDroidR(lua_State *L, const char * filename);
