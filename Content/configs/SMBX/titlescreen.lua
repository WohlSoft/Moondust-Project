
local function dbg_func4()
    error("Sample Error", 2)
end

local function dbg_func3()
    dbg_func4()
end

local function dbg_func2()
    dbg_func3()
end

local function dbg_func1()
    dbg_func2()
end

function __native_event(eventObj, ...)
    local args = {...}
    local i = 0
    Logger.debug("Event called "..eventObj.eventName)
    Logger.debug("Throwing test error!")
    dbg_func1()
end


