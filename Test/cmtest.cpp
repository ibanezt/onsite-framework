#include <memory>

#include "./../Libraries/OnSite/ClientManager.hpp"

int main() {
	std::shared_ptr<OnSite::ClientManager> pCM;
	
	pCM = std::make_shared<OnSite::ClientManager>();	
	return 0;
}
