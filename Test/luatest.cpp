#include <memory>

#include "../Libraries/OnSite/ConcurrentQueue.hpp"
#include "../Libraries/OnSite/MessagePacket.hpp"
#include "../Libraries/OnSite/ServerLuaManager.hpp"

int main() {
	std::shared_ptr<OnSite::ConcurrentQueue<OnSite::MessagePacket>> pCQ = std::make_shared<OnSite::ConcurrentQueue<OnSite::MessagePacket>>();

	std::shared_ptr<OnSite::ServerLuaManager> x = std::make_shared<OnSite::ServerLuaManager>(pCQ);
	x->runGameLoop();
	return 0;
}
