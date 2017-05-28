class 'SwingingLeaf'

function SwingingLeaf:initProps()
    self.swing_time = self.def_swing_time
    self.npc_obj.speedX = 0.0
    self.just_spawned = true
end

function SwingingLeaf:__init(npc_obj)
    self.npc_obj = npc_obj
    self.just_spawned = true
    self.def_swing_time = smbx_utils.ticksToTime(6)
    self.def_max_speed = 4
    self:initProps()
end

function SwingingLeaf:onActivated()
    self:initProps()
end

function SwingingLeaf:onLoop(TickTime)
    if(self.npc_obj.not_movable==false)then
        if(self.just_spawned)then
            if(self.npc_obj.speedY>=0)then
                self.just_spawned=false
            end
        else
            local multX = 1.0
            local multY = 1.0
            local speed_incr = (self.def_swing_time-self.swing_time)/self.def_swing_time
            local speed_decr = self.swing_time/self.def_swing_time

            if(self.swing_time>(self.def_swing_time/2))then
                    multX = self.def_max_speed * speed_incr * 2
                    multY = multX*1.2-0.5
            else
                    multX = self.def_max_speed * speed_decr * 2
                    multY = multX-2.5
            end

            self.npc_obj.speedX = -self.npc_obj.direction*multX
            self.npc_obj.speedY = speed_decr * multY

            self.swing_time = self.swing_time-(TickTime*0.17)

            if(self.swing_time<=0)then
                    self.npc_obj.direction = self.npc_obj.direction * -1
                    self.npc_obj.speedX = 0.0
                    self.npc_obj.speedY = 0.0
                    self.swing_time = self.def_swing_time
            end
        end
    else
        self.npc_obj.direction = 1
        self.npc_obj.collideWithBlocks = true
    end
end

return SwingingLeaf

