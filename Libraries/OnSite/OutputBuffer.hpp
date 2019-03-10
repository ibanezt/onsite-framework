/**
 *	OutputBuffer Class Declaration
 *	Terrell Ibanez
**/

#ifndef GUARD_OUTPUTBUFFER_H
#define GUARD_OUTPUTBUFFER_H

//C++ Libraries
#include <memory>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "NetworkData.hpp"

namespace OnSite{
	class OutputBuffer {
		public:
			//Constructor
			//Needs Socket File Descriptor
			OutputBuffer(int fdSocket);
			//Deconstructor
			~OutputBuffer();

			//Check if Bytes are Available in Buffer
			bool bytesAvailable();

			//Get Bytes for Network Data Buffer
			//NOTE: Caller is responsible for freeing array after use
			std::tuple<char*, size_t> getBytes();

			//Rewind Buffer for Bytes not Written
			void rewind(size_t nBytes);

			//Load Next Message Packet Into Buffer
			bool loadNext();
	
			//Push Message Onto Local Outgoing Message Queue		
			void push(std::shared_ptr<OnSite::MessagePacket> pMP);

			//Get Socket File Descriptor
			int getSocket();

			//Set Monitoring Flag
			void setMonitoring(bool flag);

			//Get Monitoring Flag
			bool getMonitoring();
		private:
			//Monitoring Flag (Signifies if Socket is Currently in Epoll)
			bool fMonitoring;

			//Socket File Descriptor
			int fdConnectionSocket;
			//Network Data Buffer
			std::shared_ptr<OnSite::NetworkData> pndBuffer;

			//Byte Buffer for Size of Network Data Buffer
			char* bSizeBuffer;
			//Number of Bytes in Byte Buffer
			int lAmountSizeBufferLeft;

			//Local Outgoing Message Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqLocalOutgoing;

			//Convert Length to Bytes
			void loadSizeBuffer(size_t length);
	};
}
#endif
