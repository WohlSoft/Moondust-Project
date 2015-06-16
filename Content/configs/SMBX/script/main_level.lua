
local peachPlayer_class = require("player/peach")

local npcBase_class = require("npcs/main_npc")

-- We need to reference all placed objects with lua or the garbage collector will hit it
local players = {}
local npcs = {}

-- Debug stuff
local deb_i = 0
local function temp_count(t) local i = 0 for _,__ in pairs(t) do i = i + 1 end return i end
-- Debug stuff end

function onLoop()
    deb_i = deb_i + 1
    Renderer.printText("Ticks passed: "..deb_i, 100, 130, -1, 15, 0xFFFF0055)
	if(npc_class_table)then
		Renderer.printText(temp_count(npc_class_table).." Npc registered!", 100, 160, -1, 15, 0xFFFF0055)	
	else
		Renderer.printText("No Npc registered!", 100, 160, -1, 15, 0xFFFF0055)
	
	end
end

function __native_event(eventObj, ...)
    local eventFuncToFind = "on"..eventObj.eventName:sub(0, 1):upper()..eventObj.eventName:sub(2)
    -- Logger.debug(eventFuncToFind)
    if(type(_G[eventFuncToFind]) == "function")then
        _G[eventFuncToFind](...)
    end
end

function __create_luaplayer()
    local newPlayer = peachPlayer_class()
	table.insert(players, newPlayer) --Be sure that the garbage collector doesn't destory our object
	return newPlayer
end

function __create_luanpc()
    local newNPC = npcBase_class()
    table.insert(npcs, newNPC)
    return newNPC
end




