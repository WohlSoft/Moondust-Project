class 'fire_statue'

function fire_statue:shot()
    local bullet=self.npc_obj:spawnNPC(85, GENERATOR_APPEAR, SPAWN_UP, false)
    bullet.speedX = self.npc_obj.direction * 3
    bullet.speedY = 0
    bullet.center_y = self.npc_obj.top+20
    bullet.center_x = self.npc_obj.center_x+self.npc_obj.direction*((self.npc_obj.width/2)+8)
    Audio.playSound(42)
end


function fire_statue:initProps()
    -- FOR AI_SHOWING_IDLE
    self.cur_shootDelay = 0
end

function fire_statue:__init(npc_obj)
    self.npc_obj = npc_obj

    if(self.npc_obj.id==181)then
        self.def_shootDelay = smbx_utils.ticksToTime(220)
    else
        self.def_shootDelay = smbx_utils.ticksToTime(230)
    end
    self:initProps()
end

function fire_statue:onActivated()
    self:initProps()
end

function fire_statue:onLoop(tickTime)
        self.cur_shootDelay = self.cur_shootDelay + tickTime
        if (self.def_shootDelay <= self.cur_shootDelay) then
            self:shot()
            self.cur_shootDelay = 0
        end
end

return fire_statue

