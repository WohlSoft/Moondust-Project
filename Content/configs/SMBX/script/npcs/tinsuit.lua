class 'tinsuit'

function tinsuit:initProps()
    -- Animation properties
    self.animateOne = {0}
    self.animateTwo = {1}
end

function tinsuit:__init(npc_obj)
    self.npc_obj = npc_obj
    self.cur_frame = false
    self.def_normalJumpsSpeedY = -3.5
    self.touchGround = false
    self:initProps()
end

function tinsuit:onActivated()
    self:initProps()
end

function tinsuit:onLoop(tickTime)
    if(self.npc_obj.onGround and not self.touchGround) then
            self.npc_obj.speedY = self.def_normalJumpsSpeedY
            if(self.cur_frame)then
                self.npc_obj:setSequence(self.animateOne)
            else
                self.npc_obj:setSequence(self.animateTwo)
            end
            self.cur_frame = not self.cur_frame
            self.touchGround = true
    elseif not self.npc_obj.onGround then
        self.touchGround = false
    end
end

return tinsuit

