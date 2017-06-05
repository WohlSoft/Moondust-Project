class 'Springboard'

function Springboard:playSeq(tickTime)
    if(self.sequ_is_idle)then
        return
    end

    self.ani_passed = self.ani_passed+tickTime
    if(self.ani_passed>self.ani_delay)then
        self.ani_passed = self.ani_passed-self.ani_delay
        if(self.is_forward)then
            self.ani_frame = self.ani_frame+1
            if(self.ani_frame==2)then
                self.is_forward=false
            end
        else
            self.ani_frame = self.ani_frame-1
            if(self.ani_frame==0)then
                self.sequ_is_idle=true
            end
        end
        self.npc_obj:setSequence({self.ani_frame})        
    end    
end

function Springboard:__init(npc_obj)
    self.npc_obj = npc_obj
    self.contacts = npc_obj:installContactDetector()
    self.spring_vel = 9.3
    self.spring_time = 575
    self.npc_obj:setSequence({0})

    self.sequ_is_idle=true
    self.is_forward = true
    self.ani_passed = 0
    self.ani_delay = 24
    self.ani_frame = 0
end

function Springboard:spring(player)
    Audio.playSound(24)
    player:bump(true, self.spring_vel, self.spring_time)
end

function Springboard:startAni(speed)
    if(self.sequ_is_idle)then
        self.ani_delay=math.abs(speed*1.5)
        self.is_forward = true
        self.sequ_is_idle = false
        self.ani_frame = 1
        self.ani_passed = 0
        self.npc_obj:setSequence({self.ani_frame})
    end
end

function Springboard:onLoop(tickTime)

    self:playSeq(tickTime)

    if(self.contacts:detected())then
        local players= self.contacts:getPlayers()
        for K,Plr in pairs(players) do
            if(Plr.speedY>0 and (Plr.bottom > self.npc_obj.top+4) and (Plr.bottom < self.npc_obj.top+8-(Plr.speedY/2)))then
                self:startAni(Plr.speedY)
            elseif(Plr.speedY>0 and (Plr.bottom > self.npc_obj.top+8-(Plr.speedY/2)) and (Plr.bottom < self.npc_obj.bottom) )then
                self:startAni(Plr.speedY)
                self:spring(Plr)
            end
        end
    end
end

return Springboard

