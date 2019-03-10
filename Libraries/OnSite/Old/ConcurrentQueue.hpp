#ifndef GUARD_CONCURRENTQUEUE_H
#define GUARD_CONCURRENTQUEUE_H

//NonBlocking Concurrent Queue
//Implemented as a Singly Linked List

//Standard Libraries
#include<atomic>
#include<memory>

namespace OnSite {
	template<class gType> class ConcurrentQueue {
		public:
			ConcurrentQueue();
			~ConcurrentQueue();
			void enqueue(std::shared_ptr<gType> pgPoint);
			std::shared_ptr<gType> dequeue();
		private:
			class ConcurrentNode {
				public:
					ConcurrentNode(std::shared_ptr<gType> pgPoint);
					~ConcurrentNode();
					//No Getters / Setters due to Atomic compare_and_exchange
					//Shouldn't matter anyway, this is a private class
					std::atomic<std::shared_ptr<ConcurrentNode>> pcnNext;
					std::shared_ptr<gType> pgPointer;
			};
			std::atomic<std::shared_ptr<ConcurrentNode>> pcnHead;
			std::atomic<std::shared_ptr<ConcurrentNode>> pcnTail;
			
	};
}
#include "ConcurrentQueue.cpp"
#endif

