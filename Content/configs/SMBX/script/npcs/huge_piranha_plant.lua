-------------------------------------------------------------------------------------
-- Trace of Huge Piranha Plant NPC of SMBX Engine
-------------------------------------------------------------------------------------
-- AI1:0 AI2:0 AI3:0 AI4:0 AI5:0 --INITIAL
-- AI1:71 AI2:4 AI3:0 AI4:0 AI5:0  --HIDDEN
-- AI1:74 AI2:4 AI3:0 AI4:0 AI5:0
-- AI1:0 AI2:1 AI3:0 AI4:0 AI5:0 -- SHOWING UP
-- AI1:84 AI2:1 AI3:0 AI4:0 AI5:0 
-- AI1:0 AI2:2 AI3:0 AI4:0 AI5:0 --Shown, wait something and breath air...
-- AI1:0 AI2:2 AI3:0 AI4:0 AI5:0
-- AI1:50 AI2:2 AI3:10 AI4:0 AI5:0 -- OUCH! That was painful!
-- AI1:0 AI2:3 AI3:9 AI4:0 AI5:0 -- Hidding away!
-- AI1:84 AI2:3 AI3:0 AI4:0 AI5:0
-- AI1:0 AI2:4 AI3:0 AI4:0 AI5:0  --Wait in hidden
-- AI1:74 AI2:4 AI3:0 AI4:0 AI5:0
-- AI1:0 AI2:1 AI3:0 AI4:0 AI5:0 -- Showing again!
-- AI1:84 AI2:1 AI3:0 AI4:0 AI5:0
-- AI1:0 AI2:2 AI3:0 AI4:0 AI5:0  --Shown, wait something and breath air...
-------------------------------------------------------------------------------------

class 'huge_piranha_plant'

local AI_SHOWING_UP = 0
local AI_SHOWING_IDLE = 1
local AI_HIDING_DOWN = 2
local AI_HIDING_IDLE = 3

function huge_piranha_plant:hideSprite()
    self.npc_obj:setSpriteWarp(1.0, 3, true)
end

function huge_piranha_plant:updateWarp()
    self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.top-self.npc_obj.bottom)/self.def_height), 3, true)
end


function huge_piranha_plant:initProps()
    -- Animation properties

    -- Top position (reset size to initial)
    self.npc_obj.top    = self.def_top
    self.npc_obj.bottom = self.def_bottom
    self.npc_obj.top    = self.npc_obj.bottom

    -- Currents
    self.cur_mode = AI_HIDING_IDLE
    -- if physics are paused, than iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused_physics = true
    -- FOR AI_SHOWING_UP
    self.cur_showingUpTicks = 0

    -- FOR AI_SHOWING_IDLE
    -- self.cur_showingIdleTicks = 0
    self.cur_keep_showing = true

    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0

    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = smbx_utils.ticksToTime(70) -- Initial wait time is 70 ticks!

    self:updateWarp()
end

function huge_piranha_plant:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config
    self.def_top = npc_obj.top
    self.def_height = npc_obj.height
    self.def_bottom = npc_obj.bottom
    self.speed = 1.5

    -- Detector of player
    -- self.plr_detector = self.npc_obj:installInAreaDetector(-44, -600, 44, 600, {4})

    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_SHOWING_IDLE
    -- self.def_showingIdleTicks = smbx_utils.ticksToTime(50)

    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(85)

    npc_obj.gravity = 0

    self:initProps()
end

function huge_piranha_plant:onActivated()
    self:initProps()
end

function huge_piranha_plant:onLoop(tickTime)
    if(self.cur_mode == AI_SHOWING_UP)then
        if(self.def_showingUpTicks > self.cur_showingUpTicks)then
            self.cur_showingUpTicks = self.cur_showingUpTicks + tickTime
            self.npc_obj.top = self.npc_obj.top - smbx_utils.speedConv(self.speed, tickTime)
            self:updateWarp()
        else
            self.cur_mode = AI_SHOWING_IDLE
            self.npc_obj.top = self.def_top
            self.npc_obj:resetSpriteWarp()
            self.cur_showingUpTicks = 0
            self.cur_keep_showing = true
        end
    elseif(self.cur_mode == AI_SHOWING_IDLE)then
        if(not self.cur_keep_showing)then
            self.cur_mode = AI_HIDING_DOWN
            -- self.cur_showingIdleTicks = 0
        end
    elseif(self.cur_mode == AI_HIDING_DOWN)then
        if(self.def_hidingDownTicks > self.cur_hidingDownTicks)then
            self.cur_hidingDownTicks = self.cur_hidingDownTicks + tickTime
            self.npc_obj.top = self.npc_obj.top + smbx_utils.speedConv(self.speed, tickTime)
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

function huge_piranha_plant:onHarm(harmEvent)
    self.cur_keep_showing = false
end

return huge_piranha_plant

