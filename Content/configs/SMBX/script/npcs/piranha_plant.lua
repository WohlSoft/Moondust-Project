class 'piranha_plant'

local AI_SHOWING_UP = 0
local AI_SHOWING_IDLE = 1
local AI_HIDING_DOWN = 2
local AI_HIDING_IDLE = 3

function piranha_plant:initProps()
    -- Animation properties

    -- Sound

    -- Currents
    self.cur_mode = AI_HIDING_IDLE
    -- if npc-obj paused, physics iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused = true
    -- FOR AI_SHOWING_UP
    self.cur_showingUpTicks = 0
    
    -- FOR AI_SHOWING_IDLE
    self.cur_showingIdleTicks = 0
    
    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0
    
    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = 0
end

function piranha_plant:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_top = self.npc_obj.top
    self.speed = 1
    -- Config
    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.height)
    
    -- FOR AI_SHOWING_IDLE
    self.def_showingIdleTicks = smbx_utils.ticksToTime(50)
    
    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.height)
    
    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(42)
    
    npc_obj.top = npc_obj.bottom+1
    npc_obj.gravity = 0    
    
    self:initProps()
end

function piranha_plant:onActivated()
    self:initProps()
end

function piranha_plant:onLoop(tickTime)
    local cur_npc = self.npc_obj
    if(self.cur_mode == AI_SHOWING_UP)then
        if(self.def_showingUpTicks > self.cur_showingUpTicks)then
            self.cur_showingUpTicks = self.cur_showingUpTicks + tickTime
            cur_npc.top = cur_npc.top - smbx_utils.speedConv(self.speed, tickTime)
        else
            self.cur_mode = AI_SHOWING_IDLE
            self.cur_showingUpTicks = 0
        end
    elseif(self.cur_mode == AI_SHOWING_IDLE)then
        if(self.def_showingIdleTicks >= self.cur_showingIdleTicks)then
            self.cur_showingIdleTicks = self.cur_showingIdleTicks + tickTime
        else
            self.cur_mode = AI_HIDING_DOWN
            self.cur_showingIdleTicks = 0
        end
    elseif(self.cur_mode == AI_HIDING_DOWN)then
        if(self.def_hidingDownTicks > self.cur_hidingDownTicks)then
            self.cur_hidingDownTicks = self.cur_hidingDownTicks + tickTime
            cur_npc.top = cur_npc.top + smbx_utils.speedConv(self.speed, tickTime)
        else
            self.cur_mode = AI_HIDING_IDLE
            self.cur_hidingDownTicks = 0
            cur_npc.paused=true
        end
    elseif(self.cur_mode == AI_HIDING_IDLE)then
        if(self.def_hidingIdleTicks >= self.cur_hidingIdleTicks)then
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
        else
            self.cur_mode = AI_SHOWING_UP
            cur_npc.paused=false
            self.cur_hidingIdleTicks = 0
        end
    end
end

return piranha_plant
