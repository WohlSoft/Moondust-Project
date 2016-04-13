class 'glassChamber'

function glassChamber:initProps()
    self.cur_frame = self.frames-self.npc_obj.health
    self.npc_obj:setSequence({self.cur_frame})
end

function glassChamber:__init(npc_obj)
    self.npc_obj = npc_obj
    self.frames = npc_obj.health
    self:initProps()
end

function glassChamber:onActivated()
    self:initProps()
end

function glassChamber:onHarm(harmEvent)
    self.cur_frame=self.frames-(self.npc_obj.health-harmEvent.damage)
    self.npc_obj:setSequence({self.cur_frame})
end

return glassChamber

