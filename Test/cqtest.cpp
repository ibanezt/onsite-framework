#include "./../Libraries/OnSite/ConcurrentQueue.hpp"
#include <memory>
#include <iostream>
#include <thread>

class Test {
	public:
	Test(int a){
		value = a;
	}
	~Test() {
	}
	int getValue() {
		return value;
	}
	private:
	int value;
};

void testThread1(std::shared_ptr<OnSite::ConcurrentQueue<Test>> pQueue){
	for (int i = 0; i < 200; ++i) {
		pQueue->push(std::make_shared<Test>(i));
		std::this_thread::yield();
	}
}

void testThread4(std::shared_ptr<OnSite::ConcurrentQueue<Test>>pQueue) {
	std::shared_ptr<Test> pTest;
	for (int i = 0; i < 200; ++i) {
		pTest = pQueue->pop();
		if (pTest != nullptr) {
			std::cout << pTest->getValue() << std::endl;
		}
		std::this_thread::yield();
	}
}
		

void testThread2() {
	for (int i = 0; i < 200; ++i) {
		std::cout << "2: " <<i << std::endl;
		std::this_thread::yield();
	}
}

void testThread3() {
	
	for (int i = 0; i < 200; ++i) {
		std::cout << "3: " << i << std::endl;
		std::this_thread::yield();
	}
}
int main() {
	std::shared_ptr<OnSite::ConcurrentQueue<Test>> testQueue;
	std::cout << "Constructing" << std::endl;
	testQueue = std::make_shared<OnSite::ConcurrentQueue<Test>>();
	std::cout << "Constructed" << std::endl;
	std::thread tNetwork = std::thread(&testThread1, testQueue);
	std::thread tNet2 = std::thread(&testThread4, testQueue);
	/*
	std::thread tTest (&testThread2);
	tTest.join();
	std::thread tTest2 (&testThread3);
	tTest2.join();
	for (int i = 0; i < 200; ++i) {
		std::cout << i << std::endl;
		std::this_thread::yield();
	}
	tTest.join();
	tTest2.join();
	*/
	int x = 0;
	for (int i = 0; i < 600; ++i) {
		x = x++;
		std::this_thread::yield();
	}
	tNetwork.join();
	tNet2.join();
	return 0;
}
