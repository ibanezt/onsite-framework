/**
 *	MessagePacket Class Definition
 *	Terrell Ibanez
**/

//MessagePacket Class Declaration
#include "MessagePacket.hpp"

//C++ Standard Libraries
#include <memory>
#include <string>

//Protobuf Libraries
#include "PPacket.pb.h"

//Constructor (Outgoing)
OnSite::MessagePacket::MessagePacket(int fdSocket) {
	fdAssocSocket = fdSocket;
	//Make PPacket
	ppPacket = std::make_shared<OnSite::PPacket>();
}

//Constructor
OnSite::MessagePacket::MessagePacket(int fdSocket, std::shared_ptr<OnSite::PPacket> packet) {
	fdAssocSocket = fdSocket;
	ppPacket = packet;
}

//Constructor, Parse PPAcket from String
OnSite::MessagePacket::MessagePacket(int fdSocket, std::string sData) {
	fdAssocSocket = fdSocket;
	//Make PPacket
	ppPacket = std::make_shared<OnSite::PPacket>();
	//Parse PPacket from String
	ppPacket->ParseFromString(sData);
}

//Destructor
OnSite::MessagePacket::~MessagePacket() {
	ppPacket = nullptr;
}

//Serialize PPacket for Network Transmission
std::string OnSite::MessagePacket::serialize() {
	std::string sData;
	ppPacket->SerializeToString(&sData);
	return sData;
}

//Get Associated Socket
int OnSite::MessagePacket::getSocket() {
	return fdAssocSocket;
}

//Get Associated PPacket
std::shared_ptr<OnSite::PPacket> OnSite::MessagePacket::getPPacket() {
	return ppPacket;
}
