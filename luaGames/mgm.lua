--push space to win
screen = {}
screen.width = 1920 --TODO: integreate api
screen.height = 1080 
win = 1000
minPlayers = 1

winT = OnSite.Server.newTextureMap(3685, 34, 4446, 419, 0)
loseT = OnSite.Server.newTextureMap(2575, 20, 3504, 551, 0)
nullT = OnSite.Server.newTextureMap(3564, 67, 1, 1, 0)
carSpash = OnSite.Server.newTextureMap(4895, 40, 1091, 497, 0)
dragonSpash = OnSite.Server.newTextureMap(5057, 1258, 746, 414, 0)
lightSpash = OnSite.Server.newTextureMap(5130, 571, 665, 664, 0)
plateSpash = OnSite.Server.newTextureMap(6116, 27, 484, 414, 0)

--backgrounds
lightbg = OnSite.Server.newTextureMap(0,0, 1920, 1080)
carbg = OnSite.Server.newTextureMap(0,1080, 1920, 1080)
dragonbg = OnSite.Server.newTextureMap(0, 2165, 1920, 1080)
platebg = OnSite.Server.newTextureMap(0, 3246, 1920, 1080)

gameState = -2 --prepregame:-2 pregame: -1, running: 0, win: 1, lose: 2

bg = OnSite.Server.newGameObject()

timer = {}
timer.time = 0
timer.set = function(t)
	timer.time = t
end
timer.setS = function(s)
	timer.time = s * 30
end
timer.tick = function()
	timer.time = timer.time - 1
	return timer.time < 1
end

cpuT = OnSite.Server.newTextureMap(1946, 1165, 2472, 1400, 0) --TODO: Fix this
audiT = OnSite.Server.newTextureMap(1946, 1421, 2472, 1657, 0)

frame = 0

message = OnSite.Server.newGameObject()
message.setTexture = function(txt)
	message.TextureMap = txt
	message.Position.X = screen.width / 2 - txt.width
	message.Position.Y = screen.height / 2 - txt.height
end

--Connect clients

numClients = #OnSite.Server.Clients

for i, c in ipairs(OnSite.Server.Clients) do
	c.Variables.Local.previousPushes = 0
end

RoadTop = screen.height / 3
RoadBottom = screen.height * 2 / 3



Car = function(yPos)
	local car = {}
	car.gameobj = OnSite.Server.newGameObject()
	car.gameobj.Position.X = 20
	car.gameobj.Position.Y = yPos
	car.setTexure = function (txt)
		car.gameobj.TextureMap = txt
	end
	
	car.move = function(amt)
		car.gameobj.Position.X = car.gameobj.Position.X + amt
	end
	return car
end




cpuCar = Car(RoadTop + 5)
cpuCar.setTexure(cpuT)

audiCar = Car(RoadBottom - 5)
audiCar.setTexure(audiT)

carGameLoop = function()
	if gameState == -2 then
		timer.setS(5)
		gameState = -1
		message.setTexture(carSplash)
		bg.TextureMap = carbg
	elseif gameState == -1 then
		if timer.tick() == true then
			message.setTexture(nullT)
			gameState = 0	
		end
	elseif (gameState == 0) then
		for i, c in ipairs(OnSite.Server.Clients) do
			audiCar.move(c.Variables.Sync.pushes - c.Variables.Local.previousPushes * factor)
			c.Variables.previousPushes = c.Variables.Sync.pushes
		end
		
		audiCar.move(idle)
		cpuCar.move(carIdle * numClients) --TODO: rubberband?
		if (cpuCar < audiCar) then
			message.setTexture(winT)
			timer.setS(5)
			gameState = 1
		else
			message.setTexture(loseT)
			timer.setS(5)
			gameState = 1
		end
	else	
		if timer.tick() == true then
			message.setTexture(nullT)
			gameState = -2
			OnSite.Server.GameLoop = DragonGameLoop
		end
	end
end

OnSite.Server.GameLoop = carGameLoop

downSpeed = 5
upSpeed = 5

DragonHeight = 420
DragonWidth = 79

