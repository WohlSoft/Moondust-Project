class 'elevator'

function elevator:initProps()
    -- Animation properties
    --self.radius = 4*32-self.npc_obj.width/2-16
    self.direction = self.npc_obj.direction
    self.speed = 2
end

function elevator:__init(npc_obj)
    self.npc_obj = npc_obj
    self:initProps()
end

function elevator:onActivated()
    self:initProps()
end

function elevator:onLoop(tickTime)
    self.npc_obj.speedX=0
    if(self.direction<0) then
        self.npc_obj.speedY=-2
    else
        self.npc_obj.speedY=2
    end
end

return elevator
