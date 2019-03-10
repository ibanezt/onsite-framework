/**
 *	Texture Class Declaration
 *	Terrell Ibanez
**/

#ifndef GUARD_TEXTURE_H
#define GUARD_TEXTURE_H
//C++ Libraries
#include <string>

//SDL Libraries
#if defined(__ANDROID__)
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

namespace OnSite {
	class Texture {
		public:
			Texture();
			~Texture();
			//Load Image from Path 
			bool loadFile(SDL_Renderer *sdlRenderer, const char *sPath);

			//Render to Screen
			void render(SDL_Renderer *sdlRenderer, int x, int y, int x2, int y2, int width, int height, double rotation);

			//Destroy Associated Texture
			void destroyTexture();

		private:
			SDL_Texture *msdlTexture;
			int mWidth;
			int mHeight;
	};
}

#endif
