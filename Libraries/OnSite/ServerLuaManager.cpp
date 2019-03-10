/**
 *	ServerLuaManager Class Definition
 *	Terrell Ibanez
**/

//ServerLuaManager Class Declaration
#include "ServerLuaManager.hpp"

//Lua (C) Libraries
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

//SDL Libraries
#include <SDL2/SDL.h>

//C++ Libraries
#include <iostream>

//OnSite Libraries
#include "ConcurrentQueue.hpp"

//Protubuf Libraries
#include "PPacket.pb.h"

//Constructor
OnSite::ServerLuaManager::ServerLuaManager() {
	//Create Lua State
	mlState = luaL_newstate();
	
	//Open Standard Lua Libraries
	luaL_openlibs(mlState);

	//Load API
	int rCode = luaL_loadfile(mlState, "ServerAPI.lua");

	if (rCode != 0) {
		std::cout << "Lua ServerAPI Load Error: " << lua_tostring(mlState, -1) << std::endl;
	}

	rCode = lua_pcall(mlState, 0, 0, 0);

	if (rCode != 0) {
		std::cout << "Lua ServerAPI Execution Error: " << lua_tostring(mlState, -1) << std::endl;
	}
}

//Deconstructor
OnSite::ServerLuaManager::~ServerLuaManager() {
	//Close Lua State
	lua_close(mlState);
}

//Load Game Code into Lua
void OnSite::ServerLuaManager::loadCode() {
	int rCode = luaL_loadfile(mlState, "ServerCode.lua");
	if (rCode != 0) {
		std::cout << "Lua ServerCode Loader Error: " << lua_tostring(mlState, -1) << std::endl;
	}
	
	rCode = lua_pcall(mlState, 0, 0, 0);
	if (rCode != 0) {
		std::cout << "Lua ServerCode Execution Error: " << lua_tostring(mlState, -1) << std::endl;
	}
}

void OnSite::ServerLuaManager::runGameLoop() {
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "GameLoop");
	lua_remove(mlState, -2);

	int rCode = lua_pcall(mlState, 0, 0, 0);

	if (rCode != 0) {
		std::cout << "Lua Game Loop Error: " << lua_tostring(mlState, -1) << std::endl;
	}
}

void OnSite::ServerLuaManager::processIncomingMessagePacket(std::shared_ptr<OnSite::MessagePacket> pmPacket) {
	//TODO:Fix Incoming - Differentiate Between Client and Server Variable
	//Push Sync Table to Top of Stack
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "Variables");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "Sync");
	lua_remove(mlState, -2);

	std::shared_ptr<OnSite::PPacket> pPPacket = pmPacket->getPPacket();
	
	//Push Name of Variable to Top of Stack
	lua_pushstring(mlState, pPPacket->udata().name().c_str());

	//Push Value of Variable to Top of Stack
	switch(pPPacket->udata().type()) {
		case OnSite::PUT_NUMBER:
			lua_pushnumber(mlState, pPPacket->udata().unumber().value());
		break;

		case OnSite::PUT_STRING:
			lua_pushstring(mlState, pPPacket->udata().ustring().value().c_str());
		break;

		case OnSite::PUT_BOOL:
			lua_pushboolean(mlState, pPPacket->udata().uboolean().value());
		break;
	}
	//Set Variable, Pops Name/Value
	lua_rawset(mlState, -3);
	//Finished, Pop Sync off Stack
	lua_pop(mlState, 1);
}

void OnSite::ServerLuaManager::sendOutgoingMessagePackets(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqOutgoing) {
	//Push Sync Table to Top of Stack
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "Variables");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "Sync");

	//Push NetworkSyncQueue Table to Top of Stack
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "_Internal");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "NetworkSyncQueue");
	lua_remove(mlState, -2);

	//Get Length of NetworkSyncQueue Table
	size_t length = lua_objlen(mlState, -1);

	std::shared_ptr<OnSite::MessagePacket> pmpPacket;
	std::shared_ptr<OnSite::PPacket> pPPacket;
	
	//For Each Value in NetworkSyncQueue Table
	for (size_t i = 1; i <= length; ++i) {
		//Push NetworkSyncQueue[i] to Top of Stack
		lua_rawgeti(mlState, -1, i);
		//Push Sync to Top of Stack
		lua_pushvalue(mlState, -3);
		//Push NetworkSyncQueue[i].NAME to Top of Stack
		lua_getfield(mlState, -2, "NAME");

		pPPacket = std::make_shared<OnSite::PPacket>();
		pPPacket->set_type(OnSite::PPT_UPDATE);
		OnSite::PUpdate *pUpdate= new OnSite::PUpdate();
		pUpdate->set_name(lua_tostring(mlState, -1));

		//Push Sync[NetworkSyncQueue[i].Name] to Top of Stack
		lua_rawget(mlState, -2);

		OnSite::PUpdateLuaString *pString;
		OnSite::PUpdateLuaNumber *pNumber;
		OnSite::PUpdateLuaBoolean *pBoolean;

		//Figure out Type, and Adjust Accordingly
		switch (lua_type(mlState, -1)) {
			case LUA_TSTRING:
				pString = new OnSite::PUpdateLuaString();
				pString->set_value(lua_tostring(mlState, -1));
				pUpdate->set_allocated_ustring(pString);
			break;

			case LUA_TNUMBER:
				pNumber = new OnSite::PUpdateLuaNumber();
				pNumber->set_value(lua_tonumber(mlState, -1));
				pUpdate->set_allocated_unumber(pNumber);
			break;

			case LUA_TBOOLEAN:
				pBoolean = new OnSite::PUpdateLuaBoolean();
				pBoolean->set_value(lua_toboolean(mlState, -1));
				pUpdate->set_allocated_uboolean(pBoolean);
			break;

			default:
				//TODO: ERROR
			break;
		}
		//Pop Sync/NetworkSyncQueue[i].NAME/Sync[NetworkSyncQueue[i].NAME] (NetworkSyncQueue[i] is now Top of Stack)
		lua_pop(mlState, 3);
		//Push NetworkSyncQueue[i].ID to Top of Stack
		lua_getfield(mlState, -1, "ID");
		//Add PUpdate to PPacket
		pPPacket->set_allocated_udata(pUpdate);
		//Add PPacket to MessagePacket
		pmpPacket = std::make_shared<OnSite::MessagePacket>((int)lua_tonumber(mlState, -1), pPPacket);
		//Pop NetworkSyncQueue[i] (NetworkSyncQueue is now Top of Stack)
		lua_pop(mlState, 1);
		//Push MessagePacket onto Outgoing Network Queue
		pqOutgoing->push(pmpPacket);
	}
	//Pop Sync/NetworkSyncQueue (Stack is now Empty)
	lua_pop(mlState, 2);
}

