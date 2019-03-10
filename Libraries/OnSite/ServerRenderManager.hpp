/**
 *	ServerRenderManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_SERVERRENDERMANAGER_H
#define GUARD_SERVERRENDERMANAGER_H
//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//C++ Libraries
#include <memory>
#include <tuple>

//OnSite Libraries
#include "Texture.hpp"

namespace OnSite {
	class ServerRenderManager {
		public:
			ServerRenderManager();
			~ServerRenderManager();
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
			//Get Screen Size
			std::tuple<int, int> getScreenSize();
			//Screen Size
			int SCREEN_WIDTH;
			int SCREEN_HEIGHT;
	};
}
#endif
