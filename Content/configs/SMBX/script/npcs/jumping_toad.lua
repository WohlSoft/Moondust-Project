class 'jumping_toad'

local AI_JUMPING_ANIM = 0
local AI_IDLE_ANIM = 1

function jumping_toad:initProps()
    -- Animation properties


    -- Currents
    self.cur_mode = AI_JUMPING_ANIM
    -- if physics are paused, than iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused_physics = true
    -- FOR AI_JUMPING
    self.cur_jumpingUpAnimTicks = 0
    self.cur_jumped = false

end

function jumping_toad:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config

    -- FOR AI_JUMPING
    self.def_jumpingUpAnimTicks = smbx_utils.ticksToTime(9)
    self.def_speedJumpingUp = -4.6

    npc_obj.gravity = 1
    
    self:initProps()
end

function jumping_toad:onActivated()
    self:initProps()
end

function jumping_toad:onLoop(tickTime)
    if(self.cur_mode == AI_JUMPING_ANIM)then
        if(self.def_jumpingUpAnimTicks > self.cur_jumpingUpAnimTicks)then
            if(not self.cur_jumped)then
                self.npc_obj.speedX = self.def_speedJumpingUp
                self.cur_jumped = true
            end
            self.cur_jumpingUpAnimTicks = self.cur_jumpingUpAnimTicks + tickTime
        else
            --Do animation switch
            self.cur_mode = AI_IDLE_ANIM
            self.cur_jumpingUpAnimTicks = 0
        end
    elseif(self.cur_mode == AI_IDLE_ANIM)then
        if(self.npc_obj.onGround)then
            --Do animation switch
            self.cur_mode = AI_JUMPING_ANIM
        end
    end
end

return jumping_toad
