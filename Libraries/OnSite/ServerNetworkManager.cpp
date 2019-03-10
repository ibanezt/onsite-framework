/**
 *	ServerNetworkManager Class Definition
 *	Terrell Ibanez
**/

//ServerNetworkManager Class Declaration
#include "ServerNetworkManager.hpp"

//C++ Standard Libraries
#include <memory>
#include <iostream>
#include <thread>

//System Libraries
//#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

//C Standard Libraries
#include <cstring>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "ServerInputManager.hpp"
#include "ServerOutputManager.hpp"

OnSite::ServerNetworkManager::ServerNetworkManager() {
	
	fdListenSocket = -1;
	MAXEVENTS = 64;
	//Note: Threads will not do anything until assigned
	tNetworkInput = std::thread();
	tNetworkOutput = std::thread();

	//Don't Process Anything Yet
	fProcessNetwork = false;
	
	//Create Input Queue
	pqIncoming = std::make_shared<OnSite::ConcurrentQueue<OnSite::MessagePacket>>();

	//Create Output Queue
	pqOutgoing = std::make_shared<OnSite::ConcurrentQueue<OnSite::MessagePacket>>();
	
	//Create Server Input Manager
	psimInputManager = std::make_shared<OnSite::ServerInputManager>(pqIncoming);
	
	//Create Server Output Manager
	psomOutputManager = std::make_shared<OnSite::ServerOutputManager>(pqOutgoing);
}

OnSite::ServerNetworkManager::~ServerNetworkManager() {
	//Tell Thread to stop processing
	fProcessNetwork = false;
	//Join thread so that deconstructor won't deallocate
	//before thread finishes
	if (tNetworkInput.joinable()) {
		tNetworkInput.join();
	}
	//Close Socket
	close(fdListenSocket);
}

int OnSite::ServerNetworkManager::startNetwork() {
	//Allow Network Processing
	fProcessNetwork = true;

	//Create and Bind Socket
	fdListenSocket = createBindSocket();

	if (fdListenSocket == -1) {
		std::cout << "CreateBindError, are you root?" << std::endl;
		return -1;
	}

	//Set Socket to NonBlocking Mode
	fdListenSocket = setSocketNonBlocking(fdListenSocket);

	if (fdListenSocket == -1) {
		std::cout <<"Nonblocking Error" <<std::endl;
		return -1;
	}

	//Start Listening on Socket
	fdListenSocket = listenSocket(fdListenSocket);

	if (fdListenSocket == -1) {
		std::cout <<"Socket Listen Error" << std::endl;
		return -1;
	}

	//Setup InputEpoll
	fdEpollInput = createEpoll();

	if (fdEpollInput == -1) {
		return -1;
	}

	fdListenSocket = registerSocket(fdListenSocket, fdEpollInput, &eInputEvent, EPOLLIN);

	//Setup Buffer Array for InputEvents
	aeInputEvents = (struct epoll_event*) malloc(MAXEVENTS * sizeof(eInputEvent));
 
	//Setup OutputEpoll
	fdEpollOutput = createEpoll();
	
	if (fdEpollInput == -1) {
		return -1;
	}

	//Setup Buffer Array for OutputEvents
	aeOutputEvents = (struct epoll_event*) malloc(MAXEVENTS * sizeof(eOutputEvent));

	//Start Network Input Thread
	std::thread t1(&OnSite::ServerNetworkManager::networkInputThread, this);

	//Start Network Output Thread
	std::thread t2(&OnSite::ServerNetworkManager::networkInputThread, this);

	//Swap Input Thread into Slot
	tNetworkInput = std::move(t1);
	
	//Swap Output Thread into Slot
	tNetworkOutput = std::move(t2);
}

