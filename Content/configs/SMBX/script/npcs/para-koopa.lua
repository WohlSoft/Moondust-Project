class 'ParaKoopa'

-- special-option-0=Chase
-- special-option-1=Jump
-- special-option-2=Hover Left-Right
-- special-option-3=Hover Up-Down

local AI_CHASE   = 0
local AI_JUMP    = 1
local AI_HOVER_H = 2
local AI_HOVER_V = 3

function ParaKoopa:processChase(tickTime)

end

function ParaKoopa:processJump(tickTime)
    if(self.npc_obj.onGround)then
        self.npc_obj.speedY = self.def_JumpsSpeedY
    end
end

function ParaKoopa:processHoverH(tickTime)
    self.npc_obj:applyAccel( 0.0, self.accelDirect )
    self.floating_timer = self.floating_timer + tickTime
    if(self.floating_timer>=300)then
        self.accelDirect = self.accelDirect * (-1)
        self.floating_timer = self.floating_timer - 600
    end
end

function ParaKoopa:processHoverV(tickTime)

end


function ParaKoopa:initProps()
    self.def_JumpsSpeedY = -9
    self.def_ParaGoomba244_animWalkingSpeed = self.npc_obj.frameDelay
    self.def_ParaGoomba244_animWalking = {0,1}
    self.def_ParaGoomba244_animFlying = {0,2,1,3}
    self.def_ParaGoomba244_animFlyingSpeed = self.npc_obj.frameDelay/2
    if(self.npc_obj.id == 244)then
        self:setFlyingAni()
    end
    if( self.algorithm ~= AI_JUMP )then
        self.npc_obj.gravity_accel = 0
        self.accelDirect = -1
    end
    if( self.algorithm == AI_CHASE )then
        --Comming soon!
    elseif( self.algorithm == AI_HOVER_H )then
        self.npc_obj.motionSpeed = 0
        self.floating_timer = 0
    elseif( self.algorithm == AI_HOVER_V )then
        self.npc_obj.motionSpeed = 0
        self.def_flying_time = 500
        self.flying_time = 0
    end
end

function ParaKoopa:onActivated()
    self:initProps()
end

function ParaKoopa:setFlyingAni()
        self.npc_obj:setSequence(self.def_ParaGoomba244_animFlying)
        self.cur_anim = self.def_ParaGoomba244_animFlying
        self.npc_obj.frameDelay = self.def_ParaGoomba244_animFlyingSpeed
end

function ParaKoopa:setWalkingAni()
        self.npc_obj:setSequence(self.def_ParaGoomba244_animWalking)
        self.cur_anim = self.def_ParaGoomba244_animWalking
        self.npc_obj.frameDelay = self.def_ParaGoomba244_animWalkingSpeed
end


function ParaKoopa:onLoop(tickTime)
    if( self.algorithm == AI_JUMP )then
        self:processJump(tickTime)
        --Check for right Animation (Only NPC-244)
        if(self.npc_obj.id==244)then
            if(self.npc_obj.onGround)then
                if(self.cur_anim == self.def_animFlying)then
                    self:setWalkingAni()
                end
            else
                if(self.cur_anim == self.def_ParaGoomba244_animWalking)then
                    self:setFlyingAni()
                end
            end
        end
    elseif( self.algorithm == AI_CHASE )then
        self:processChase(tickTime)
    elseif( self.algorithm == AI_HOVER_H )then
        self:processHoverH(tickTime)
    elseif( self.algorithm == AI_HOVER_V )then
        self:processHoverV(tickTime)
    end
end

function ParaKoopa:onHarm(harmEvent)
    self.npc_obj.gravity_accel = self.old_gravityAccel
end

function ParaKoopa:__init(npc_obj)
    self.npc_obj = npc_obj
    self.algorithm = npc_obj.special1
    self.old_gravityAccel = npc_obj.gravity_accel
end

function ParaKoopa:onTransform(id)
    -- self.npc_obj.speedX = 0
    self.npc_obj.speedY = math.abs(self.npc_obj.speedY)/4
end

return ParaKoopa


