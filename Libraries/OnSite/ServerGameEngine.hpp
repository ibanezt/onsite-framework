/**
 *	ServerGameEngine Class Declaration
 *	Terrell Ibanez
**/

//C++ Standard Libraries
#include <chrono>
#include <memory>
#include <thread>

//OnSite Libraries
#include "ConcurrentQueue.hpp"
#include "MessagePacket.hpp"
#include "ServerRenderManager.hpp"
#include "ServerNetworkManager.hpp"
#include "ServerLuaManager.hpp"
#include "Scene.hpp"

namespace OnSite {
	class ServerGameEngine {
		public:
			//Constructor
			ServerGameEngine();
			//Destructor
			~ServerGameEngine();
			//Start Server
			void startServer();
			//Setup Game
			void initializeGame();
			//Check if Game Finished
			bool checkFinished();
		private:
			//Process Network Input
			void processNetworkInput(std::shared_ptr<OnSite::MessagePacket> pmPacket);
			//Render Frame
			void render();

			//Game Loop Thread
			std::thread mtGameLoop;
			//Game Execute Flag
			bool mfRunGame;
			//Frame Start Time Point
			std::chrono::time_point<std::chrono::high_resolution_clock> mtpFrameStartTime;
			//Frame Stop Time Point
			std::chrono::time_point<std::chrono::high_resolution_clock> mtpFrameStopTime;
			//Time Delta
			std::chrono::nanoseconds mnsMaxDeltaTime;
			//Time Elapsed in Nanoseconds
			std::chrono::nanoseconds mnsTimeElapsed;
			//Current Scene
			std::shared_ptr<Scene> msCurrScene;
			//Change Scene
			void setScene(std::shared_ptr<Scene> msNewScene);
			//Game Loop Code
			void gameLoopThread();
			
			//Render Engine
			std::shared_ptr<OnSite::ServerRenderManager> msrmRenderer;

			//Network Engine
			std::shared_ptr<OnSite::ServerNetworkManager> msnmNetwork;

			//Lua Engine
			std::shared_ptr<OnSite::ServerLuaManager> mslmLua;
	};
}
