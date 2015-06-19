local smbx_utils = {}

local SMBXFrameSpeed = 65.0
local SMBXTickTime = 1000.0 / SMBXFrameSpeed

function smbx_utils.ticksToTime(ticks)
    return ticks * SMBXTickTime
end



return smbx_utils