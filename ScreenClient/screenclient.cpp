
//C++ Libraries
#include <chrono>
#include <memory>
#include <thread>

//Onsite Libraries
#include "../Libraries/OnSite/ServerGameEngine.hpp"

int main() {
	std::shared_ptr<OnSite::ServerGameEngine> sgEngine = std::make_shared<OnSite::ServerGameEngine>();
	sgEngine->startServer();

	while (!sgEngine->checkFinished()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
