
local peachPlayer_class = require("player/peach")

local players = {}

local deb_i = 0
function onLoop()
    deb_i = deb_i + 1
    Renderer.printText("Ticks passed: "..deb_i, 100, 130, -1, 15, 0xFFFF0055)
end

function __native_event(eventObj, ...)
    local eventFuncToFind = "on"..eventObj.eventName:sub(0, 1):upper()..eventObj.eventName:sub(2)
    Logger.debug(eventFuncToFind)
    if(type(_G[eventFuncToFind]) == "function")then
        _G[eventFuncToFind](...)
    end
end

function __create_luaplayer()
    local newPlayer = peachPlayer_class()
	table.insert(players, newPlayer) --Be sure that the garbage collector doesn't destory our object
	return newPlayer
end




