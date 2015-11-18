
-- We need to reference all placed objects with lua or the garbage collector will hit it
local __refs = {}
__refs.players = {}
__refs.npcs = {}

-- Debug stuff
local deb_i = 0
local function temp_count(t) local i = 0 for _,__ in pairs(t) do i = i + 1 end return i end
-- Debug stuff end

function onLoop()
    if(Settings.isDebugInfoShown())then
        deb_i = deb_i + 1
        Renderer.printText("Ticks passed: "..deb_i, 100, 130, 0, 15, 0xFFFF0055)
        if(npc_class_table)then
            Renderer.printText(temp_count(npc_class_table).." Npc registered!", 100, 160, 0, 15, 0xFFFF0055)	
        else
            Renderer.printText("No Npc registered!", 100, 160, 0, 15, 0xFFFF0055)
        end
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
    local newPlayer = bases.player()
	__refs.players[newPlayer] = true --Be sure that the garbage collector doesn't destory our object
    return newPlayer
end

function __create_luanpc()
    local newNPC = bases.npc()
    __refs.npcs[newNPC] = true --Be sure that the garbage collector doesn't destory our object
    return newNPC
end

function __destroy_luanpc(theNPC)
    __refs.npcs[theNPC] = nil --Let the garbage collector destory our object
end

function __destroy_luaplayer(thePlayer)
    __refs.players[thePlayer] = nil --Let the garbage collector destory our object
end




