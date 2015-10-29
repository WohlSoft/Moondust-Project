class 'rinka_shooter'

function rinka_shooter:shoot()
    local bullet=self.npc_obj:spawnNPC(210, GENERATOR_APPEAR, SPAWN_UP, false)
    bullet.center_x = self.npc_obj.center_x
    bullet.center_y = self.npc_obj.center_y
end


function rinka_shooter:initProps()
    -- FOR AI_SHOWING_IDLE
    self.cur_shootDelay = 0
end

function rinka_shooter:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_shootDelay = smbx_utils.ticksToTime(230)
    self:initProps()
end

function rinka_shooter:onActivated()
    self:initProps()
end

function rinka_shooter:onLoop(tickTime)
        self.cur_shootDelay = self.cur_shootDelay + tickTime
        if (self.def_shootDelay <= self.cur_shootDelay) then
            self:shoot()
            self.cur_shootDelay = 0
        end
end

return rinka_shooter

