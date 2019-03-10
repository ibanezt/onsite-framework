/**
 *		InputBuffer Class Definition
 *		Terrell Ibanez
**/

//Class Declaration
#include "InputBuffer.hpp"

//C++ Standard Libraries
#include <memory>

//C Standard Libraries
#include <cstdlib>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "NetworkData.hpp"

//Constructor
OnSite::InputBuffer::InputBuffer(int fdSocket, std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pcqIn) {
	//Save Socket File Descriptor
	fdConnectionSocket = fdSocket;
	//Set Receiving Flag to Off
	fReceiving = false;
	//Allocate Byte Buffer
	bSizeBuffer = (char*) malloc(sizeof(char) * 8);
	//No Bytes in Byte Buffer Yet
	lAmountBufferFilled = 0;
	//Store Reference to Incoming Message Queue
	pcqIncoming = pcqIn;
}

//Destructor
OnSite::InputBuffer::~InputBuffer() {
	//Free Byte Buffer
	free(bSizeBuffer);
}

//Add Bytes to Buffer
void OnSite::InputBuffer::addBytes(char *bArray, size_t length) {
	//For Each Byte
	for (size_t i = 0; i < length; ++i) {
		//Have we figured out how long it's going to be?
		if (fReceiving == true) {
			//Add Byte to Buffer
			pndBuffer->addByte(bArray[i]);
			//If Buffer is Full, We're Done
			if (!pndBuffer->bytesAvailableToWrite()) {

				//Create Message Packet from Object
				std::shared_ptr<OnSite::MessagePacket> pMP;
				pMP = std::make_shared<OnSite::MessagePacket>(fdConnectionSocket, pndBuffer->getString());
				//Add to Incoming Message Queue
				pcqIncoming->push(pMP);
				//Switch Back out of Receiving Mode
				fReceiving = false;
			}
		}
		//Figure Out How Long it is
		else {
			//If we have all 8 Bytes, convert it to size_t
			if (lAmountBufferFilled == 8) {
				size_t sSize = 0;
				lAmountBufferFilled = 0;
				for (int j = 0; j < 8; ++j) {
					//8 Bit Left Shift to Insert Bytes
					sSize += (bSizeBuffer[j] << (j * 8));
				}
				//Make Buffer
				pndBuffer = std::make_shared<OnSite::NetworkData>(sSize);

				//Switch to Receiving Mode
				fReceiving = true;

				//Make Sure to Reprocess Current Byte in Other Mode
				--i;
			}
			else {
				//Add Byte to Buffer
				bSizeBuffer[lAmountBufferFilled] = bArray[i];
				//Increment Counter for Number of Bytes
				++lAmountBufferFilled;
			}
		}
	}
}
