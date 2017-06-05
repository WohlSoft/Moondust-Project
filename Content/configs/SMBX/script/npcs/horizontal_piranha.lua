class 'horizontal_piranha'

local AI_SHOWING_UP = 0
local AI_SHOWING_IDLE = 1
local AI_HIDING_DOWN = 2
local AI_HIDING_IDLE = 3

function horizontal_piranha:hideSprite()
    if (self.npc_obj.direction == 1) then
        self.npc_obj:setSpriteWarp(1.0, 2, true)
    elseif (self.npc_obj.direction == -1) then
        self.npc_obj:setSpriteWarp(1.0, 4, true)
    end
end

function horizontal_piranha:updateWarp()
    if (self.npc_obj.direction == 1) then
        self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.left-self.npc_obj.right)/self.def_width), 2, true)
    elseif (self.npc_obj.direction == -1) then
        self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.left-self.npc_obj.right)/self.def_width), 4, true)
    end
end


function horizontal_piranha:initProps()
    -- Animation properties

    -- right-left position (reset size to initial)
    self.npc_obj.left     = self.def_left
    self.npc_obj.right    = self.def_right
    if (self.npc_obj.direction == 1) then
        self.npc_obj.right = self.npc_obj.left
    elseif (self.npc_obj.direction == -1) then
        self.npc_obj.left = self.npc_obj.right
    end

    -- Currents
    self.cur_mode = AI_HIDING_IDLE
    -- if physics are paused, than iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused_physics = true
    -- FOR AI_SHOWING_UP
    self.cur_showingUpTicks = 0

    -- FOR AI_SHOWING_IDLE
    self.cur_showingIdleTicks = 0

    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0

    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = smbx_utils.ticksToTime(70)

    self:updateWarp()
end

function horizontal_piranha:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config
    self.def_right = npc_obj.right
    self.def_left = npc_obj.left
    self.def_width = npc_obj.width
    self.speed = 1.5
    
    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.width/self.speed)

    -- FOR AI_SHOWING_IDLE
    self.def_showingIdleTicks = smbx_utils.ticksToTime(50)

    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.width/self.speed)

    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(75)
    -- If player stands over, reset time to -10 seconds
    -- self.def_hidingIdleTicks_waitPlayer = smbx_utils.ticksToTime(65)

    npc_obj.gravity = 0

    -- self:initProps()
end

function horizontal_piranha:onActivated()
    self:initProps()
end

function horizontal_piranha:onLoop(tickTime)
    if(self.cur_mode == AI_SHOWING_UP)then
        if(self.def_showingUpTicks > self.cur_showingUpTicks)then
            self.cur_showingUpTicks = self.cur_showingUpTicks + tickTime
            if (self.npc_obj.direction == 1) then
                self.npc_obj.right = self.npc_obj.right + smbx_utils.speedConv(self.speed, tickTime)
            elseif (self.npc_obj.direction == -1) then
                self.npc_obj.left = self.npc_obj.left - smbx_utils.speedConv(self.speed, tickTime)
            end
            self:updateWarp()
        else
            self.cur_mode = AI_SHOWING_IDLE
            if (self.npc_obj.direction == 1) then
                self.npc_obj.right = self.def_right
            elseif (self.npc_obj.direction == -1) then
                self.npc_obj.left = self.def_left
            end
            self.npc_obj:resetSpriteWarp()
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
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
            if (self.npc_obj.direction == 1) then
                self.npc_obj.right = self.npc_obj.right - smbx_utils.speedConv(self.speed, tickTime)
            elseif (self.npc_obj.direction == -1) then
                self.npc_obj.left = self.npc_obj.left + smbx_utils.speedConv(self.speed, tickTime)
            end
            self:updateWarp()
        else
            self.cur_mode = AI_HIDING_IDLE
            self:hideSprite()
            self.cur_hidingDownTicks = 0
            self.npc_obj.paused_physics=true
        end
    elseif(self.cur_mode == AI_HIDING_IDLE)then
        if(self.def_hidingIdleTicks >= self.cur_hidingIdleTicks)then
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
        else
            self.cur_mode = AI_SHOWING_UP
            self.npc_obj.paused_physics = false
            self.cur_hidingIdleTicks = 0
        end
    end
end

return horizontal_piranha

