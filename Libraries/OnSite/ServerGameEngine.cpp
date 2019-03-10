/**
 *	ServerGameEngine Class Definition
 *	Terrell Ibanez
**/

//ServerGameEngine Class Declaration
#include "ServerGameEngine.hpp"

//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//C++ Standard Libraries
#include <chrono>
#include <memory>
#include <thread>

//OnSite Libraries
#include "ServerRenderManager.hpp"
#include "ServerNetworkManager.hpp"
#include "ServerLuaManager.hpp"

OnSite::ServerGameEngine::ServerGameEngine() {
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
	msrmRenderer = std::make_shared<OnSite::ServerRenderManager>();
	//Setup Networking
	msnmNetwork = std::make_shared<OnSite::ServerNetworkManager>(); 
	//Setup Lua
	mslmLua = std::make_shared<OnSite::ServerLuaManager>();
}

OnSite::ServerGameEngine::~ServerGameEngine() {
	//Tell Thread to stop game
	mfRunGame = false;
	//Join Thread so that deconstructor won't deallocate
	//before thread finishes
	mtGameLoop.join();
}

void OnSite::ServerGameEngine::startServer() {
	mslmLua->loadCode();
	msrmRenderer->loadTexture();
	//Allow GameLoop
	mfRunGame = true;
	//Start Thread
	mtGameLoop = std::thread(&ServerGameEngine::gameLoopThread, this);
}

void OnSite::ServerGameEngine::processNetworkInput(std::shared_ptr<OnSite::MessagePacket> pmPacket) {
	//Figure out Type of Packet
	switch(pmPacket->getPPacket()->type()) {
		//Lua Update
		case OnSite::PPT_UPDATE:
			//Pass to Lua Manager
			mslmLua->processIncomingMessagePacket(pmPacket);
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

void OnSite::ServerGameEngine::gameLoopThread() {
	std::shared_ptr<OnSite::MessagePacket> pmPacket;
	SDL_Event sdlEvent;
	//Wait for Start
	while (mfRunGame) {
		//Save Frame Start Time Point
		mtpFrameStartTime = std::chrono::high_resolution_clock::now();
		//Receive Network Input
		std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIncoming= msnmNetwork->getOutputQueue();
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
		mslmLua->runGameLoop();
		//Process Callbacks
		//Process Game Engine Updates
		//Send Outgoing Network Changes
		mslmLua->sendOutgoingMessagePackets(msnmNetwork->getOutputQueue());
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
		msrmRenderer->updateWindow();	
	}
}

void OnSite::ServerGameEngine::render() {
	msrmRenderer->render(mslmLua->getTextureMaps());
}

bool OnSite::ServerGameEngine::checkFinished() {
	return !mfRunGame;
}
