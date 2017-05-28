class 'jumping_toad'

local AI_JUMPING_ANIM = 0
local AI_IDLE_ANIM = 1

function jumping_toad:initProps()
    -- Animation properties
    self.animateJumpInAir = {2}
    self.animateJumpUp = {1}
    self.animateCharge = {0}

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
    self.npc_obj:setSequence(self.animateJumpInAir)
    self.cur_charged = false

end

function jumping_toad:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config

    -- FOR AI_JUMPING
    self.def_jumpingUpAnimTicks = smbx_utils.ticksToTime(9)
    self.def_jumpingUpAnimTicksLaunch = smbx_utils.ticksToTime(8)
    self.def_speedJumpingUp = -4.6
    -- FOR AI_IDLE
    self.def_idleChargingTicks  = smbx_utils.ticksToTime(7)

    npc_obj.gravity = 1
    
    self:initProps()
end

function jumping_toad:onActivated()
    self:initProps()
end

function jumping_toad:onLoop(tickTime)
    if(self.cur_mode == AI_JUMPING_ANIM)then
        if((not self.cur_beganJump) and (self.def_jumpingUpAnimTicksLaunch <= self.cur_jumpingUpAnimTicks))then
                self.npc_obj:setSequence(self.animateJumpInAir)
                self.cur_beganJump = true
        end
        if(self.def_jumpingUpAnimTicks > self.cur_jumpingUpAnimTicks)then
            if(not self.cur_jumped)then
                self.npc_obj.speedY = self.def_speedJumpingUp
                self.cur_jumped = true
            end
            self.cur_jumpingUpAnimTicks = self.cur_jumpingUpAnimTicks + tickTime
        else
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
                self.cur_beganJump = false
                self.cur_jumpingUpAnimTicks = 0
                self.npc_obj:setSequence(self.animateJumpUp)
            end
        end
    end
end

return jumping_toad
