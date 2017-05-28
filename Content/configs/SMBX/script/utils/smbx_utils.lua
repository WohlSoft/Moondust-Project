local smbx_utils = {}

local SMBXFrameSpeed = 65.0
--1000.0 / SMBXFrameSpeed
local SMBXTickTime = 15.285

function smbx_utils.ticksToTime(ticks)
    return ticks * SMBXTickTime
end

function smbx_utils.speedConv(speed, timeTicks)
        return speed * (timeTicks/SMBXTickTime)
end

return smbx_utils

