
local deb_i = 0

function onLoop()
    if(Settings.isDebugInfoShown())then
        deb_i = deb_i + 1
        Renderer.printText("Ticks passed: "..deb_i, 100, 130, 0, 15, 0xFFFF0055)
    end
end


function __native_event(eventObj, ...)
    local eventFuncToFind = "on"..eventObj.eventName:sub(0, 1):upper()..eventObj.eventName:sub(2)
    -- Logger.debug(eventFuncToFind)
    if(type(_G[eventFuncToFind]) == "function")then
        _G[eventFuncToFind](...)
    end
end


