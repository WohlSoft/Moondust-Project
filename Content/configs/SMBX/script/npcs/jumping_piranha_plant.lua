-------------------------------------------------------------------------------------
-- Trace of Jumping Piranha Plant NPC of SMBX Engine
-------------------------------------------------------------------------------------
-- AI1:0 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:0 -- INITIAL
-- AI1:0 AI2:-1 AI3:0 AI4:0 AI5:0 Speed-Y:0 -- Waiting hidden
-- AI1:0 AI2:-29 AI3:0 AI4:0 AI5:0 Speed-Y:0
-- AI1:1 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:0 -- Going UP! (initial speed = 6)
-- AI1:1 AI2:1 AI3:0 AI4:0 AI5:0 Speed-Y:-6
-- AI1:1 AI2:2 AI3:0 AI4:0 AI5:0 Speed-Y:-5.8
-- AI1:1 AI2:3 AI3:0 AI4:0 AI5:0 Speed-Y:-5.6
-- AI1:1 AI2:96 AI3:0 AI4:0 AI5:0 Speed-Y:-0.019999999999996
-- AI1:2 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:3.5041414214732e-15 -- Falling down
-- AI1:2 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:0.010000000000004
-- AI1:2 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:0.75
-- AI1:2 AI2:0 AI3:0 AI4:0 AI5:0 Speed-Y:0.75 -- Fell down!
-- AI1:0 AI2:60 AI3:0 AI4:0 AI5:0 Speed-Y:0.75
-- AI1:0 AI2:59 AI3:0 AI4:0 AI5:0 Speed-Y:0.75

-------------------------------------------------------------------------------------

class 'jumping_piranha_plant'

local AI_SHOWING_UP = 0
local AI_FLOATING_UPDOWN = 1
local AI_HIDING_DOWN = 2
local AI_HIDING_IDLE = 3

function jumping_piranha_plant:hideSprite()
        self.npc_obj:setSpriteWarp(1.0, 3, true)
end

function jumping_piranha_plant:updateWarp()
        self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.top-self.npc_obj.bottom)/self.def_height), 3, true)
end


function jumping_piranha_plant:initProps()
    self.npc_obj:setBodyType(false, true); --set sticky dynamic NPC
    -- Animation properties
    -- Top position (reset size to initial)
    self.npc_obj.y      = self.def_y
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
    self.cur_showingIdleTicks = 0

    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0

    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = smbx_utils.ticksToTime(70) -- Initial wait time is 70 ticks!

    self:updateWarp()
end

function jumping_piranha_plant:__init(npc_obj)
    self.npc_obj = npc_obj

    -- Config
    self.def_y   = npc_obj.y
    self.def_top = npc_obj.top
    self.def_height = npc_obj.height
    self.def_bottom = npc_obj.bottom
    self.speedUP = 6.0
    self.speedDOWN = 0.65

    -- Detector of player
    self.plr_detector = self.npc_obj:installInAreaDetector(-44, -600, 44, 600, {4})

    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speedUP)

    -- FOR AI_SHOWING_IDLE
    self.def_showingIdleTicks = smbx_utils.ticksToTime(50)

    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speedDOWN)

    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(90)
    -- If player stands over, reset time to -10 seconds
    self.def_hidingIdleTicks_waitPlayer = smbx_utils.ticksToTime(65)

    npc_obj.gravity = 0

    self:initProps()
end

function jumping_piranha_plant:onActivated()
    self:initProps()
end

function jumping_piranha_plant:onLoop(tickTime)
    if(self.cur_mode == AI_SHOWING_UP)then
        if(self.def_showingUpTicks > self.cur_showingUpTicks)then
            self.cur_showingUpTicks = self.cur_showingUpTicks + tickTime
            self.npc_obj.top = self.npc_obj.top - smbx_utils.speedConv(self.speedUP, tickTime)
            self:updateWarp()
        else
            self.cur_mode = AI_FLOATING_UPDOWN
            self.npc_obj.top = self.def_top
            self.npc_obj:resetSpriteWarp()
            self.cur_showingUpTicks = 0
            self.npc_obj.speedY = -6.0
        end
    elseif(self.cur_mode == AI_FLOATING_UPDOWN)then
        if( self.npc_obj.y < self.def_y )then
            if(self.npc_obj.speedY > self.speedDOWN )then
                self.npc_obj.speedY = self.speedDOWN
            elseif(self.npc_obj.speedY < self.speedDOWN )then
                if(self.npc_obj.speedY < -4.0 )then
                    self.npc_obj.speedY = self.npc_obj.speedY + smbx_utils.speedConv(0.25, tickTime)
                elseif(self.npc_obj.speedY < -1.0 )then
                    self.npc_obj.speedY = self.npc_obj.speedY + smbx_utils.speedConv(0.125, tickTime)
                else
                    self.npc_obj.speedY = self.npc_obj.speedY + smbx_utils.speedConv(0.0255, tickTime)
                end
            end
        else
            self.cur_mode = AI_HIDING_DOWN
            self.cur_showingIdleTicks = 0
            self.npc_obj.speedY = 0
        end
    elseif(self.cur_mode == AI_HIDING_DOWN)then
        if(self.def_hidingDownTicks > self.cur_hidingDownTicks)then
            self.cur_hidingDownTicks = self.cur_hidingDownTicks + tickTime
            -- self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
            self.npc_obj.top = self.npc_obj.top + smbx_utils.speedConv(self.speedDOWN, tickTime)
            self:updateWarp()
        else
            self.cur_mode = AI_HIDING_IDLE
            self:hideSprite()
            self.cur_hidingDownTicks = 0
            self.npc_obj.y = self.def_bottom -- Avoid unnecessary self-going down
            self.npc_obj.paused_physics=true
        end
    elseif(self.cur_mode == AI_HIDING_IDLE)then
        if(self.def_hidingIdleTicks >= self.cur_hidingIdleTicks)then
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
        else
            if(self.plr_detector:detected()==false)then
                self.cur_mode = AI_SHOWING_UP
                self.npc_obj.paused_physics = false
                self.cur_hidingIdleTicks = 0
            else
                -- If player staying over, also wait 10 seconds!
                self.cur_hidingIdleTicks = self.def_hidingIdleTicks_waitPlayer
            end
        end
    end
end

return jumping_piranha_plant

