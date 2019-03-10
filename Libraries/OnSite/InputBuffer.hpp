/**
 *	InputBuffer Class Declaration
 *	Terrell Ibanez
**/

#ifndef GUARD_INPUTBUFFER_H
#define GUARD_INPUTBUFFER_H

//C++ Standard Libraries
#include <memory>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "NetworkData.hpp"

namespace OnSite{
	class InputBuffer {
		public:
			//Constructor
			//Needs Socket File Descriptor, and Reference Pointer to Incoming Message Queue
			InputBuffer(int fdSocket, std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIn);
			//Deconstructor
			~InputBuffer();

			//Add Bytes to Network Data Buffer
			void addBytes(char *bArray, size_t length);
		private:
			//Flag: Are we in receiving mode?
			bool fReceiving;

			//Socket File Descriptor
			int fdConnectionSocket;
			//Network Data Buffer
			std::shared_ptr<OnSite::NetworkData> pndBuffer;

			//Byte Buffer for Size of Network Data Buffer
			char* bSizeBuffer;
			//Number of Bytes in Byte Buffer
			int lAmountBufferFilled;

			//Reference Pointer to Incoming Message Queue
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIncoming;
	};
}
#endif
