/**
 *	OutputBuffer Class Definition
 *	Terrell Ibanez
**/

//Class Declaration
#include "OutputBuffer.hpp"

//C++ Standard Libraries
#include <memory>

//C Libraries
#include <cstdlib>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "NetworkData.hpp"

//Constructor
OnSite::OutputBuffer::OutputBuffer(int fdSocket) {
	//Save Socket File Descriptor
	fdConnectionSocket = fdSocket;
	//Allocate Byte Buffer
	bSizeBuffer = (char*) malloc(sizeof(char) * 8);
	//No Bytes in Byte Buffer Yet
	lAmountSizeBufferLeft = 0;
	pcqLocalOutgoing = std::make_shared<OnSite::ConcurrentQueue<OnSite::MessagePacket>>();
}

//Destructor
OnSite::OutputBuffer::~OutputBuffer() {
	//Free Byte Buffer
	free(bSizeBuffer);
}

//Check if Bytes Are Available in Buffer
bool OnSite::OutputBuffer::bytesAvailable() {
	return pndBuffer->bytesAvailableToRead();
}

//Get Bytes for Network Data Buffer
//TODO Make More Efficient
std::tuple<char*, size_t> OnSite::OutputBuffer::getBytes() {
	//Byte Array Buffer
	char *aByteBuffer = nullptr;
	//Number of Bytes
	size_t stBytes= 0;
	//Check if Bytes Are Available
	if (pndBuffer->bytesAvailableToRead()) {
		//Fill aByteBuffer
		aByteBuffer = (char*) malloc(sizeof(char) * 512);
		for (stBytes = 0; stBytes < 512; ++stBytes) {
			if (pndBuffer->bytesAvailableToRead()) {
				aByteBuffer[stBytes] = pndBuffer->readByte();
			}
		}
	}
	//Return Byte Array (nullptr if no bytes available)
	return std::make_tuple(aByteBuffer, stBytes);
}

//Rewind Buffer for Bytes Not Written
void OnSite::OutputBuffer::rewind(size_t nBytes) {
	pndBuffer->rewind(nBytes);
}

//Load Next MessagePacket into Buffer
//TODO Make More Efficient
bool OnSite::OutputBuffer::loadNext() {
	//Outgoing MessagePacket
	std::shared_ptr<MessagePacket> pMP;
	//String for Serialized MessagePacket
	std::string sData;

	//Attempt to Pop Next MessagePacket in Local Outgoing Queue
	pMP = pcqLocalOutgoing->pop();
	
	//Check if MessagePacket is Valid
	if (pMP != nullptr) {
		//Serialize MessagePacket for Data Transmission
		sData = pMP->serialize();
		//Compute Bytes for Size
		loadSizeBuffer(sData.length());
		//Allocate Buffer for Data with Room for Size
		pndBuffer = std::make_shared<OnSite::NetworkData>(sData.length() + 8);
		//Add Size Bytes
		for (size_t i = 0; i < 8; ++i) {
			pndBuffer->addByte(bSizeBuffer[i]);
		}
		//Add Data Bytes
		for (size_t i = 0; i < sData.length(); ++i) {
			pndBuffer->addByte(sData[i]);
		}
		//Return True if MessagePacket Loaded
		return true;
	}
	//Return False if No MessagePackets are Available
	return false;
}

//Push Message Onto Local Outgoing Message Queue
void OnSite::OutputBuffer::push(std::shared_ptr<MessagePacket> pMP) {
	pcqLocalOutgoing->push(pMP);
}

//Get Socket File Descriptor
int OnSite::OutputBuffer::getSocket() {
	return fdConnectionSocket;
}

//Set Monitoring Flag
void OnSite::OutputBuffer::setMonitoring(bool flag) {
	fMonitoring = flag;
}
//Get Monitoring Flag
bool OnSite::OutputBuffer::getMonitoring() {
	return fMonitoring;
}
//Convert Data Length to Bytes and Load to Buffer
void OnSite::OutputBuffer::loadSizeBuffer(size_t length) {
	for (size_t i = 0; i < 8; ++i) {
		bSizeBuffer[i] = (length >> (i * 8));
	}
}
