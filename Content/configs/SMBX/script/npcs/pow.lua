class 'powBrick'

function powBrick:initProps()
    self.recent_speedY = 0
end

function powBrick:__init(npc_obj)
    self.npc_obj = npc_obj
    self.egg_color = {0}
    self.recent_speedY = 0
    
    self.def_ThwompSnd = 37

    self:initProps()
end

function powBrick:onActivated()
    self:initProps()
end

function powBrick:onLoop(TickTime)
    if ((self.npc_obj.onGround) and (self.recent_speedY > 3)) then
        self.npc_obj:kill(2);
    end
    self.recent_speedY=self.npc_obj.speedY
end

function powBrick:onKill(killEvent)
    local activeNPCs = NPC.getActive()
    for K,Goo in pairs(activeNPCs) do
        if(npc_isCoin(Goo.id))then
            Goo.gravity=1.0
            Goo.direction=0 --if set direction to 0, it will be randomely replaced with 1 or -1
            Goo.speedX=(math.random(1, 5)/10)*Goo.direction
            Goo.collideWithBlocks=true
        end
    end
    local players = Player.get()
    for K,Plr in pairs(players) do
        if(Plr.onGround)then
            Plr:bump(true, 6, 0)
            Audio.playSound(2)
        end
    end
    Level.shakeScreen(30.0, 30.0, 0.05, 0.05)
    Audio.playSound(self.def_ThwompSnd)
end

return powBrick

