/**
 *	ServerInputManager Class Definition
 *	Terrell Ibanez
**/

//ServerInputManager Class Declaration
#include "ServerInputManager.hpp"

//C++ Standard Libraries
#include <memory>
#include <mutex>
#include <unordered_map>

//DEBUG
#include <iostream>

//OnSite Libraries
#include "InputBuffer.hpp"

//Constructor
OnSite::ServerInputManager::ServerInputManager(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIn) {
	//Store Reference to Incoming Message Packet Queue
	pcqIncoming = pcqIn;

	//Make InputBuffer HashMap
	umInputBuffers = std::make_shared<std::unordered_map<int, std::shared_ptr<OnSite::InputBuffer>>>();
		
}

//Deconstructor
OnSite::ServerInputManager::~ServerInputManager() {
	//Clear HashMap
	umInputBuffers->clear();
	//Remove Reference
	umInputBuffers = nullptr;
}

//Add Client for Input Tracking
void OnSite::ServerInputManager::addClient(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Make Connection
	std::shared_ptr<OnSite::InputBuffer> pibIn = std::make_shared<OnSite::InputBuffer>(fdSocket, pcqIncoming);
	//Insert Connection into HashMap
	umInputBuffers->operator[](fdSocket) = pibIn;
}

//Remove Client from Input Tracking
void OnSite::ServerInputManager::removeClient(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Remove Client from HashMap
	umInputBuffers->erase(fdSocket);
}

//Add Bytes to Client Input Buffer
void OnSite::ServerInputManager::addBytes(int fdSocket, char* bArray, size_t nBytes) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Check if Input Buffer Exists
	if (umInputBuffers->operator[](fdSocket) != nullptr) {
		//DEBUG
		std::cout << "BUFFER FOUND " << std::endl;
		//Add Bytes to Connection Buffer
		umInputBuffers->operator[](fdSocket)->addBytes(bArray, nBytes);
	}
}
