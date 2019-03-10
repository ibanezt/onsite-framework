/**
 *	ClientLuaManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_CLIENTLUAMANAGER_H
#define GUARD_CLIENTLUAMANAGER_H

//Lua - C Libraries
#if defined(__ANDROID__)
extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}
#else
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}
#endif

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
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"

namespace OnSite {
	class ClientLuaManager {
		public:
			//Constructor
			ClientLuaManager();
			//Deconstructor
			~ClientLuaManager();
			//Android lua code loader.
			int loadLuaDroid(lua_State *L, const char * filename);
			//Load Game Code
			void loadCode();
			//Run Lua Game Loop
			void runGameLoop();
			//Process IncomingMessagePacket
			void processIncomingMessagePacket(std::shared_ptr<OnSite::MessagePacket> pmPacket);
			//Add Outgoing MessagePackets to OutgoingQueue
			void sendOutgoingMessagePackets(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqOutgoing);
			//Get Texture Maps
			std::tuple<std::tuple<double, double, double, double, double, double, double>*, size_t> getTextureMaps();
		private:
			//Lua State
			lua_State *mlState;
			//Outgoing Network Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> mpqOutgoing;
	};
}
#endif
