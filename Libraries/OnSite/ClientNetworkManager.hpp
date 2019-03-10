/**
 *	ClientNetworkManager Class Declaration
 *	Terrell Ibanez
**/
#ifndef GUARD_CLIENTNETWORKMANAGER_H
#define GUARD_CLIENTNETWORKMANAGER_H
//C++ Standard Libraries
#include <thread>

//System Libraries
#include <sys/epoll.h>

//OnSite Libraries
#include "ClientInputManager.hpp"
#include "ClientOutputManager.hpp"
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"

namespace OnSite {
	class ClientNetworkManager {
		public:
			//Constructor
			ClientNetworkManager();
			//Destructor
			~ClientNetworkManager();
			
			//Start Network Thread
			int startNetwork();
	
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> getInputQueue();
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> getOutputQueue();

		private:
			//File Descriptor for Connection Socket
			int fdConnectSocket;
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

			//Create and Connect on Socket (BLOCKING)
			int createConnectSocket();
			//Set Socket Mode to NonBlocking
			int setSocketNonBlocking(int fdSocket);

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

			//Incoming Queue (Server->Client)
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqIncoming;
			//Outgoing Queue (Client->Server)
			std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pqOutgoing;
			//Client Input Manager
			std::shared_ptr<OnSite::ClientInputManager> pcimInputManager;
			//Client Output Manager
			std::shared_ptr<OnSite::ClientOutputManager> pcomOutputManager;
	};
}
#endif
