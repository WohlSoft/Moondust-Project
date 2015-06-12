
local curX = 0
local curY = 0
local curBorderWidth = Settings.getScreenWidth() - 585
local curBorderHeight = Settings.getScreenHeight() - 20

local doDown = true
local doRight = true

function onLoop()
    if(curBorderWidth <= curX)then
        doRight = false
    end
    if(0 >= curX)then
        doRight = true
    end
    if(curBorderHeight <= curY)then
        doDown = false
    end
    if(0 >= curY)then
        doDown = true
    end
    
    if(doDown)then
        curY = curY + 1
    else
        curY = curY - 1
    end
    
    if(doRight)then
        curX = curX + 1
    else
        curX = curX - 1
    end
    Renderer.printText("Engine by Wohlstand & Kevsoft", curX, curY, 1, 15, 0xFFFF0077)
end


function __native_event(eventObj, ...)
    local eventFuncToFind = "on"..eventObj.eventName:sub(0, 1):upper()..eventObj.eventName:sub(2)
    Logger.debug(eventFuncToFind)
    if(type(_G[eventFuncToFind]) == "function")then
        _G[eventFuncToFind](...)
    end
end


