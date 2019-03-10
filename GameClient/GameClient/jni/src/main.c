#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#include <android/log.h>
#include <android/asset_manager.h>
#include <android\asset_manager_jni.h>
#include <jni.h>

#include "SDL.h"

//extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
//}

#include <stdio.h>

/* Enums */

// Lua Expects 0 for a lot of things...

//Lua returns 0 on success for Load / pcall
#define LUA_ERRNONE 0
#define LUA_NOARGS 0
#define LUA_NOERRFUNC 0

//externally defined get

typedef struct Sprite
{
	SDL_Texture* texture;
	Uint16 w;
	Uint16 h;
} Sprite;

/* Adapted from SDL's testspriteminimal.c */
Sprite LoadSprite(const char* file, SDL_Renderer* renderer)
{
	Sprite result;
	result.texture = NULL;
	result.w = 0;
	result.h = 0;
	
    SDL_Surface* temp;

    /* Load the sprite image */
    temp = SDL_LoadBMP(file);
    if (temp == NULL)
	{
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return result;
    }
    result.w = temp->w;
    result.h = temp->h;

    /* Create texture from the image */
    result.texture = SDL_CreateTextureFromSurface(renderer, temp);
    if (!result.texture) {
        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(temp);
        return result;
    }
    SDL_FreeSurface(temp);

    return result;
}

void draw(SDL_Window* window, SDL_Renderer* renderer, const Sprite sprite)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Rect destRect = {w/2 - sprite.w/2, h/2 - sprite.h/2, sprite.w, sprite.h};
	/* Blit the sprite onto the screen */
	SDL_RenderCopy(renderer, sprite.texture, NULL, &destRect);
}

int loadLuaDroidA(lua_State *L, const char * filename)
{
	jmethodID mid;

	jobject context = (jobject)SDL_AndroidGetActivity();
	JNIEnv* env = Android_JNI_GetEnv();
	jobject assetManager;
	
	if(context != NULL)
	{
		__android_log_write(ANDROID_LOG_INFO, "TEST", "Context isn't null\n");
		mid = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, context),
            "getAssets", "()Landroid/content/res/AssetManager;");
		assetManager = (*env)->CallObjectMethod(env, context, mid);
		
		AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
		
		//(*env)->DeleteLocalRef(actv);
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
			
		return rCode;
	}
	return -1;
}

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    __android_log_write(ANDROID_LOG_INFO, "TEST", "Testing\n");

    //Create Lua State
    lua_State *L = luaL_newstate();

    //Open Standard Lua Libraries
    luaL_openlibs(L);
	
	//Load File into Lua
	int rCode = loadLuaDroidA (L, "greet.lua");
	
	char outstr2[5];
	sprintf(outstr2, "rCode : %d\n", rCode);
	__android_log_write(ANDROID_LOG_INFO, "LUA", outstr2);
    
	if (rCode == LUA_ERRNONE) {
		//Execute Lua Code, and Tell Lua to Push all Results onto Stack
		//(Including Potential Error)
		rCode = lua_pcall(L, LUA_NOARGS, LUA_MULTRET, LUA_NOERRFUNC);
		//Show Error, if Any
		if (rCode != LUA_ERRNONE) {
				char outstr[100];
				sprintf(outstr, "Error, %s\n", lua_tostring(L, -1));
				__android_log_write(ANDROID_LOG_INFO, "LUA", outstr);
				lua_pop(L, 1);
		}
		else
		{
			outstr2[0] = '\0';
			lua_getglobal(L, "ret");
			sprintf(outstr2, "%d", (int)lua_tonumber(L, -1));
			
			__android_log_write(ANDROID_LOG_INFO, "LUA", "looks like it worked\n");
			__android_log_write(ANDROID_LOG_INFO, "LUA", outstr2);
		}
	}
	else {
		//printf("Error Loading File\n");
		__android_log_write(ANDROID_LOG_INFO, "LUA", "Error Loading File.\n");
	}
		
    if(SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer) < 0)
        exit(2);

	Sprite sprite = LoadSprite("image.bmp", renderer);
    if(sprite.texture == NULL)
        exit(2);

    /* Main render loop */
    Uint8 done = 0;
    SDL_Event event;
    while(!done)
	{
        /* Check for events */
        while(SDL_PollEvent(&event))
		{
            if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_FINGERDOWN)
			{
                done = 1;
            }
        }
		
		
		/* Draw a gray background */
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
		
		draw(window, renderer, sprite);
	
		/* Update the screen! */
		SDL_RenderPresent(renderer);
		
		SDL_Delay(10);
    }



    exit(0);
}
