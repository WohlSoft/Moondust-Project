class 'thwomp'

local AI_WATCHING = 0
local AI_FALLING = 1
local AI_WAITING_ON_GROUND = 2
local AI_MOVING_UP = 3

function thwomp:initProps()
    -- Currents
    self.cur_mode = AI_WATCHING
    self.cur_wait_ticks = 0
    self.npc_obj.y = self.initial_y
    self.npc_obj.speedX = 0
    self.npc_obj.speedY = 0
end

function thwomp:__init(npc_obj)
    self.npc_obj = npc_obj

    -- Config
    self.initial_y = npc_obj.y
    self.speed_up = -2
    self.speed_fall = 6

    self.is180=(npc_obj.id==180)

    -- Detector of target player
    self.plr_detector = self.npc_obj:installInAreaDetector(-32-npc_obj.width/2, 0, 32+npc_obj.width/2, 700, {4})
    if(self.is180==true)then
    self.plr_detector_wide = self.npc_obj:installInAreaDetector(-64-npc_obj.width/2, 0, 64+npc_obj.width/2, 700, {4})
    end

    -- FOR AI_WAITING_ON_GROUND
    self.def_wait_ticks = smbx_utils.ticksToTime(100)
    npc_obj.gravity = 0

    -- Audio
    self.def_thwompSoundID = 37

    self:initProps()
end

function thwomp:onActivated()
    self:initProps()
end

function thwomp:smokeEffect()
    local effect = EffectDef();
        effect.id = 10
        effect.startX = self.npc_obj.center_x - self.npc_obj.width/4.0
        effect.startY = self.npc_obj.bottom
        effect.gravity = 0.0;
        effect.startDelay = 0.0;
        effect.maxVelX = 25.0;
        effect.maxDelay = 0;
        effect.loopsCount = 1;
        effect.framespeed = 125;
        effect.velocityX = -3.0;
        effect.velocityY = 0.0;
    Effect.runEffect(effect, true);
        effect.startX = self.npc_obj.center_x + self.npc_obj.width/4.0
        effect.velocityX = 3.0;
    Effect.runEffect(effect, true);
end

function thwomp:onLoop(tickTime)

    if(self.is180==true)then
            if(self.plr_detector_wide:detected()==true)then
                if(self.plr_detector:detected()==true)then
                    self.npc_obj:setSequence({2})
                else
                    self.npc_obj:setSequence({1})
                end
            else
                self.npc_obj:setSequence({0})
            end
    end

    if(self.cur_mode == AI_WATCHING)then
        if(self.plr_detector:detected()==true)then
            self.cur_mode=AI_FALLING
            self.npc_obj.speedX = 0
            if(self.npc_obj.speedY<self.speed_fall)then
                self.npc_obj.gravity = 2.0
                self.npc_obj:applyAccel( 0.0, 5.0 )
            else
                self.npc_obj.gravity = 0.0
                self.npc_obj.speedY = self.speed_fall
            end
        end
    elseif(self.cur_mode == AI_FALLING)then
        if(self.npc_obj.onGround)then
            self.npc_obj.gravity = 0.0
            Level.shakeScreenY(10.0, 0.05)
            Audio.playSound(self.def_thwompSoundID)
            self.npc_obj.speedY = 0
            self.cur_mode = AI_WAITING_ON_GROUND
            self:smokeEffect()
        end
    elseif(self.cur_mode == AI_WAITING_ON_GROUND)then
        if(self.def_wait_ticks > self.cur_wait_ticks)then
            self.cur_wait_ticks = self.cur_wait_ticks + tickTime
        else
            self.cur_mode = AI_MOVING_UP
            self.cur_wait_ticks = 0
            self.npc_obj.speedY = self.speed_up
        end
    elseif(self.cur_mode == AI_MOVING_UP)then
        if(self.npc_obj.y <= self.initial_y+2 )then
                self.cur_mode=AI_WATCHING
                self.npc_obj.y = self.initial_y
                self.npc_obj.speedX = 0
                self.npc_obj.speedY = 0
        end
    end
end

return thwomp