//Returns 2-Tuple Containing a 7-Tuple Array, as Well as Length of that Array
/*
std::tuple<std::tuple<double, double, double, double, double, double, double>*, size_t> getTextureMaps() {
	//Push GameObject Table to Top of Stack
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "GameObjects");
	
	//Get Length of GameObjects Table
	size_t tableLength = lua_objlen(mlState, -1);
	
	//7-Tuple Array
	// X,Y, U, V, Width, Height, Rotation
	std::tuple<double, double, double, double, double ,double>* mapArray;

	size_t arrLength = 0;
	for (size_t i = 1; i<=length; ++i) {
		//Push GameObjects[i] to Top of Stack
		lua_rawgeti(mlState, -1, i);
		
		//Push GameObjects[i].TextureMap to Top of Stack
		lua_getfield(mlState, -1, "TextureMap");

		//Check if TextureMap Exists
		if (!lua_isnil(mlState, -1)) {
			lua_getfield(mlState, -1, "X");
			lua_getfield(mlState, -2, "Y");
			lua_getfield(mlState, -3, "Width");
			lua_getfield(mlState, -4, "Height");
			
			lua_getnumber(mlState, -4), lua_getnumber(mlState, -3), lua_getnumber(mlState, -3), lua_getnumber(mlState, -1)};
			//Pop X, Y, Width, Height, (We're Done with that)
			lua_pop(mlState, 4);
		}
		
		//Pop GameObjects[i] / GameObjects[i].TextureMap
		lua_pop(mlState, 2);
		return nullptr;
	}
}
*/

std::tuple<std::tuple<double, double, double, double, double, double, double>*, size_t> OnSite::ServerLuaManager::getTextureMaps() {
	//Push OnSite.Server._Internal table to Top of Stack
	lua_getglobal(mlState, "OnSite");
	lua_getfield(mlState, -1, "Server");
	lua_remove(mlState, -2);
	lua_getfield(mlState, -1, "_Internal");
	lua_remove(mlState, -2);
	//Push OnSite.Server._Internal.prepareTextureMaps function to Top of Stack
	lua_getfield(mlState, -1, "prepareTextureMaps");
	//Call prepareTextureMaps()
	int rCode = lua_pcall(mlState, 0, 0,0);

	if (rCode != 0) {
		std::cout << "Lua prepareTexureMaps Error: " << lua_tostring(mlState, -1) << std::endl;
	}

	//Push TextureMaps table to Top of Stack
	lua_getfield(mlState, -1, "TextureMaps");

	size_t arrLength = lua_objlen(mlState, -1);

	std::tuple<double, double, double, double, double, double, double>* mapArray;
	
	mapArray = (std::tuple<double, double, double, double, double, double, double>*) malloc(sizeof(std::tuple<double, double, double, double, double, double, double>) * arrLength);
	for (size_t i = 1; i <= arrLength; ++i) {
		lua_rawgeti(mlState, -1, i);

		for (size_t j=1; j <= 7; ++j) {
			lua_rawgeti(mlState, -j, j);
		}
		
		mapArray[i-1] = std::make_tuple(lua_tonumber(mlState, -7), lua_tonumber(mlState, -6), lua_tonumber(mlState, -5), lua_tonumber(mlState, -4), lua_tonumber(mlState, -3), lua_tonumber(mlState, -2), lua_tonumber(mlState, -1));
		lua_pop(mlState, 8);
	}

	return std::make_tuple(mapArray, arrLength);
}
