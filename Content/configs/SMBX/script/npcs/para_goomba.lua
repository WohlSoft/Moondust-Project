class 'para_goomba'

local AI_WALKING = 0
local AI_JUMPING = 1


function para_goomba:initProps()
    -- Animation properties
    self.cur_anim = self.def_animWalking
    
    -- Currents
    self.cur_mode = AI_WALKING

    -- FOR AI_WALKING
    self.cur_walkingTicks = 0

    self.cur_normalJumpsLeft = self.def_normalJumps
    self.cur_highJumpsLeft = self.def_highJumps

    self.npc_obj:setSequence(self.cur_anim)
end

function para_goomba:setFlyingAni()
        self.npc_obj:setSequence(self.def_animFlying)
        self.cur_anim = self.def_animFlying
        self.npc_obj.frameDelay = self.def_animFlyingSpeed
end

function para_goomba:setWalkingAni()
        self.npc_obj:setSequence(self.def_animWalking)
        self.cur_anim = self.def_animWalking
        self.npc_obj.frameDelay = self.def_animWalkingSpeed
end

function para_goomba:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config
    -- Animation sequence
    self.def_animWalkingSpeed = self.npc_obj.frameDelay
    self.def_animWalking = {0,1}
    self.def_animFlying = {0,2,1,3}
    self.def_animFlyingSpeed = self.npc_obj.frameDelay/2

    -- FOR AI_WALKING
    self.def_walkingTicks = smbx_utils.ticksToTime(32)

    -- FOR AI_JUMPING
    self.def_normalJumps = 3
    self.def_highJumps = 1

    self.def_normalJumpsSpeedY = -4
    self.def_highJumpsSpeedY = -7

    self:initProps()
end

function para_goomba:onActivated()
    self:initProps()
end

function para_goomba:onTransform(id)
    self.npc_obj.speedY = math.abs(self.npc_obj.speedY)/4
end

function para_goomba:onLoop(tickTime)
    if(self.cur_mode == AI_WALKING)then
        if(self.def_walkingTicks > self.cur_walkingTicks)then
            self.cur_walkingTicks = self.cur_walkingTicks + tickTime
        else
            self.cur_mode = AI_JUMPING
            self.cur_walkingTicks = 0
        end
    elseif(self.cur_mode == AI_JUMPING)then
        if(self.npc_obj.onGround)then
            if(self.cur_normalJumpsLeft > 0)then
                self.npc_obj.speedY = self.def_normalJumpsSpeedY
                self.cur_normalJumpsLeft = self.cur_normalJumpsLeft - 1
            elseif(self.cur_highJumpsLeft > 0)then
                self.npc_obj.speedY = self.def_highJumpsSpeedY
                self.cur_highJumpsLeft = self.cur_highJumpsLeft - 1
            else
                self.cur_normalJumpsLeft = self.def_normalJumps
                self.cur_highJumpsLeft = self.def_highJumps
                self.cur_mode = AI_WALKING
            end
        end
    end
    --Check for right Animation
    if(self.npc_obj.onGround)then
        if(self.cur_anim == self.def_animFlying)then
            self:setWalkingAni()
        end
    else
        if(self.cur_anim == self.def_animWalking)then
            self:setFlyingAni()
        end
    end
end

return para_goomba