ArrowHeight = 122
ArrowWidth = 28

curHP = 4
incHP = 2
spawnTwo = 10
spawnThree = 16

castleLine = ScreenSize.h * 2 / 3

--x Positions for the 'lanes'
lanePos = {}
lanePos.left = w * 3 / 8
lanePos.mid = w / 2
lanePos.right = w * 5 / 8

dragonText = OnSite.Server.newTextureMap(1924, 2165, DragonHeight, DragonWidth, 0)
arrowText= OnSite.Server.newTextureMap(1933, 2335,  ArrowHeight, ArrowWidth, 0) 

Dragon = function (hitpoints, lane)
	local d = {}
	d.hp = hitpoints
	d.lane = lane
	d.gameObj = OnSite.Server.newGameObject()
	d.gameObj.TextureMap = dragonText
	if lane == 0 then
		d.gameObj.Position.X = lanePos.mid
	end
	
	if lane == 1 then
		d.gameObj.Position.X = lanePos.left
	end
	
	if lane == 2 then
		d.gameObj.Position.X = lanePos.right
	end
	return d
end

Arrow =  function (lane)
	local a = {}
	a.lane = lane
	a.gameObj = OnSite.Server.newGameObject()
	a.gameObj.TextureMap = arrowText
	if lane == 0 then
		a.gameObj.Position.X = lanePos.mid
	end
	
	if lane == 1 then
		a.gameObj.Position.X = lanePos.left
	end
	
	if lane == 2 then
		a.gameObj.Position.X = lanePos.right
	end
	
	a.gameObj.Position.Y = castleLine
	
	return a
end

Dragons = {}
Arrows = {}
numAdd = 1