int OnSite::ServerNetworkManager::createBindSocket() {
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
	aiHints.ai_flags = AI_PASSIVE; //Listen on all Interfaces
	aiHints.ai_protocol = 0; //Auto-Select Protocol
	aiHints.ai_canonname = nullptr;
	aiHints.ai_addr = nullptr;
	aiHints.ai_next = nullptr;

	rcGAI = getaddrinfo(nullptr, cstrPort, &aiHints, &llaiResult);
 
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

		//Attempt to Bind Socket
		rcGAI = bind(fdSocket, paiNode->ai_addr, paiNode->ai_addrlen);

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
		//Unable to bind
		return -1;
	}
	
	return fdSocket;
	
}	

int OnSite::ServerNetworkManager::setSocketNonBlocking(int fdSocket) {
	int flags, rCode;
	
	//Get Current Flags
	flags = fcntl(fdSocket, F_GETFL, 0);
	
	if (flags == -1) {
		//Unable to get Current Flags
		return -1;
	}

	//Flip NonBlocking Flag Bit
	flags |= O_NONBLOCK;

	//Set New Flags
	rCode = fcntl(fdSocket, F_SETFL, flags);

	if (rCode == -1) {
		//Unable to Set Flags
		return -1;
	}
	
	return fdSocket;
}



int OnSite::ServerNetworkManager::listenSocket(int fdSocket) {
	int rCode;

	//Attempt to Listen on Socket
	rCode = listen(fdSocket, SOMAXCONN); //Set Backlog Length to Max

	if (rCode == -1) {
		return -1;
	}

	return fdSocket;
}

void OnSite::ServerNetworkManager::closeSocket(int fdSocket) {
	close(fdSocket);
	//Remove Socket from SocketList
	//Note:Closing Sockets Automatically Removes them from epoll
}

int OnSite::ServerNetworkManager::createEpoll() {
	//Create epoll file descriptor, with 0 flags
	//int fdEpoll = epoll_create1(0); //epoll_create1 not available in Android
	int fdEpoll = epoll_create(100);
	if (fdEpoll == -1) {
		//Unable to create epoll
		return -1;
	}
	return fdEpoll;
}

int OnSite::ServerNetworkManager::registerSocket(int fdSocket, int fdEpoll, epoll_event* eEvent, int fMonitorEvent) {
	int rCode;

	//Set epoll to watch fdSocket
	eEvent->data.fd = fdSocket;

	//Set epoll to monitor input/output based on mode, and set mode to edge triggered
	eEvent->events = fMonitorEvent | EPOLLET;

	//Associate Event with Socket through Epoll
	rCode = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdSocket, eEvent);	
	
	if(rCode == -1) {
		//Unable to associate Event with Socket
		return -1;
	}

	return fdSocket;
}

