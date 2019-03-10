#ifndef GUARD_CONCURRENTQUEUE_H
#define GUARD_CONCURRENTQUEUE_H

//Locking Concurrent Queue
//Implemented as a Singly Linked List

//C++ Standard Libraries
#include<mutex>
#include<memory>

namespace OnSite {
	template<class gType> class ConcurrentQueue {
		public:
			ConcurrentQueue();
			~ConcurrentQueue();
			void push(std::shared_ptr<gType> pgPoint);
			std::shared_ptr<gType> pop();
		private:
			class ConcurrentNode {
				public:
					ConcurrentNode(std::shared_ptr<gType> pgPoint);
					~ConcurrentNode();

					std::shared_ptr<ConcurrentNode> pcnNext;
					std::shared_ptr<gType> pgPointer;
			};
			std::shared_ptr<ConcurrentNode> pcnHead;
			std::shared_ptr<ConcurrentNode> pcnTail;
			std::mutex mLock;
	};
}
#include "ConcurrentQueue.cpp"
#endif

