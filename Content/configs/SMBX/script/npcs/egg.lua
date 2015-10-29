class 'yoshi_egg'

function yoshi_egg:initProps()

    self.recent_speedY = 0

    if(self.npc_obj.special1==98)then -- Blue Yoshi
        self.egg_color={1}
    elseif(self.npc_obj.special1==99)then -- Yellow Yoshi
        self.egg_color={2}
    elseif(self.npc_obj.special1==100)then -- Red Yoshi
        self.egg_color={3}
    elseif(self.npc_obj.special1==148)then -- Black Yoshi
        self.egg_color={4}
    elseif(self.npc_obj.special1==149)then -- Purpure Yoshi
        self.egg_color={5}
    elseif(self.npc_obj.special1==150)then -- Magenta Yoshi
        self.egg_color={6}
    elseif(self.npc_obj.special1==228)then -- Cyan Yoshi
        self.egg_color={7}
    else
        self.egg_color={0}     -- Default green
    end

    self.npc_obj:setSequence(self.egg_color)
end

function yoshi_egg:__init(npc_obj)
    self.npc_obj = npc_obj
    self.egg_color = {0}
    self.recent_speedY = 0
    
    self.def_eggSmashSoundID = 36
    self.def_eggHatchSoundID = 51

    self:initProps()
end

function yoshi_egg:onActivated()
    self:initProps()
end

function yoshi_egg:onLoop(TickTime)
    if ((self.npc_obj.onGround) and (self.recent_speedY > 3)) then
        self.npc_obj:kill(1);
        if(self.npc_obj.special1==0)then
            Audio.playSound(self.def_eggSmashSoundID)        
        else
            Audio.playSound(self.def_eggHatchSoundID)
        end
    end
    self.recent_speedY=self.npc_obj.speedY
end

return yoshi_egg

