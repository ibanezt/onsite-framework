testDir = ./Test
buildDir = ./Build/Object
binDir = ./Build
libDir = ./Libraries/OnSite
screenDir = ./ScreenClient

flags = -Wl,-no-as-needed -std=c++11
linkLua = -llua
linkThread = -lpthread
linkprotobuf2 = `pkg-config --cflags --libs protobuf`
compileprotobuf = `pkg-config --cflags protobuf`
linkprotobuf = -lprotobuf
linkSOIL = -lSOIL
linkSDL2 = -lSDL2 -lSDL2_image
linkDynamic = -ldl
screenclient: ServerGameEngine.o
	g++ $(flags) $(screenDir)/screenclient.cpp $(wildcard $(buildDir)/*.o) $(linkLua) $(linkSDL2) $(linkprotobuf) $(linkThread) $(linkDynamic) -o $(binDir)/screenclient
serverrendertest: ServerRenderManager.o
	g++ $(flags) $(testDir)/serverrendertest.cpp $(addprefix $(buildDir)/,$^) $(linkSDL2) -o $(binDir)/serverrendertest
cqtest: 
	g++ $(flags)  $(testDir)/cqtest.cpp $(linkThread) -o $(binDir)/cqtest
clientsim: MessagePacket.o PPacket.o
	g++ $(flags) $(testDir)/clientsim.cpp $(addprefix $(buildDir)/,$^) $(linkprotobuf) -o $(binDir)/clientsim
luatest: MessagePacket.o ServerLuaManager.o
	g++ $(flags) $(testDir)/luatest.cpp $(wildcard $(buildDir)/*.o) $(linkLua) $(linkprotobuf) -ldl -o $(binDir)/luatest
nettest: ServerInputManager.o ServerOutputManager.o ServerNetworkManager.o NetworkData.o MessagePacket.o InputBuffer.o OutputBuffer.o PPacket.o
	g++ $(flags)  $(testDir)/nettest.cpp $(addprefix $(buildDir)/,$^) $(linkThread) $(linkprotobuf) -o $(binDir)/nettest
cmtest: ClientManager.o
	g++ $(flags) $(testDir)/cmtest.cpp $(buildDir)/ClientManager.o $(buildDir)/Connection.o -o $(binDir)/cmtest
clienttest: ClientGameEngine.o
	g++ $(flags) $(testDir)/clienttest.cpp $(wildcard $(buildDir)/*.o) $(linkLua) $(linkSDL2) $(linkprotobuf) $(linkThread) $(linkDynamic) -o $(binDir)/clienttest
clean: 
	rm $(buildDir)/*.o
.PHONY : clean
ClientGameEngine.o: ClientNetworkManager.o ClientRenderManager.o ClientLuaManager.o MessagePacket.o
	g++ -c $(flags) $(libDir)/ClientGameEngine.cpp -o $(buildDir)/ClientGameEngine.o
ClientNetworkManager.o: ClientInputManager.o ClientOutputManager.o
	g++ -c $(flags) $(libDir)/ClientNetworkManager.cpp -o $(buildDir)/ClientNetworkManager.o
ClientRenderManager.o: Texture.o
	g++ -c $(flags) $(libDir)/ClientRenderManager.cpp -o $(buildDir)/ClientRenderManager.o
ClientLuaManager.o: LuaAPI.o
	g++ -c $(flags) $(libDir)/ClientLuaManager.cpp -o $(buildDir)/ClientLuaManager.o
ClientInputManager.o: InputBuffer.o
	g++ -c $(flags) $(libDir)/ClientInputManager.cpp -o $(buildDir)/ClientInputManager.o
ClientOutputManager.o: OutputBuffer.o
	g++ -c $(flags) $(libDir)/ClientOutputManager.cpp -o $(buildDir)/ClientOutputManager.o

ServerGameEngine.o: ServerNetworkManager.o ServerRenderManager.o ServerLuaManager.o MessagePacket.o
	g++ -c $(flags) $(libDir)/ServerGameEngine.cpp -o $(buildDir)/ServerGameEngine.o
ServerInputManager.o: InputBuffer.o
	g++ -c $(flags) $(libDir)/ServerInputManager.cpp -o $(buildDir)/ServerInputManager.o
ServerOutputManager.o: OutputBuffer.o
	g++ -c $(flags) $(libDir)/ServerOutputManager.cpp -o $(buildDir)/ServerOutputManager.o
ServerRenderManager.o: Texture.o
	g++ -c $(flags) $(libDir)/ServerRenderManager.cpp -o $(buildDir)/ServerRenderManager.o
MessagePacket.o: PPacket.o
	g++ -c $(flags) $(libDir)/MessagePacket.cpp -o $(buildDir)/MessagePacket.o
ServerNetworkManager.o: ServerInputManager.o ServerOutputManager.o ServerRenderManager.o MessagePacket.o
	g++ -c $(flags) $(libDir)/ServerNetworkManager.cpp $(linkThread) -o $(buildDir)/ServerNetworkManager.o
LuaAPI.o:
	cp ./luaGames/mgm.lua ./Libraries/OnSite/ServerCode.lua
	cp $(wildcard $(libDir)/*.lua) $(binDir)/
NetworkData.o:
	g++ -c $(flags) $(libDir)/NetworkData.cpp -o $(buildDir)/NetworkData.o
ConcurrentQueue.o:
	g++ -c $(flags) $(libDir)/ConcurrentQueue.cpp -o $(buildDir)/ConcurrentQueue.o
Texture.o:
	g++ -c $(flags) $(libDir)/Texture.cpp -o $(buildDir)/Texture.o
InputBuffer.o:  MessagePacket.o NetworkData.o
	g++ -c $(flags) $(libDir)/InputBuffer.cpp -o $(buildDir)/InputBuffer.o
OutputBuffer.o: MessagePacket.o NetworkData.o
	g++ -c $(flags) $(libDir)/OutputBuffer.cpp -o $(buildDir)/OutputBuffer.o
PPacket.o:
	protoc -I=$(libDir)/ --cpp_out=$(libDir)/ $(libDir)/PPacket.proto
	g++ -c $(flags) $(libDir)/PPacket.pb.cc -o $(buildDir)/PPacket.o
ServerLuaManager.o: LuaAPI.o MessagePacket.o
	g++ -c $(flags) $(libDir)/ServerLuaManager.cpp -o $(buildDir)/ServerLuaManager.o
