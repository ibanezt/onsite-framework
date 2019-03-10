#ifndef GUARD_CLIENTINPUTMANAGER_H
#define GUARD_CLIENTINPUTMANAGER_H
/** 	
 *	ClientInputManager Class Declaration
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
	class ClientInputManager {
		public:
			//Constructor
			ClientInputManager(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIn);
			//Deconstructor
			~ClientInputManager();

			//Set Socket for Input Tracking (LOCKING)
			void setSocket(int fdSocket);
			//Remove Socket from Input Tracking (LOCKING)
			void removeServerConnection(int fdSocket);
			
			//Add Bytes to Client Input Buffer (LOCKING)
			void addBytes(int fdSocket, char* bArray, size_t nBytes);

		private:
			//Reference to Incoming Message Packet Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIncoming;
			//Client Input Buffer
			std::shared_ptr<OnSite::InputBufer> bInputBuffer;
			
			//Lock for Thread Safety
			std::mutex mLock;
	};
}
#endif
