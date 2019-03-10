#include <chrono>
#include <memory>
#include <thread>

#include "../Libraries/OnSite/ClientGameEngine.hpp"

int main() {
	std::shared_ptr<OnSite::ClientGameEngine> cgEngine = std::make_shared<OnSite::ClientGameEngine>();
	cgEngine->startClient();

	while (!cgEngine->checkFinished()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
