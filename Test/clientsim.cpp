
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include <memory>

#include "./../Libraries/OnSite/MessagePacket.hpp"
#include "./../Libraries/OnSite/PPacket.pb.h"
int createConnectSocket() {
    int fdSocket;
    char cstrPort[] = "80";

    int rcGAI; //GetAddressInfo return code

    struct addrinfo aiHints; //Parameters
    struct addrinfo *llaiResult; //Result Linked List (Pointer)
    struct addrinfo *paiNode; //Pointer to Current Node in Result Linked List 

    //Zero out Parameters
    memset(&aiHints, 0, sizeof(struct addrinfo));

    aiHints.ai_family = AF_INET; //IPv4
    aiHints.ai_socktype = SOCK_STREAM; //TCP Socket
    aiHints.ai_flags = 0; //No Flags
    aiHints.ai_protocol = 0; //Auto-Select Protocol
    aiHints.ai_canonname = nullptr;
    aiHints.ai_addr = nullptr;
    aiHints.ai_next = nullptr;

    rcGAI = getaddrinfo("localhost", cstrPort, &aiHints, &llaiResult);

    if (rcGAI != 0) {
        //Unable to get addrinfo

        return -1;
    }

    for (paiNode = llaiResult; paiNode != nullptr; paiNode = paiNode->ai_next) {
        //Create Socket
        fdSocket = socket(paiNode->ai_family, paiNode->ai_socktype, paiNode->ai_protocol);

        if (fdSocket == -1) {
            //Try Next Socket
            continue;
        }

        //Attempt to Connect to Socket
        rcGAI = connect(fdSocket, paiNode->ai_addr, paiNode->ai_addrlen);

        if (rcGAI == 0) {
            //Bind Successful
            break;
        }
        else {
            close(fdSocket);
        }
    }

    freeaddrinfo(llaiResult);

    if (paiNode == nullptr) {
        //Unable to Connect
        return -1;
    }

    return fdSocket;

}
                                  

int main() {
	int fdSocket = createConnectSocket();
	if (fdSocket == -1) {
		std::cout << "Error Creating Socket" << std::endl;
		return -1;
	}
	std::shared_ptr<OnSite::MessagePacket> pMP;

	std::shared_ptr<OnSite::PPacket> pPacket;
	pPacket = std::make_shared<OnSite::PPacket>();

	pPacket->set_type(OnSite::CONTROL);
	OnSite::PControl* pControl;

	pControl = new OnSite::PControl();
	pControl->set_type(OnSite::TEST);
	
	pPacket->set_allocated_cdata(pControl);
	
	pMP = std::make_shared<OnSite::MessagePacket>(fdSocket, pPacket);
	std::string sData;
	sData = pMP->serialize();
	std::cout << sData.length() << std::endl;

	//Set length
	char* bSizeBuffer = (char*) malloc(sizeof(char) * 8);

	for (size_t i = 0; i < 8; ++i) {
                bSizeBuffer[i] = (sData.length() >> (i * 8));
    }

		
	ssize_t ret = write(fdSocket, bSizeBuffer, 8);

	std::cout << ret << std::endl;
	
	ret = write(fdSocket, sData.c_str(), sData.length());
	std::cout << ret << std::endl;

	return 0;
}