void OnSite::ServerNetworkManager::networkInputThread() {
	int rCode;
	int cNumEvents;
	//Check if Network should continue
	while(fProcessNetwork) {
		std::cout << "Waiting for Epoll" << std::endl;
		//Wait for Events (Timeout in ms, -1 = Indefinitely, 0 = immediately)
		cNumEvents = epoll_wait(fdEpollInput, aeInputEvents, MAXEVENTS, -1);
		std::cout << "Epoll Event Received" << std::endl;
		//Process Events
		for(int i = 0; i < cNumEvents; ++i) {
			if ((aeInputEvents[i].events & EPOLLERR) || (aeInputEvents[i].events & EPOLLHUP) || (!(aeInputEvents[i].events & EPOLLIN))) {
				//Error Occured or Socket is not ready for reading
				//Close Socket
				std::cout << "Error: closing socket" << std::endl;
				close(aeInputEvents[i].data.fd);
			}
			//Notification on Listening Socket means Incoming Connection
			else if (fdListenSocket == aeInputEvents[i].data.fd) {
				//Incoming Socket Address
				struct sockaddr saInAddress;
				//Incoming Socket Address Length
				socklen_t slInLength;
				//Incoming Socket File Descriptor
				int fdInSocket;
				//Hostname
				char cstrHost[NI_MAXHOST];
				//Port
				char cstrPort[NI_MAXSERV];
				
				slInLength = sizeof(saInAddress);
					
				do {
					//Attempt to Accept Connection
					fdInSocket = accept(fdListenSocket, &saInAddress, &slInLength);
					
					//Error During Accept
					if (fdInSocket == -1) {
						
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							//Processed All Incoming Connections
							break;
						}
						else {
							//ERROR
							break;
						}
					}

					//Name Info
					//Nice but not necessary
					rCode = getnameinfo(&saInAddress, slInLength, cstrHost, sizeof(cstrHost), cstrPort, sizeof(cstrPort), NI_NUMERICHOST | NI_NUMERICSERV);

					if (rCode == 0) {
						//Name Info Received
						std::cout << "Accepted Connection on Descriptor " << fdInSocket << "[Host=" << cstrHost << ", Port=" << cstrPort << "]" << std::endl;
					}

					//Set Incoming Socket as Non-Blocking
					rCode = setSocketNonBlocking(fdInSocket);

					if (rCode == -1) {
						std::cout << "Error Setting Socket as Nonblocking" << std::endl;
						//ERROR TODO
					}

					//Register Socket for Monitoring
					eInputEvent.data.fd = fdInSocket;
					eInputEvent.events = EPOLLIN | EPOLLET;
					rCode = epoll_ctl(fdEpollInput, EPOLL_CTL_ADD, fdInSocket, &eInputEvent);
					if (rCode == -1) {
						//ERROR TODO
					}
					//Add Client for Input Tracking
					psimInputManager->addClient(fdInSocket);
					//Add Client for Output Tracking
					psomOutputManager->addClient(fdInSocket);
					
							
				} while(true); //Hacky but necessary
				//Loop Around Again
				continue;
			}
			else {
				//Data waiting to be Read
				//Note: Read ALL Data, since we are running edge-triggered
				bool fDone;
				do {
					ssize_t count;
					char buffer[512]; //512 byte buffer
					
					count = read(aeInputEvents[i].data.fd, buffer, sizeof buffer);
					
					if (count == -1) {
						if (errno != EAGAIN) {
							//ERROR on Read
							fDone = true;
						}
						break;
					}
					else if (count == 0) {
						//EOF encountered (Remote closed connection)
						fDone = true;
						break;
					}

					std::shared_ptr<OnSite::NetworkData> ndWrite = std::make_shared<OnSite::NetworkData>(count);
					//Write Data
					//TEST Only, write to standard output
					for (int j = 0; j < count; ++j) {
						std::cout << buffer[j];
					}
					//Pass Bytes to Client Input Manager
					psimInputManager->addBytes(aeInputEvents[i].data.fd, buffer, count);

					//DEBUG
					std::cout <<" " << count << " BYTES RECEIVED" << std::endl;
				} while(!fDone); //Hacky, but necessary due to errno being context specific
				if (fDone) {
					//Close Descriptor
					//NOTE: Closing descriptors automatically removes them from epoll
					std::cout << "Closed connection on descriptor " << aeInputEvents[i].data.fd << std::endl;
					close(aeInputEvents[i].data.fd);
					//Remove Client from Tracking
					psimInputManager->removeClient(aeInputEvents[i].data.fd);
					psomOutputManager->removeClient(aeInputEvents[i].data.fd);
				}
			}
		}
		std::this_thread::yield();
	}
}

