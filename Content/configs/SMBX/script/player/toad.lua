class 'toadPlayer'

function toadPlayer:__init(plr_obj)
    self.plr_obj = plr_obj
    if(self.plr_obj.stateID==1)then
        self.plr_obj.health = 1
    elseif(self.plr_obj.stateID==2)then
        self.plr_obj.health = 2
    elseif(self.plr_obj.stateID>=3)then
        self.plr_obj.health = 3
    end
end

function toadPlayer:onLoop(tickTime)
    if(Settings.isDebugInfoShown())then
        Renderer.printText("Hii! =)", 100, 230, 0, 15, 0xFFFF0055)
        Renderer.printText("Player x: "..tostring(self.plr_obj.x), 100, 260, 0, 15, 0xFFFF0055)
        Renderer.printText("Player y: "..tostring(self.plr_obj.y), 100, 300, 0, 15, 0xFFFF0055)
    end
    if((self.plr_obj.stateID==4) or (self.plr_obj.stateID==5))then
        if((not self.plr_obj.onGround) and self.plr_obj:getKeyState(KEY_JUMP) )then
            if(self.plr_obj.speedY>=3)then
                self.plr_obj.speedY=3
                self.plr_obj:setAnimation(15, 128)
            end
        end
    end
end

function toadPlayer:onHarm(harmEvent)
    processPlayerHarm(self.plr_obj, harmEvent)
end

function toadPlayer:onTakeNpc(npcObj)
    ProcessPlayerPowerUP(self.plr_obj, npcObj)
end

function toadPlayer:onKeyPressed(keyType)
    if( (self.plr_obj.stateID==3) and (keyType==KEY_RUN) and (not self.plr_obj.isDucking) )then
        self.plr_obj:playAnimationOnce(7, 128, true, false, 1)
        ShootFireball(self.plr_obj)
    end
    if( (self.plr_obj.stateID==6) and (keyType==KEY_RUN) and (not self.plr_obj.isDucking) )then
        self.plr_obj:playAnimationOnce(7, 128, true, false, 1)
        ShootHammer(self.plr_obj)
    end
end

return toadPlayer

