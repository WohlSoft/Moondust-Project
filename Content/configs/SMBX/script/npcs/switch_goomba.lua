class 'SwitchGoomba'

local GREEN_ID = 63
local RED_ID = 65
local BLUE_ID = 61
local YELLOW_ID = 59

local left_red = 0
local left_green = 0
local left_blue = 0
local left_yellow = 0

function SwitchGoomba:__init(npc_obj)
    self.npc_obj = npc_obj
    if(self.npc_obj.id==GREEN_ID)then
        left_green = left_green + 1
    elseif(self.npc_obj.id==RED_ID)then
        left_red = left_red + 1
    elseif(self.npc_obj.id==BLUE_ID)then
        left_blue = left_blue + 1
    elseif(self.npc_obj.id==YELLOW_ID)then
        left_yellow = left_yellow + 1
    end
end

function SwitchGoomba:onKill(killEvent)
    if(self.npc_obj.id==GREEN_ID)then
        left_green = left_green - 1
        if(left_green<=0)then
            Level.toggleSwitch(GREEN_SW)
        end
    elseif(self.npc_obj.id==RED_ID)then
        left_red = left_red - 1
        if(left_red<=0)then
            Level.toggleSwitch(RED_SW)
        end
    elseif(self.npc_obj.id==BLUE_ID)then
        left_blue = left_blue - 1
        if(left_blue<=0)then
            Level.toggleSwitch(BLUE_SW)
        end
    elseif(self.npc_obj.id==YELLOW_ID)then
        left_yellow = left_yellow - 1
        if(left_yellow<=0)then
            Level.toggleSwitch(YELLOW_SW)
        end
    end
end

return SwitchGoomba