//Network Output Thread Code
void OnSite::ServerNetworkManager::networkOutputThread() {
	int rCode;
	int cNumEvents;
	
	while (fProcessNetwork) {
		//Process Outgoing Queue
		
		//Get Sockets to be Written To
		int *afdSockets = nullptr;
		size_t stSocketArrayLength = 0;

		std::tie(afdSockets, stSocketArrayLength) = psomOutputManager->getSockets();
		
		//Register Sockets
		//For Each Socket File Descriptor
		for (size_t i = 0; i < stSocketArrayLength; ++i) {
			//Setup eOutputEvent
			eOutputEvent.data.fd = afdSockets[i];
			//Set Monitoring for Output, in Edge-Triggered Polling Mode
			eOutputEvent.events = EPOLLOUT | EPOLLET;
			//Use eOutputEvent to Register Socket for Monitoring
			rCode = epoll_ctl(fdEpollOutput, EPOLL_CTL_ADD, afdSockets[i], &eOutputEvent);
			//Check if Socket was Registered Properly
			if (rCode == -1) {
				//ERROR TODO
			}
			//Tell OutputBuffer Socket is Being Monitored
			psomOutputManager->setMonitoring(afdSockets[i], true);
		}
		//Done with afdSockets
		free(afdSockets);

		//Wait for Output Events (Timeout is 0 to Return Immediately)
		cNumEvents = epoll_wait(fdEpollOutput, aeOutputEvents, MAXEVENTS, 0);
	
		//For Each Output Event	
		for (int i = 0; i < cNumEvents; ++i) {
			//Check Socket for Errors
			if ((aeOutputEvents[i].events & EPOLLERR) || (aeOutputEvents[i].events & EPOLLHUP) || (!(aeOutputEvents[i].events & EPOLLIN))) {
				//Close the Socket
				std::cout << "Error: closing socket" << std::endl;
				//Note: Closing Socket Automatically Removes from Epoll
				close(aeOutputEvents[i].data.fd);
				//Remove Socket from Input Manager
				psimInputManager->removeClient(aeOutputEvents[i].data.fd);
				//Remove Socket from Output Manager
				psomOutputManager->removeClient(aeOutputEvents[i].data.fd);
			}
			else {
				//Socket is Ready for Writing
				//Byte Array Buffer
				char *aByteBuffer = nullptr;
				//Length of Byte Array
				size_t stBufferSize = 0;
				ssize_t sstReturnCode = 0;
				//Bytes Written
				size_t stBytesWritten = 0;
				//Ask for Bytes to Write
				std::tie(aByteBuffer, stBufferSize) = psomOutputManager->getBytes(aeOutputEvents[i].data.fd);

				//Check if Bytes are Available
				if (aByteBuffer != nullptr) {
					//Bytes Available, Write to Socket
					do {
						//Write Remaining Bytes to Socket
						sstReturnCode = write(aeOutputEvents[i].data.fd, aByteBuffer + stBytesWritten, stBufferSize - stBytesWritten);	
						//If Socket Write was Successful
						if (sstReturnCode != -1) {
							//Count Bytes Written
							stBytesWritten += sstReturnCode;
						}
						//While Socket Write is Successful and Entire Buffer Hasn't Been Written
					} while ((sstReturnCode != -1) && (stBytesWritten != stBufferSize));
					
					//Check if Socket Write was Unsuccessful
					if (sstReturnCode == -1) {
						//Write Unsuccessful, Figure out Why
						//Check if Socket Write Buffer Was Full
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							//Rewind Buffer for Bytes Not Written
							psomOutputManager->rewind(aeOutputEvents[i].data.fd, stBufferSize - stBytesWritten);
						}
						else {
							//ERROR TODO
						}
					}
					else {
						//Socket Write Successful

					}
					//Done with aByteBuffer
					free(aByteBuffer);
				}
				else {
					//No More Bytes to Write
					//Remove Socket from Epoll Monitoring (eOutputEvent is ignored)
					epoll_ctl(fdEpollOutput, EPOLL_CTL_DEL, aeOutputEvents[i].data.fd, &eOutputEvent);
					//Tell OutputManager Socket is No Longer Being Monitored
					psomOutputManager->setMonitoring(aeOutputEvents[i].data.fd, false);
				}
			}
		}

		//No Events Received this round, Use Time to Process Queue
		if (cNumEvents == 0) {
			psomOutputManager->processQueue();
		}
	}
}

std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> OnSite::ServerNetworkManager::getOutputQueue() {
	return pqOutgoing;
}
