/**
 *	NetworkData Class Declaration
 *	Terrell Ibanez
**/

#ifndef GUARD_NETWORKDATA_H
#define GUARD_NETWORKDATA_H

//C++ Standard Libraries
#include <memory>
#include <string>
#include <vector>

namespace  OnSite {
	class NetworkData {
		public:
			//Constructor
			NetworkData(size_t maxSize);
			//Destructor
			~NetworkData();

			//Add Byte to Buffer
			void addByte(char b);
			//Read Byte from Buffer
			char readByte();
			//Check if Bytes are Available to Read
			bool bytesAvailableToRead();
			//Check if Space is Avaialable to Write
			bool bytesAvailableToWrite();

			//Rewind for Bytes not Written
			void rewind(size_t nBytes);

			//Converts Buffer Data to std::string
			//NOTE: Only Returns Written Bytes
			//NOTE: Does NOT Mark Bytes as Read
			std::string getString();
		private:
			//Number of Bytes Stored in Buffer
			size_t nBytesStored;
			//Number of Bytes Read
			size_t nBytesRead;
			//Max Size of Buffer
			size_t nMaxBytes;
			//Byte Buffer
			char* baData;
	};
}
#endif
