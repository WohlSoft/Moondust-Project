class 'Koopa'

function Koopa:__init(npc_obj)
    self.npc_obj = npc_obj
end

function Koopa:onTransform(id)
    self.npc_obj.speedX = 0
    self.npc_obj.speedY = math.abs(self.npc_obj.speedY)/4
end

return Koopa

