/**
 *	ClientGameEngine Class Definition
 *	Terrell Ibanez
**/

//ClientGameEngine Class Declaration
#include "ClientGameEngine.hpp"

//SDL Libraries
#if defined(__ANDROID__)
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

//C++ Standard Libraries
#include <chrono>
#include <memory>
#include <thread>

//OnSite Libraries
#include "ClientRenderManager.hpp"
#include "ClientNetworkManager.hpp"
#include "ClientLuaManager.hpp"

OnSite::ClientGameEngine::ClientGameEngine() {
	//Note: Thread will not do anything until assigned
	mtGameLoop = std::thread();
	//Don't Start Game Yet
	mfRunGame = false;
	//No Scene
	msCurrScene = nullptr;
	//Lock to 30 FPS, measured in nanoseconds
	mnsMaxDeltaTime = std::chrono::nanoseconds(1000 * 1000 * 1000 / 30);
	//Time Elapsed = 0 ns
	mnsTimeElapsed = std::chrono::nanoseconds(0);

	//Setup Rendering
	mcrmRenderer = std::make_shared<OnSite::ClientRenderManager>();
	//Setup Networking
	mcnmNetwork = std::make_shared<OnSite::ClientNetworkManager>(); 
	//Setup Lua
	mclmLua = std::make_shared<OnSite::ClientLuaManager>();
}

OnSite::ClientGameEngine::~ClientGameEngine() {
	//Tell Thread to stop game
	mfRunGame = false;
	//Join Thread so that deconstructor won't deallocate
	//before thread finishes
	mtGameLoop.join();
}

void OnSite::ClientGameEngine::startClient() {
	mclmLua->loadCode();
	mcrmRenderer->loadTexture();
	//Allow GameLoop
	mfRunGame = true;
	//Start Thread
	mtGameLoop = std::thread(&ClientGameEngine::gameLoopThread, this);
}

void OnSite::ClientGameEngine::processNetworkInput(std::shared_ptr<OnSite::MessagePacket> pmPacket) {
	//Figure out Type of Packet
	switch(pmPacket->getPPacket()->type()) {
		//Lua Update
		case OnSite::PPT_UPDATE:
			//Pass to Lua Manager
			mclmLua->processIncomingMessagePacket(pmPacket);
		break;

		//File Part
		case OnSite::PPT_FILE:
			//Disregard, Server Shouldn't be Receiving Files
		break;

		//Control Signal
		case OnSite::PPT_CONTROL:
			//TODO
		break;

		default:
			//TODO: ERROR
		break;
	}
}

void OnSite::ClientGameEngine::gameLoopThread() {
	std::shared_ptr<OnSite::MessagePacket> pmPacket;
	SDL_Event sdlEvent;
	//Wait for Start
	while (mfRunGame) {
		//Save Frame Start Time Point
		mtpFrameStartTime = std::chrono::high_resolution_clock::now();
		//Receive Network Input
		std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIncoming= mcnmNetwork->getOutputQueue();
		pmPacket = pqIncoming->pop();
		while (pmPacket != nullptr) {
			processNetworkInput(pmPacket);
			pmPacket = pqIncoming->pop();
		}

		//SDL Input
		while (SDL_PollEvent(&sdlEvent) !=0) {
			if (sdlEvent.type == SDL_QUIT) {
				mfRunGame = false;
			}
		}
		//Run Lua Game Logic
		mclmLua->runGameLoop();
		//Process Callbacks
		//Process Game Engine Updates
		//Send Outgoing Network Changes
		mclmLua->sendOutgoingMessagePackets(mcnmNetwork->getOutputQueue());
		//Render to Screen Buffer
		render();	
		//Get Frame Stop Time Point
		mtpFrameStopTime = std::chrono::high_resolution_clock::now();
		//Check How Much Time Elapsed
		mnsTimeElapsed = mtpFrameStopTime - mtpFrameStartTime;

		//Don't Delay if We're Already over MaxDeltaTime
		if (mnsTimeElapsed <= mnsMaxDeltaTime) {
			//Delay for Remaining Time
			std::this_thread::sleep_for(mnsMaxDeltaTime - mnsTimeElapsed);
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(mnsMaxDeltaTime - mnsTimeElapsed).count() << std::endl;
		}
		else {
			//TODO: Log 
		}
		//Update Window
		mcrmRenderer->updateWindow();	
	}
}

void OnSite::ClientGameEngine::render() {
	mcrmRenderer->render(mclmLua->getTextureMaps());
}

bool OnSite::ClientGameEngine::checkFinished() {
	return !mfRunGame;
}
