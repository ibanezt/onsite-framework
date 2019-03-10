/**
 *	ServerOutputManager Class Definition
 *	Terrell Ibanez
**/

//ServerOutputManager Class Declaration
#include "ServerOutputManager.hpp"

//C++ Standard Libraries
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>

//OnSite Libraries
#include "OutputBuffer.hpp"

//Constructor
OnSite::ServerOutputManager::ServerOutputManager(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqOut) {
	//Pointer to Outgoing Queue
	pcqOutgoing = pcqOut;
	//Make InputBuffer HashMap
	umOutputBuffers = std::make_shared<std::unordered_map<int, std::shared_ptr<OnSite::OutputBuffer>>>();
	
}

//Deconstructor
OnSite::ServerOutputManager::~ServerOutputManager() {
	//Clear HashMap
	umOutputBuffers->clear();
	//Remove Reference
	umOutputBuffers = nullptr;
}

//Add Client for Output Tracking (LOCKING)
void OnSite::ServerOutputManager::addClient(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Make Output Buffer
	std::shared_ptr<OnSite::OutputBuffer> pobOut = std::make_shared<OnSite::OutputBuffer>(fdSocket);
	//Insert Output Buffer into HashMap
	umOutputBuffers->operator[](fdSocket) = pobOut;
}

//Remove Client from Output Tracking (LOCKING)
void OnSite::ServerOutputManager::removeClient(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Remove Client from HashMap
	umOutputBuffers->erase(fdSocket);
}

//Get Bytes from Client Output Buffer to Send (LOCKING)
std::tuple<char*, size_t> OnSite::ServerOutputManager::getBytes(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//Check if Output Buffer Exists
	if (umOutputBuffers->find(fdSocket) != umOutputBuffers->end()) {
		return umOutputBuffers->operator[](fdSocket)->getBytes();
	}
	return std::make_tuple(nullptr, 0);
}

//Rewind Buffer for Bytes Not Written (LOCKING)
void OnSite::ServerOutputManager::rewind(int fdSocket, size_t nBytes) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);

	//Check if Socket is Valid
	if (umOutputBuffers->find(fdSocket) != umOutputBuffers->end()) {
		//Rewind Buffer
		umOutputBuffers->operator[](fdSocket)->rewind(nBytes);
	}
}

//Process Outgoing Queue (LOCKING)
void OnSite::ServerOutputManager::processQueue() {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);
	//MessagePacket from Queue
	std::shared_ptr<OnSite::MessagePacket> pmpPacket;
	
	//Attempt to Pop Next MessagePacket on Outgoing Queue
	pmpPacket = pcqOutgoing->pop();

	//Check if MessagePacket is Valid
	while (pmpPacket != nullptr) {
		//Get Associated Socket File Descriptor
		int fdSocket = pmpPacket->getSocket();
		//Check if Broadcast MessagePacket
		if (fdSocket == -1) {
			//Send MessagePacket to All Clients
			//For Each OutputBuffer
			for (std::unordered_map<int, std::shared_ptr<OnSite::OutputBuffer>>::iterator it = umOutputBuffers->begin(); it != umOutputBuffers->end(); ++it) {
				//Check if OutputBuffer is Valid
				if (it->second != nullptr) {
					//Push MessagePacket onto Buffer's Local Output Queue
					it->second->push(pmpPacket);
				}
			}
		}
		else {
			//Not a Broadcast Message Packet
			//Check if Socket is Valid
			if (umOutputBuffers->find(fdSocket) != umOutputBuffers->end()) {
				//Push MessagePacket onto OutputBuffer's Local Output Queue
				umOutputBuffers->operator[](fdSocket)->push(pmpPacket);
			}
		}
	}
}

//Get list of Socket File Descriptors to Monitor with Epoll (LOCKING)
std::tuple<int*, size_t> OnSite::ServerOutputManager::getSockets() {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);

	//Array of Socket File Descriptors
	int *afdSockets = nullptr;
	afdSockets = (int*) malloc(sizeof(int) * umOutputBuffers->size());
	//Number of File Socket Descriptors in Array
	size_t stSocketArrayLength = 0;
	//For Each OutputBuffer
	for (std::unordered_map<int, std::shared_ptr<OnSite::OutputBuffer>>::iterator it; it != umOutputBuffers->end(); ++it) {
		//Check if Socket File Descriptor is Already Being Monitored
		if (!it->second->getMonitoring()) {
			//Attempt to Load Next MessagePacket
			if (it->second->loadNext()) {
				//If Successful, Add Socket to Array
				afdSockets[stSocketArrayLength] = it->second->getSocket();
				++stSocketArrayLength;
			}
		}
	}
	return std::make_tuple(afdSockets, stSocketArrayLength);
}

//Set Socket Monitoring Flag (LOCKING)
void OnSite::ServerOutputManager::setMonitoring(int fdSocket, bool flag) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);

	//Check if Socket is Valid
	if (umOutputBuffers->find(fdSocket) != umOutputBuffers->end()) {
		//Change Monitoring Flag
		umOutputBuffers->operator[](fdSocket)->setMonitoring(flag);
	}
}

//Get Socket Monitoring Flag (LOCKING)
bool OnSite::ServerOutputManager::getMonitoring(int fdSocket) {
	//Acquire Lock for Thread Safety
	std::lock_guard<std::mutex> lock(mLock);

	//Check if Socket is Valid
	if (umOutputBuffers->find(fdSocket) != umOutputBuffers->end()) {
		//Return Monitoring Flag
		return umOutputBuffers->operator[](fdSocket)->getMonitoring();
	}
}
