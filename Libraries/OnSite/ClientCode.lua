--Init Code
print("INIT START")
local g = OnSite.Client.newGameObject()
local t1 = OnSite.Client.newTextureMap(0, 0, 64, 205, 0)
local t2 = OnSite.Client.newTextureMap(64, 0, 64, 205, 0)
local t3 = OnSite.Client.newTextureMap(128, 0, 64, 205, 0)
local t4 = OnSite.Client.newTextureMap(196, 0, 64, 205, 0)

local textures = {t1, t2, t3, t4}

print("OBJECTS CREATED")
g.Position.X = 200
g.Position.Y = 200

g.TextureMap = t1

local i = 1

print("INIT END")
--Set Game Loop Code
OnSite.Client.GameLoop = function()
	i = i + 1
	if (i > 4) then
		i = 1
	end
	g.TextureMap = textures[i]
	g.Position.X = (g.Position.X + 10) % 500
	print("GAME LOOP RUN")
end
print("GAME LOOP SET")
