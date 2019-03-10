//Concurrent Queue Class
#include "ConcurrentQueue.hpp"

//Standard Libraries
//#include <atomic>
#include <iostream>

template<class gType> OnSite::ConcurrentQueue<gType>::ConcurrentQueue() {
	//Create Dummy Node
	std::shared_ptr<ConcurrentNode> pcnNewNode = std::make_shared<ConcurrentNode>(nullptr);
	//NOTE: DUMMY NODE PREVENTS RACE CONDITIONS
	//DO NOT CHANGE

	//Point Head / Tail to Dummy Node
	std::cout << "CRead 1" << std::endl;
	pcnHead = pcnNewNode;
	std::cout << "CRead 2" << std::endl;
	pcnTail = pcnHead.load();
	std::cout << "Finished CRead" << std::endl;
	pcnNewNode = nullptr;
}

template<class gType> OnSite::ConcurrentQueue<gType>::~ConcurrentQueue() {
	std::shared_ptr<ConcurrentNode> pLastNode;
	std::shared_ptr<ConcurrentNode> pCurrNode;
	
	pLastNode = pcnHead;

	for (pCurrNode = pLastNode; pLastNode != nullptr; pLastNode = pCurrNode) {
		pCurrNode = pLastNode->pcnNext.load();
		pLastNode->pcnNext = nullptr;
	}

	pcnHead = nullptr;
	pcnTail = nullptr;
	//Nodes will automatically De-Allocate due to shared_ptr
}

template<class gType> void OnSite::ConcurrentQueue<gType>::enqueue(std::shared_ptr<gType> pgPoint) {
	std::shared_ptr<ConcurrentNode> pcnNode = std::make_shared<ConcurrentNode>(pgPoint);
	std::shared_ptr<ConcurrentNode> pGetTail;
	std::shared_ptr<ConcurrentNode> pGetTailNext;

	bool fCompleted = false;
	do {
		pGetTail = pcnTail;
		pGetTailNext = pcnTail.load()->pcnNext;
		if (pGetTail == pcnTail.load()) {
			if (pGetTailNext == nullptr) {
				std::cout<<"pGetTailNext:nullptr"<<std::endl;
				if (pGetTail->pcnNext.compare_exchange_strong(pGetTailNext, pcnNode), std::memory_order_release, std::memory_order_relaxed) {
					fCompleted = true;
				}
			}
			else {
				pcnTail.load()->pcnNext.compare_exchange_strong(pGetTail, pGetTailNext, std::memory_order_release, std::memory_order_relaxed);
			}
		}
		//std::cout <<"Loop"<<std::endl;
	} while (fCompleted == false);
	pcnTail.compare_exchange_strong(pGetTail, pcnNode, std::memory_order_release, std::memory_order_relaxed);
}

template<class gType> std::shared_ptr<gType> OnSite::ConcurrentQueue<gType>::dequeue() {
	bool fCompleted = false;

	std::shared_ptr<ConcurrentNode> pGetHead;
	std::shared_ptr<ConcurrentNode> pGetTail;
	std::shared_ptr<ConcurrentNode> pGetNext;

	std::shared_ptr<gType> pgPointOut;

	do {
		pGetHead = pcnHead;
		pGetTail = pcnTail;
		pGetHead = pcnHead->pcnNext;
		
		if (pGetHead == pcnHead) {
			if (pcnHead->gObject == pcnTail) {
				if (pGetHead == nullptr) {
					pgPointOut = nullptr;
					fCompleted = true;
				}
				pcnTail.compare_and_exchange_weak(pGetTail, pGetNext);
			}
			else {
				pgPointOut = pGetNext->ptPointer;
				if (pcnHead.compare_and_exchange(pcnHead, pGetNext, std::memory_order_release, std::memory_order_relaxed)) {
					fCompleted = true;
				}
			}
		}		
	}
	while (fCompleted == false);
	pGetHead->pcnNext = nullptr;
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
