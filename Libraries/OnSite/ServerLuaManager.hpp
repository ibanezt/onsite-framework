/**
 *	ServerLuaManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_SERVERLUAMANAGER_H
#define GUARD_SERVERLUAMANAGER_H

//Lua - C Libraries
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

//SDL Libraries
#include <SDL2/SDL.h>

//C++ Libraries
#include <memory>
#include <tuple>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"

namespace OnSite {
	class ServerLuaManager {
		public:
			//Constructor
			ServerLuaManager();
			//Deconstructor
			~ServerLuaManager();
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
