class 'jumping_ninji'

local AI_JUMPING_ANIM = 0
local AI_IDLE_ANIM = 1

function jumping_ninji:initProps()
    -- Animation properties
    self.animateJumpUp = {1}
    self.animateCharge = {0}

    self.npc_obj.speedX = 0

    -- Currents
    self.cur_mode = AI_IDLE_ANIM
    -- if physics are paused, than iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused_physics = false
    -- FOR AI_JUMPING
    self.cur_jumpingUpAnimTicks = 0
    self.cur_jumped = false
    self.cur_beganJump = false
    -- FOR AI_IDLE
    self.cur_idleChargingTicks = 0
    self.npc_obj:setSequence(self.animateJumpUp)
    self.cur_charged = false

end

function jumping_ninji:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config

    -- FOR AI_JUMPING
    self.def_jumpingUpAnimTicks = smbx_utils.ticksToTime(9)
    self.def_jumpingUpAnimTicksLaunch = smbx_utils.ticksToTime(8)
    self.def_speedJumpingUp = -7
    -- FOR AI_IDLE
    self.def_idleChargingTicks  = smbx_utils.ticksToTime(5)

    npc_obj.gravity = 1
    
    self:initProps()
end

function jumping_ninji:onActivated()
    self:initProps()
end

function jumping_ninji:onLoop(tickTime)
    if(self.cur_mode == AI_JUMPING_ANIM)then
        if(not self.cur_beganJump)then
            self.npc_obj:setSequence(self.animateJumpUp)
            self.cur_beganJump = true
        end
        if(not self.cur_jumped)then
            self.npc_obj.speedY = self.def_speedJumpingUp
            self.cur_jumped = true
        end
        if(self.npc_obj.onGround)then
            --Do animation switch
            self.cur_mode = AI_IDLE_ANIM
            self.cur_idleChargingTicks = 0
            self.cur_charged = false
            self.cur_beganJump = false
        end
    elseif(self.cur_mode == AI_IDLE_ANIM)then
        if(self.npc_obj.onGround)then
            if(not self.cur_charged) then
                self.npc_obj:setSequence(self.animateCharge)
                self.cur_charged = true
            end
            if(self.def_idleChargingTicks > self.cur_idleChargingTicks)then
                self.cur_idleChargingTicks = self.cur_idleChargingTicks + tickTime
            else
                --Do animation switch
                self.cur_mode = AI_JUMPING_ANIM
                self.cur_jumped = false
                self.cur_jumpingUpAnimTicks = 0
                self.npc_obj:setSequence(self.animateJumpUp)
            end
        elseif(not self.cur_beganJump)then
                self.npc_obj:setSequence(self.animateJumpUp)
                self.cur_beganJump = true
        end
    end
end

return jumping_ninji

