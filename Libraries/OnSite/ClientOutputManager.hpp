#ifndef GUARD_CLIENTOUTPUTMANAGER_H
#define GUARD_CLIENTOUTPUTMANAGER_H
/** 	
 *	ClientOutputManager Class Declaration
 * 	Terrell Ibanez
**/

//NOTE: Uses Locks for Thread Safety (BLOCKS while Waiting to Acquire Lock)

//C++ Standard Libraries
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>

//OnSite Libraries
#include "OutputBuffer.hpp"

namespace OnSite {
	class ClientOutputManager {
		public:
			//Constructor
			ClientOutputManager(std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqOut);
			//Deconstructor
			~ClientOutputManager();

			//Add Client for Output Tracking (LOCKING)
			void addClient(int fdSocket);
			//Remove Client from Output Tracking (LOCKING)
			void removeClient(int fdSocket);
			
			//Get Bytes from Client Output Buffer to Send (LOCKING)
			//NOTE: Caller is responsible for freeing byte array
			std::tuple<char*, size_t> getBytes(int fdSocket);

			//Rewind for Bytes Not Written (LOCKING)
			void rewind(int fdSocket, size_t nBytes);

			//Process Outgoing Queue, add MessagePackets to be Sent to Individual Output Buffers (LOCKING)
			void processQueue(); 

			//Get List of Socket File Descriptors to Monitor with Epoll (LOCKING)
			std::tuple<int*, size_t> getSockets();

			//Set Socket Monitoring Flag (LOCKING)
			void setMonitoring(int fdSocket, bool flag);
			
			//Get Socket Monitoring Flag (LOCKING)
			bool getMonitoring(int fdSocket);

		private:
			//Pointer to Outgoing Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqOutgoing;

			//Client Output Buffer Hash Map
			std::shared_ptr<std::unordered_map<int, std::shared_ptr<OnSite::OutputBuffer>>> umOutputBuffers;
			
			//Lock for Thread Safety
			std::mutex mLock;
	};
}
#endif
