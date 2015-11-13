class 'cannon'

function cannon:shot()
    local bullet=self.npc_obj:spawnNPC(17, GENERATOR_APPEAR, SPAWN_UP, false)
    bullet.speedX = self.npc_obj.direction * 3
    bullet.speedY = 0
    bullet.direction = self.plr_pos_detector:directedTo()
    bullet.center_y = self.npc_obj.center_y
    bullet.center_x = self.npc_obj.center_x + self.plr_pos_detector:directedTo()*((self.npc_obj.width/2))
    Effect.runStaticEffectCentered(10, bullet.center_x, bullet.center_y)
end


function cannon:initProps()
    -- FOR AI_SHOWING_IDLE
    self.cur_shootDelay = 0
end

function cannon:__init(npc_obj)
    self.npc_obj = npc_obj

    self.def_shootDelay = smbx_utils.ticksToTime(200)

    -- Detector of player
    self.plr_detector = self.npc_obj:installInAreaDetector(-44, -600, 44, 600, {4})
    self.plr_pos_detector = self.npc_obj:installPlayerPosDetector()

    self:initProps()
end

function cannon:onActivated()
    self:initProps()
end

function cannon:onLoop(tickTime)
        self.cur_shootDelay = self.cur_shootDelay + tickTime
        if (self.def_shootDelay <= self.cur_shootDelay) then
            if(self.plr_detector:detected()==false)then
                self:shot()
            end
            self.cur_shootDelay = 0
        end
end

return cannon

