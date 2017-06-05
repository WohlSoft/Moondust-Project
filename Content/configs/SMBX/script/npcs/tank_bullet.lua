class 'TankBullet'

function TankBullet:__init(npc_obj)
    self.npc_obj = npc_obj
end

function TankBullet:onActivated()
    Audio.playSound(22)
end

return TankBullet