DragonGameLoop = function()
	if gameState == -2 then
		timer.setS(5)
		message.setTexture(dragonSplash)
		bg.TextureMap = dragonbg
		gameState = -1
	elseif gameState == -1 then
		if timer.tick() == true then
			gameState = 0
			message.setTexture(nullT)
		end
	elseif (gameState == 1) then
		--move arrows upward
		for i, v in ipairs(Arrows) do 
			--if Arrow still exists
			if v ~= nil then
				v.gameObj.Position.Y = v.gameObj.Position.Y - upSpeed
				for j, d in ipairs(Dragons) do
					--if an arrow hits the dragon
					if v.lane == d.lane and v.gameObj.Position.Y < d.gameObj.Position.Y + DragonHeight then
						--hurt it
						d.hp = d.hp - arrowPower
						--if the dragon dies
						if d.hp < 1 then
							d.TextureMap = nullT
							--remove it and increase difficulty
							table.remove(Dragons, j)
							curHP = curHP + incHP
							if curHP > spawnTwo  and numAdd == 1 then
								numAdd = 2
								curHP = curHP * 2 / 3
							end
							
							if curHP > spawnThree and numAdd == 2 then
								numAdd = 3
								curHP = curHP * 2 / 3
							end
						end
						--then remove the arrow
						v.TextureMap = nullT
						table.remove(Arrows, i)
					end
				end
			end
		end
		
		--if there are no dragons add some
		if #Dragons == 0 then
			for i = 0, numAdd do
				table.insert(Dragons, dragon(curHP, i))
			end
		end
				
		--move dragons downward
		for i, d in ipairs(Dragons) do
			d.Position.Y = d.Position.Y + downSpeed
			if d.Position > castleLine then
				gamestate = 1
				timer.setS(5)
				message.setTexture(loseT)
			end
		end
		
		--Spawn an arrow for each button pushed
		for i, c in ipairs(numClients) do
			if c.Variables.Sync.pushes > c.Variables.Local.previousPushes then
				table.insert(Arrows, arrow(Dragon[math.random(#Dragons)].lane))
				c.Variables.Local.previousPushes = c.Variables.Sync.pushes
			end	
		end
	else	
		if timer.tick() == true then
			message.setTexture(nullT)
			gameState = -2
			OnSite.Server.GameLoop = PlateGameLoop
		end
	end
end

center = ScreenSize.width / 2
plateText = Onsite.Server.newTextureMap(1925, 3243, 229, 269, 0)
fallSpeed = 5
Plate = function(x, y, numTaps)
	local p = {}
	p.gameObj = Onsite.Server.newGameObject()
	p.gameObj.Position.X = x
	p.gameObj.Position.Y = y
	p.gameObj.TextureMap = plateText
	p.tapsLeft = numTaps
	p.falling = false
	return p
end
plates = {}
for i = 1, 5 do
	plates[i] = Plate(300 * (i - 1), 50, 15 + (7 * i * #Onsite.Server.Clients))
end

PlateGameLoop = function()
	if gameState == -2 then
		timer.setS(5)
		message.setTexture(plateSplash)
		bg.TextureMap = platebg
		gameState = -1
	elseif gameState == -1 then
		if timer.tick() == true then
			message.setTexture(nullT)
			gameState = 0
	elseif gameState == 1 then
		for i, c in ipairs(Onsite.Server.Clients) do
			if c.Variables.Sync.pushes > c.Variables.Local.previousPushes then
				for j, p in ipairs(plates) do
					p.tapsLeft = p.tapsLeft - 1
					if p.tapsLeft < 0 then
						p.falling = true
					end
				end
				c.Variables.Local.previousPushes = c.Variables.Sync.pushes
			end
		end
		
		for i, p in ipairs(plates) do
			if p.falling == true then
				p.gameObj.Position.Y = p.gameObj.Position.Y + fallSpeed
				if p.gameObj.Position.Y > ScreenSize.height - 300 then
					p.gameObj.TextureMap = nullT
					table.remove(plates, i)
				end
			end
		end

		if #plates == 0
			message.setTexture(winT)
			gameState = 1
			timer.setS(5)
		end
	else
		if timer.tick() == true then
			message.setTexture(lightSplash)
			timer.setS(5)
			bg.TextureMap = lightbg
			gameState = -1
		end
	end
end	


light = Onsite.Server.newGameObject()


greenTex = Onsite.Server.newTextureMap(1922, 0, 58, 60, 0)
redTex = Onsite.Server.newTextureMap(1922, 122, 58, 60, 0)
yellowTex = Onsite.Server.newTextureMap(1922, 60, 58, 60, 0)

light.TextureMap = greenText
greenAvg = 150
greenVar = 60
yellowTime = 30
redAvg = 120
redVar = 60

player = Car(ScreenSize.height / 2)
playerText = OnSite.Server.newTextureMap(1924, 192, 183, 308, 0)
framesLeft = greenAvg + (math.random(greenVar * 2) - greenVar)
--lightState: 0 green, 1 yellow , 2 red
lightState = 0
frames 
= 1800

LightGameLoop = function()
	if gameState == -1 then
		if timer.tick() == true then
			message.setTexture(nullT)
			timer.setS(60)
			gameState = 0
		end
	elseif (gameState == 0)		

		if framesLeft < 1 then
			if lightState == 0 then
				framesLeft = yellowTime
				lightState = 1
			elseif lightState == 1 then
				framesLeft = redAvg + (math.random(redVar * 2) - redVar)
				lightState = 2
			else
				framesLeft = greenAvg + (math.random(greenVar * 2) - greenVar)
				lightState = 0
			end
		end
		if lightState ~= 2
			multi = 1
		else
			multi = -1
		end
		
		for i, c in ipairs(Onsite.Server.Clients) do
			player.move((c.Variables.Sync.pushes - c.Variables.Local.previousPushes * factor) * multi)
			if player.gameObj.Position.X < win then
				gameState = 1
				message.setTexture(winT)
				timer.setS(5)
			elseif player.gameObj.Position.X < 20 then
				player.gameObj.Position.X = 20
			end
			c.Variables.previousPushes = c.Variables.Sync.pushes
		end
		
		if timer.tick < 1 then
			gameState = 1
			message.setTexture(loseT)
			timer.setS(5)
		end
	else 
		if timer.tick() == true then
			message.setTexture(nullT)
		end
	end
