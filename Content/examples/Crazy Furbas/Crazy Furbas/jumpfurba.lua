class 'jumpfurba'

function jumpfurba:initProps()
    -- Animation properties
    self.animateOne = {0}
    self.animateTwo = {1}
end

function jumpfurba:__init(npc_obj)
    self.npc_obj = npc_obj
    self.cur_frame = false
    self.touchGround = false
    self.def_normalJumpsSpeedY = -1.0 * math.random(3.0, 11.0)
    self:initProps()
end

function jumpfurba:onActivated()
    self:initProps()
end

function jumpfurba:onLoop(tickTime)
    if(self.npc_obj.onGround and not self.touchGround) then
            self.npc_obj.speedY = self.def_normalJumpsSpeedY
            if(self.cur_frame)then
                self.npc_obj:setSequence(self.animateOne)
            else
                self.npc_obj:setSequence(self.animateTwo)
            end
            self.cur_frame = not self.cur_frame
            self.touchGround = true
            self.def_normalJumpsSpeedY = -1.0 * math.random(3.0, 11.0)
    elseif not self.npc_obj.onGround then
        self.touchGround = false
    end
end

return jumpfurba

