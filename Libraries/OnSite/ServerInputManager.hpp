#ifndef GUARD_SERVERINPUTMANAGER_H
#define GUARD_SERVERINPUTMANAGER_H
/** 	
 *	ServerInputManager Class Declaration
 * 	Terrell Ibanez
**/

//NOTE: Uses Locks for Thread Safety (BLOCKS while Waiting to Acquire Lock)

//C++ Standard Libraries
#include <memory>
#include <mutex>
#include <unordered_map>

//OnSite Libraries
#include "InputBuffer.hpp"

namespace OnSite {
	class ServerInputManager {
		public:
			//Constructor
			ServerInputManager(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIn);
			//Deconstructor
			~ServerInputManager();

			//Add Client for Input Tracking (LOCKING)
			void addClient(int fdSocket);
			//Remove Client from Input Tracking (LOCKING)
			void removeClient(int fdSocket);
			
			//Add Bytes to Client Input Buffer (LOCKING)
			void addBytes(int fdSocket, char* bArray, size_t nBytes);

		private:
			//Reference to Incoming Message Packet Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIncoming;
			//Client Input Buffer Hash Map
			std::shared_ptr<std::unordered_map<int, std::shared_ptr<OnSite::InputBuffer>>> umInputBuffers;
			
			//Lock for Thread Safety
			std::mutex mLock;
	};
}
#endif
