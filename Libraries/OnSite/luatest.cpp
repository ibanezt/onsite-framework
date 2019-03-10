
//Lua Libraries

/*
extern "C" {
	#include "lua-5.1.5/src/lua.h"
	#include "lua-5.1.5/src/lualib.h"
	#include "lua-5.1.5/src/lauxlib.h"
}
*/

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <stdio.h>

/* Enums */

// Lua Expects 0 for a lot of things...

//Lua returns 0 on success for Load / pcall
#define LUA_ERRNONE 0
#define LUA_NOARGS 0
#define LUA_NOERRFUNC 0

int main(int argc, char **argv) {
	//Grab the Filename from Program Argument
	const char *file = argv[1];
	
	//Create Lua State
	lua_State *L = luaL_newstate();

	//Open Standard Lua Libraries
	luaL_openlibs(L);
	
	//Load File into Lua
	int rCode = luaL_loadfile(L, file);
	if (rCode == LUA_ERRNONE) {
		//Execute Lua Code, and Tell Lua to Push all Results onto Stack
		//(Including Potential Error)
		rCode = lua_pcall(L, LUA_NOARGS, LUA_MULTRET, LUA_NOERRFUNC);

		//Show Error, if Any
		if (rCode != LUA_ERRNONE) {
			printf("Error, %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else {
		printf("Error Loading File\n");
	}
		
	return 0;
}
