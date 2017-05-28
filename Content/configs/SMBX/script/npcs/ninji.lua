class 'ninji'

--function ninji:initProps()
--end

function ninji:__init(npc_obj)
    self.npc_obj = npc_obj
    self.block_detector = self.npc_obj:installInAreaDetector(0, -15, 32, 15, {1,3})
end

--function ninji:onActivated()
--    self:initProps()
--end

function ninji:onLoop(tickTime)
    if(self.npc_obj.onGround)then
        if((self.npc_obj.onCliff) or (self.block_detector:detected()))then
            self.npc_obj.speedY = -6
        end
    end
end

return ninji
