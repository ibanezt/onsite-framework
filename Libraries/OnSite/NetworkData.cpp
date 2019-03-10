/**
 *	Network Data Class Definition
 *	Terrell Ibanez
**/

//Class Declaration
#include "NetworkData.hpp"

//C Standard Libraries
#include <cstdlib>

//Constructor
OnSite::NetworkData::NetworkData(size_t maxSize) {
	//0 Bytes Stored
	nBytesStored = 0;
	//0 Bytes Read
	nBytesRead = 0;
	//Allocate Byte Array
	baData = (char *) malloc(sizeof(char) * maxSize);
	//Keep Track of Max Size
	nMaxBytes = maxSize;
}

//Desctructor
OnSite::NetworkData::~NetworkData() {
	//Free Byte Array
	free(baData);
}

//Add Byte to Buffer
//Inefficient, but it's "OOP"
void OnSite::NetworkData::addByte(char b) {
	//Make Sure We Don't Overflow Buffer
	if (nBytesStored < nMaxBytes) {
		//Add Byte to Buffer
		baData[nBytesStored] = b;
		++nBytesStored;
	}
}

//Read Byte from Buffer
//Inefficient, but it's "OOP"
char OnSite::NetworkData::readByte() {
	char b = '\n';
	//Make Sure We Don't Overrrun Buffer
	if (nBytesRead < nBytesStored) {
		b = baData[nBytesRead];
		++nBytesRead;		
	}
	return b;
}

//Check if Bytes are Available in Buffer to Read
bool OnSite::NetworkData::bytesAvailableToRead() {
	return (nBytesRead < nBytesStored);
}

//Check if Space is Available to Write Bytes
bool OnSite::NetworkData::bytesAvailableToWrite() {
	return (nBytesStored < nMaxBytes);
}

//Rewind Buffer for Bytes Not Written
void OnSite::NetworkData::rewind(size_t nBytes) {
	if (nBytesRead - nBytes > 0) {
		nBytesRead -= nBytes;
	}
}
//Converts to std::string
//Note: Only Returns Written Bytes, So Make Sure Writing is Done Before Calling
//Note: Does NOT Mark Bytes as Read
std::string OnSite::NetworkData::getString() {
	std::string sData(baData, nBytesStored);
	return sData;
}

