--[[
	OnSite Server API
	Terrell Ibanez
--]]

--Everything Goes into OnSite Table
OnSite = {}

--Keep Definitions from Polluting Global Namespace
do
	--Lua Libraries
	local Lib = {}
	--TODO:Sandboxing
	Lib.string = string
	Lib.math = math
	Lib.table = table
	local Data = {}
	local Queue = {}
	do
		local Push = function(t, x)
			table.insert(t,x)
		end

		local Pop = function(t)
			return table.remove(t)
		end
		Queue.Push = Push
		Queue.Pop = Pop
		Data.Queue = Queue
	end

	Lib.Data = Data
	OnSite.Lib = Lib

	--Server Functions and Variables
	local Server = {}

	--Internal Use
	local _Internal = {}
	local NetworkSyncQueue = {}

	_Internal.NetworkSyncQueue = NetworkSyncQueue
	Server._Internal = _Internal

	--Default Empty GameLoop
	local GameLoop = function()
		print("Hello, world!")
	end

	Server.GameLoop = GameLoop

	do 
		local Variables = {}
		local Local = {}
		Variables.Local = Local

		--Sync Table uses Proxy Table with MetaTables to Automatically Track Changes
		local Sync = {}
		local _Sync = Sync
		Sync = {}

		local metaTable = {
			__index = function(t, Key)
				return _Sync[Key]
			end,

			__newindex = function(t, Key, Value)
				_Sync[Key] = Value
				local t = {}
				t.ID = -1
				t.NAME = KEY
				OnSite.Lib.Data.Queue.Push(NetworkSyncQueue, t)
			end
		}
		setmetatable(Sync, metaTable)

		Variables.Sync = Sync
		Server.Variables = Variables
	end

	--Clients List
	local Clients = {}
	
	local newClient = function(id) 
		local t = {}
		local Variables = {}
		local Local = {}
		Variables.Local = Local
		local Sync = {}
		local _Sync = Sync
		Sync = {}

		local metaTable = {
			__index = function(t, Key)
				return _Sync[Key]
			end,

			__newindex = function(t, Key, Value)
				_Sync[Key] = Value
				local t2 = {}
				t2.ID = id
				t2.NAME = KEY
				OnSite.Lib.Data.Queue.Push(NetworkSyncQueue, t2)
			end
		}
		setmetatable(Sync, metaTable)

		Variables.Sync = Sync
		t.Variables = Variables
		t.ID = id
		return t
	end

	_Internal.newClient = newCLient;

	local addClient = function(id)
		table.insert(Clients, newClient(id))
	end

	_Internal.addClient = addClient;

	local getClientByID = function(id)
		for i = 1, #Clients do
			if (Clients[i].ID == id) then
				return Clients[i]
			end
		end
	end

	local newTextureMap = function(x, y, width, height, rotation)
		local t = {}
		t.X = x
		t.Y = y
		t.Width = width
		t.Height = height
		t.Rotation = rotation
		return t
	end

	Server.newTextureMap = newTextureMap

	local GameObjects = {}

	local newGameObject = function() 
		local g = {}
		g.Position = {}
		g.Position.X = 0
		g.Position.Y = 0
		g.TextureMap = nil
		
		table.insert(GameObjects, g)
		return g
	end

	--Internal Table for TextureMap Data
	local TextureMaps = {}
	
	--Internal Function to Prepare TextureMap Data
	local prepareTextureMaps = function()
		--Clear TextureMaps Table
		local TextureMaps = {}
		
		for i=1, #GameObjects do
			if (GameObjects[i].TextureMap ~= nil) then
				local t = {GameObjects[i].Position.X, GameObjects[i].Position.Y, GameObjects[i].TextureMap.X, GameObjects[i].TextureMap.Y, GameObjects[i].TextureMap.Width, GameObjects[i].TextureMap.Height, GameObjects[i].TextureMap.Rotation}
				table.insert(TextureMaps, t);
			end
		end
		_Internal.TextureMaps = TextureMaps
	end

	_Internal.TextureMaps = TextureMaps
	_Internal.prepareTextureMaps = prepareTextureMaps

	Server.newGameObject = newGameObject

	Server.GameObjects = GameObjects
	
	Server.Clients = Clients
	OnSite.Server = Server
end
