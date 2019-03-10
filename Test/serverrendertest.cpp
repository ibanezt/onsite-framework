#include <memory>
#include "../Libraries/OnSite/ServerRenderManager.hpp"

int main() {
	std::shared_ptr<OnSite::ServerRenderManager> x;
	x = std::make_shared<OnSite::ServerRenderManager>();
	while(true) {
		x->render();
		x->updateWindow();
	}
	return 0;
}
