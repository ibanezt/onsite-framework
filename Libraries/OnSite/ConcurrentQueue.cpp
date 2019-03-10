//Concurrent Queue Class
#include "ConcurrentQueue.hpp"

//Standard Libraries
#include <mutex>

//TODO Remove
#include <iostream>

template<class gType> OnSite::ConcurrentQueue<gType>::ConcurrentQueue() {
	pcnHead = nullptr;
	pcnTail = nullptr;
}

template<class gType> OnSite::ConcurrentQueue<gType>::~ConcurrentQueue() {
	std::shared_ptr<ConcurrentNode> pLastNode;
	std::shared_ptr<ConcurrentNode> pCurrNode;
	
	pLastNode = pcnHead;

	for (pCurrNode = pLastNode; pLastNode != nullptr; pLastNode = pCurrNode) {
		pCurrNode = pLastNode->pcnNext;
		pLastNode->pcnNext = nullptr;
	}

	pcnHead = nullptr;
	pcnTail = nullptr;
	//Nodes will automatically De-Allocate due to shared_ptr
}

template<class gType> void OnSite::ConcurrentQueue<gType>::push(std::shared_ptr<gType> pgPoint) {
	std::shared_ptr<ConcurrentNode> pcnNode = std::make_shared<ConcurrentNode>(pgPoint);

	std::lock_guard<std::mutex> lock(mLock);
	if (pcnTail != nullptr) {
		pcnTail->pcnNext = pcnNode;
		pcnTail = pcnTail->pcnNext;
	}
	else {
		pcnTail = pcnNode;
		pcnHead = pcnTail;
	}
	//TODO remove
	std::cout << "PUSHED" << std::endl;
}

template<class gType> std::shared_ptr<gType> OnSite::ConcurrentQueue<gType>::pop() {
	std::shared_ptr<gType> pgPointOut = nullptr;
	std::lock_guard<std::mutex> lock(mLock);
	
	if (pcnHead != nullptr) {
		pgPointOut = pcnHead->pgPointer;
		pcnHead = pcnHead->pcnNext;
		if (pcnHead == nullptr) {
			pcnTail = pcnHead;
		}
	}
	else {
		pgPointOut = nullptr;
	}
	return pgPointOut;
}

template<class gType> OnSite::ConcurrentQueue<gType>::ConcurrentNode::ConcurrentNode(std::shared_ptr<gType> pgPoint) {
	pcnNext = nullptr;
	pgPointer = pgPoint;
}

template<class gType> OnSite::ConcurrentQueue<gType>::ConcurrentNode::~ConcurrentNode() {
	pcnNext = nullptr;
	pgPointer = nullptr;
}
