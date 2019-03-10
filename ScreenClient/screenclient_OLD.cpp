/* *  
   *	Onsite Framework
   *	Terrell Ibanez
   *	
   * */

/*	Design:
	Four Threads
	Main / Scheduler
	GUI / Render
	Game Logic
	Network Logic
*/

/*	Hungarian Apps Notation
	c = counter
	t = Thread
	f = Flag
	p = pointer
*/

//Standard Libraries
#include <iostream>
#include <memory>
#include <thread>

//OnSite Libraries
#include "NetworkManager.hpp"
//Game Class

//Scene Class

//SceneRenderer Class

//GameObject Class


int main(int argc, char **argv) {
	//Initalize
	//Start RenderManager Thread
	//Start Network Thread
	std::shared_ptr<NetworkManager> pnmNetManager = std::make_shared<OnSite::NetworkManager>();
	pnmNetManager->startNetwork();

	int i = 0;
	while (i < 1000) {
		//Wait for Scheduled Start Time

		//Decide Which Game to Run
		//Start Associating Clients
		//Request Game from Distribution Server
		//Wait for Game to Load
		//Wait for Clients to Load Game
		//Synchronize Start Time
		//Run Game
		//Wait for Game to Finish
		//Signal End of Game
		//Clean Up
		i++;
		std::cout << "i: " << i << std::endl;
		std::this_thread::yield();
	}

	return 0;
}


