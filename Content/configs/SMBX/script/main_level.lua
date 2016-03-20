
-- utils
smbx_utils = require("utils/smbx_utils")

-- ID's of switches
GREEN_SW  = 1
RED_SW    = 2
BLUE_SW   = 3
YELLOW_SW = 4

-- is NPC a coin?
function npc_isCoin(npcID)
    if(npcID==10)then
        return true
    elseif(npcID==33)then
        return true
    elseif(npcID==88)then
        return true
    elseif(npcID==103)then
        return true
    elseif(npcID==138)then
        return true
    elseif(npcID==152)then
        return true
    elseif((npcID>=251) and (npcID<=253))then
        return true
    elseif(npcID==258)then
        return true
    elseif(npcID==274)then
        return true
    end
    return false
end


