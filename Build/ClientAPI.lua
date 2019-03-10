--[[
	OnSite Client API
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

	--Client Functions and Variables
	local Client = {}

	--Internal Use
	local _Internal = {}
	local NetworkSyncQueue = {}

	_Internal.NetworkSyncQueue = NetworkSyncQueue
	Client._Internal = _Internal

	--Default Empty GameLoop
	local GameLoop = function()
		print("Hello, world!")
	end

	Client.GameLoop = GameLoop

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
		Client.Variables = Variables
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

	Client.newTextureMap = newTextureMap

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

	Client.newGameObject = newGameObject

	Client.GameObjects = GameObjects
	
	OnSite.Client = Client
end
