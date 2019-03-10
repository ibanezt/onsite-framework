/**
 *	ClientRenderManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_CLIENTRENDERMANAGER_H
#define GUARD_CLIENTRENDERMANAGER_H
//SDL Libraries

#if defined(__ANDROID__)
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

//C++ Libraries
#include <memory>
#include <tuple>

//OnSite Libraries
#include "Texture.hpp"

namespace OnSite {
	class ClientRenderManager {
		public:
			ClientRenderManager();
			~ClientRenderManager();
			//Load Texture
			void loadTexture();
			//Render to Buffer
			void render(std::tuple<std::tuple<double, double, double, double, double, double, double>*, size_t> tMaps);
			//Update - Flush Buffer Contents to Window
			void updateWindow();
		private:
			//Window
			SDL_Window *sdlWindow;
			//Window Renderer
			SDL_Renderer *sdlRenderer;
			
			//Texture Atlas
			std::shared_ptr<OnSite::Texture> textureAtlas;

			//Screen Size
			int SCREEN_WIDTH;
			int SCREEN_HEIGHT;
	};
}
#endif
