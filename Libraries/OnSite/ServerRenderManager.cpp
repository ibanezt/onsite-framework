/**
 *	ServerRenderManager Class Definition
*	Terrell Ibanez
**/

//ServerRenderManager Class Declaration
#include "ServerRenderManager.hpp"

//C++ Libraries
#include <iostream>
#include <memory>

//SDL Libaries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

OnSite::ServerRenderManager::ServerRenderManager() {
	//TODO:Error Checking

	//Initialize SDL Video
	SDL_Init(SDL_INIT_VIDEO);
	
	//Set Texture Filtering to Linear
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	//Create Window
	//sdlWindow = SDL_CreateWindow("OnSite Framework", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	sdlWindow = SDL_CreateWindow("OnSite Framework", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);

	//Create Renderer with VSync Enabled
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (sdlRenderer == nullptr) {
		std::cout << "ERROR: RENDERER" << std::endl;
	}

	//Screen Size
	SCREEN_WIDTH = 1920;
	SCREEN_HEIGHT = 1080;
	
	//Set Logical Window Size
	SDL_RenderSetLogicalSize(sdlRenderer, 1920, 1080);

	//Set Renderer Color
	SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initalize SDL_Image
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cout << "ERROR LOADING SDL_IMAGE" << std::endl;
	}
}


OnSite::ServerRenderManager::~ServerRenderManager() {
	//Destroy Window
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
	
	//Quit SDL
	IMG_Quit();
	SDL_Quit();
}

void OnSite::ServerRenderManager::loadTexture() {
	textureAtlas = std::make_shared<OnSite::Texture>();
	textureAtlas->loadFile(sdlRenderer, "textures.png");
}

void OnSite::ServerRenderManager::render(std::tuple<std::tuple<double, double, double, double, double, double, double>*, size_t> tMaps) {
	//Clear Screen
	SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(sdlRenderer);
	
	std::tuple<double, double, double, double, double, double, double> *mapArray;
	size_t arrLength;
	
	//UnPack Tuple
	std::tie(mapArray, arrLength) = tMaps;
	//Render Current Frame
 	for (size_t i = 0; i < arrLength; ++i) {

		//Unpack Tuple
		double x, y, x2, y2, width, height, rotation;
		std::tie(x, y, x2, y2, width, height, rotation) = mapArray[i];
		textureAtlas->render(sdlRenderer, x, y, x2, y2, width, height, rotation);
	}
}

void OnSite::ServerRenderManager::updateWindow() {
	std::cout << "SCREEN UPDATE" << std::endl;
	//Update the Screen
	SDL_RenderPresent(sdlRenderer);
}

std::tuple<int, int> OnSite::ServerRenderManager::getScreenSize() {
	return std::make_tuple(SCREEN_WIDTH, SCREEN_HEIGHT);
}
