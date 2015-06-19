local smbx_utils = {}

local SMBXTickTime = 1000.0/65.0

function smbx_utils.ticksToTime(ticks)
    return ticks*SMBXTickTime
end

return smbx_utils