#include "./../Libraries/OnSite/ServerNetworkManager.hpp"

#include <memory>
#include <iostream>

int main() {
	std::shared_ptr<OnSite::ServerNetworkManager> test = std::make_shared<OnSite::ServerNetworkManager>(); 
	std::cout << "Starting Network" << std::endl;
	test->startNetwork();
	std::cout << "Joining" <<std::endl;
	test->join();
	std::cout << "EOP" <<std::endl;
	return 0;
}
