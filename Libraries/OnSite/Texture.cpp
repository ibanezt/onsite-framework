/**
 *	Texture Class Declaration
 *	Terrell Ibanez
**/

//Texture Class Declaration
#include "Texture.hpp"

//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//C++ Libraries
#include <iostream>

OnSite::Texture::Texture() {
	msdlTexture = nullptr;
}

OnSite::Texture::~Texture() {

}

bool OnSite::Texture::loadFile(SDL_Renderer *sdlRenderer, const char* sPath) {
	//TODO: Error Checking

	//Destroy Existing Texture
	destroyTexture();

	SDL_Surface *sdlSurface = IMG_Load(sPath);
	if (sdlSurface == nullptr) {
		std::cout << "ERROR LOADING IMAGE" << std::endl;
	}
	SDL_SetColorKey(sdlSurface, SDL_TRUE, SDL_MapRGB(sdlSurface->format, 0, 0xFF, 0xFF));

	msdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
	mWidth = sdlSurface->w;
	mHeight = sdlSurface->h;
	SDL_FreeSurface(sdlSurface);
	return true;
}

void OnSite::Texture::render(SDL_Renderer *sdlRenderer, int x, int y, int x2, int y2, int width, int height, double rotation) {
	std::cout << x << " " << y << " " << x2 << " " << y2 << " " << width << " " << height << " " << rotation << std::endl;
	//Set Rendering Space
	SDL_Rect sdlQuad = {x, y, width, height};
	sdlQuad.w = width;
	sdlQuad.h = height;
	
	//Set Render Clipping
	SDL_Rect sdlClip = {x2, y2, width, height};

	//Render to Screen
	SDL_RenderCopyEx(sdlRenderer, msdlTexture, &sdlClip, &sdlQuad, rotation, NULL, SDL_FLIP_NONE);
}

void OnSite::Texture::destroyTexture() {
	if (msdlTexture != nullptr) {
		SDL_DestroyTexture(msdlTexture);
		msdlTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
}
