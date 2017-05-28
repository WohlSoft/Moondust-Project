class 'twitter'

function twitter:initProps()
    self.cur_normalJumpsLeft = self.def_normalJumps
    self.cur_highJumpsLeft = self.def_highJumps
end

function twitter:__init(npc_obj)
    self.npc_obj = npc_obj

    -- FOR AI_JUMPING
    self.def_normalJumps = 3
    self.def_highJumps = 1

    self.def_normalJumpsSpeedY = -3.5
    self.def_highJumpsSpeedY = -5.5

    self:initProps()
end

function twitter:onActivated()
    self:initProps()
end

function twitter:onLoop(tickTime)
    if(self.npc_obj.onGround)then
        if(self.cur_normalJumpsLeft > 0)then
            self.npc_obj.speedY = self.def_normalJumpsSpeedY
            self.cur_normalJumpsLeft = self.cur_normalJumpsLeft - 1
        elseif(self.cur_highJumpsLeft > 0)then
            self.npc_obj.speedY = self.def_highJumpsSpeedY
            self.cur_highJumpsLeft = self.cur_highJumpsLeft - 1
        else
            self.cur_normalJumpsLeft = self.def_normalJumps
            self.cur_highJumpsLeft = self.def_highJumps
        end
    end
end

return twitter

