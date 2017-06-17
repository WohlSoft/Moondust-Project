-------------------------------------------------------------------------------------
--=---=-=====-=---=-=---=--====------=====-===-====--=====-=====-====---===--====----
--=---=-=-----==--=-=---=-=----------=------=--=---=-=-------=---=---=-=---=-=---=---
--=---=-===---=-=-=-=---=--===-------===----=--====--===-----=---====--=====-====----
---=-=--=-----=--==-=---=-----=------=------=--=---=-=-------=---=---=-=---=-=-------
----=---=====-=---=--===--===--------=-----===-=---=-=====---=---=---=-=---=-=-------
-------------------------------------------------------------------------------------
-- Trace of Venus Firetrap NPC of SMBX Engine
-------------------------------------------------------------------------------------
-- AI1:0 AI2:1 AI3:0 AI4:1 AI5:0 -- SHOWING UP
-- AI1:42 AI2:1 AI3:0 AI4:1 AI5:0
-- AI1:0 AI2:2 AI3:0 AI4:1 AI5:0 -- SHOWED IDLING
-- AI1:50 AI2:2 AI3:0 AI4:1 AI5:0 --SHOOT!
-- SHOOT!
-- AI1:51 AI2:2 AI3:0 AI4:1 AI5:0
-- AI1:99 AI2:2 AI3:0 AI4:1 AI5:0
-- AI1:0 AI2:3 AI3:0 AI4:1 AI5:0 -- HIDING DOWN
-- AI1:42 AI2:3 AI3:0 AI4:1 AI5:0
-- AI1:43 AI2:4 AI3:0 AI4:1 AI5:0 -- HIDING IDLE
-- AI1:44 AI2:4 AI3:0 AI4:1 AI5:0
-- AI1:149 AI2:4 AI3:0 AI4:1 AI5:0
-------------------------------------------------------------------------------------

class 'venus_firetrap'

local AI_SHOWING_UP = 1
local AI_SHOWING_IDLE = 2
local AI_HIDING_DOWN = 3
local AI_HIDING_IDLE = 4

function venus_firetrap:hideSprite()
    self.npc_obj:setSpriteWarp(1.0, NPC_WARP_BOTTOM, true)
end

function venus_firetrap:updateWarp()
    self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.top-self.npc_obj.bottom) / (self.def_height)), NPC_WARP_BOTTOM, true)
end

function venus_firetrap:shot()
    local bullet=self.npc_obj:spawnNPC(246, GENERATOR_APPEAR, SPAWN_UP, false)
    bullet.speedX = self.npc_obj.direction * 3.0
    bullet.center_x = self.npc_obj.abs_center_x + self.npc_obj.direction*( (self.npc_obj.width/2)-4 )
    bullet.center_y = self.npc_obj.abs_top + 24

    local pposX = self.plrpos_detector:positionX()
    local pposY = self.plrpos_detector:positionY()
    local hDirect = self.plrpos_detector:directedTo()
    local vDirect = 0
    if(pposY < self.npc_obj.abs_center_y)then
        vDirect=-1
    else
        vDirect=1
    end
    local hdist = math.abs(pposX-self.npc_obj.abs_center_x)
    local vdist = math.abs(pposY-self.npc_obj.abs_center_y)
    local angle = 0
    if(vdist~=0)then
        angle = math.atan(hdist/vdist)
    end
    -- bullet.speedX = math.sin(angle)* 3.0 * hDirect
    bullet.speedY = math.cos(angle)* 3.0 * vDirect

    if( bullet.speedY < -2.0 ) then
        bullet.speedY = -2.0
    elseif( bullet.speedY > 2.0 ) then
        bullet.speedY = 2.0
    end
end


function venus_firetrap:initProps()
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
    self.cur_showingIdleTicks = 0
    self.shooted = false

    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0

    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = smbx_utils.ticksToTime(75) -- Initial wait time is 75 ticks!

    self.curani = -1
    self:refreshAnimation();
    self:updateWarp()
end

function venus_firetrap:__init(npc_obj)
    self.npc_obj = npc_obj

    -- Config
    self.def_top = npc_obj.top
    self.def_height = npc_obj.height
    self.def_bottom = npc_obj.bottom
    self.speed = 1.5
    self.def_centerY = self.npc_obj.center_y

    self.curani = -1

    self.def_frame_lookup = {1}
    self.def_frame_lookdown = {0}

    -- Detector of player
    self.plr_detector = self.npc_obj:installInAreaDetector(-44, -600, 44, 600, {4})
    self.plrpos_detector = self.npc_obj:installPlayerPosDetector()

    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_SHOWING_IDLE
    self.def_showingIdleTicks = smbx_utils.ticksToTime(100)
    self.def_shootDelayTicks = smbx_utils.ticksToTime(50)

    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(150)
    -- If player stands over, reset time to -10 seconds
    self.def_hidingIdleTicks_waitPlayer = smbx_utils.ticksToTime(140)

    npc_obj.gravity = 0

    self:initProps()
end

function venus_firetrap:onActivated()
    self:initProps()
end

function venus_firetrap:refreshAnimation()
    if((self.plrpos_detector:positionY()<self.def_centerY) and (self.curani~=0)) then
        self.npc_obj:setSequence(self.def_frame_lookup)
        self.curani=0
    elseif((self.plrpos_detector:positionY()>self.def_centerY) and (self.curani~=1)) then
        self.npc_obj:setSequence(self.def_frame_lookdown)
        self.curani=1
    end
end

function venus_firetrap:onLoop(tickTime)

    self.npc_obj.direction = self.plrpos_detector:directedTo()
    self:refreshAnimation()

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
            self.shooted = false;
        end
    elseif(self.cur_mode == AI_SHOWING_IDLE)then
        if(self.def_showingIdleTicks > self.cur_showingIdleTicks)then
            self.cur_showingIdleTicks = self.cur_showingIdleTicks + tickTime
            if((not self.shooted) and (self.def_shootDelayTicks < self.cur_showingIdleTicks))then
                self:shot()
                self.shooted = true
            end
        else
            self.cur_mode = AI_HIDING_DOWN
            self.cur_showingIdleTicks = 0
        end
    elseif(self.cur_mode == AI_HIDING_DOWN)then
        if(self.def_hidingDownTicks > self.cur_hidingDownTicks)then
            self.cur_hidingDownTicks = self.cur_hidingDownTicks + tickTime
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
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

return venus_firetrap

