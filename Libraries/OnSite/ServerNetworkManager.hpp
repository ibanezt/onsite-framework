/**
 *	ServerNetworkManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_SERVERNETWORKMANAGER_H
#define GUARD_SERVERNETWORKMANAGER_H
//C++ Standard Libraries
#include <thread>

//System Libraries
#include <sys/epoll.h>

//OnSite Libraries
#include "ServerInputManager.hpp"
#include "ServerOutputManager.hpp"
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"

namespace OnSite {
	class ServerNetworkManager {
		public:
			//Constructor
			ServerNetworkManager();
			//Destructor
			~ServerNetworkManager();
			
			//Start Network Thread
			int startNetwork();
	
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> getInputQueue();
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> getOutputQueue();

		private:
			//File Descriptor for Listening Socket
			int fdListenSocket;
			//File Descriptor for Epoll Instance (Input)
			int fdEpollInput;
			//File Descriptor for Epoll Instance (Output)
			int fdEpollOutput;
			//Max Number of Epoll Events to Process at Once
			int MAXEVENTS;
			
			//Thread for Network Input
			std::thread tNetworkInput;
			std::thread tNetworkOutput;

			//Flag to Enable / Disable Network
			bool fProcessNetwork;

			//Event for Event Monitoring Registration
			//Note: Pre-Allocated for Efficency
			struct epoll_event eInputEvent;
			struct epoll_event eOutputEvent;

			//Buffer Array of Input Events
			struct epoll_event *aeInputEvents;
			//Buffer Array of Output Events
			struct epoll_event *aeOutputEvents;

			//Create and Bind Socket to Address
			int createBindSocket();
			//Set Socket Mode to NonBlocking
			int setSocketNonBlocking(int fdSocket);
			//Listen on Socket
			int listenSocket(int fdSocket);
			//Create Epoll Instance
			int createEpoll();
			//Register Socket for Epoll Monitoring
			int registerSocket(int fdSocket, int fdEpoll, epoll_event* eEvent, int fMonitorEvent);
			//Close Socket
			void closeSocket(int fdSocket);
			//Input Thread Code
			void networkInputThread();
			//Output Thread Code
			void networkOutputThread();

			//Incoming Queue (Client->Server)
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIncoming;
			//Outgoing Queue (Server->Client)
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqOutgoing;
			//Client Input Manager
			std::shared_ptr<OnSite::ServerInputManager> psimInputManager;
			//Client Output Manager
			std::shared_ptr<OnSite::ServerOutputManager> psomOutputManager;
	};
}
#endif
