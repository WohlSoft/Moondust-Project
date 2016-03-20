class 'marioPlayer'

function marioPlayer:__init(plr_obj)
    self.plr_obj = plr_obj
    if(self.plr_obj.stateID==1)then
        self.plr_obj.health = 1
    elseif(self.plr_obj.stateID>=2)then
        self.plr_obj.health = 2
    end
end

function marioPlayer:onLoop(tickTime)
    if(Settings.isDebugInfoShown())then
        Renderer.printText("It's me, Mario!", 100, 230, 0, 15, 0xFFFF0055)
        Renderer.printText("Player x: "..tostring(self.plr_obj.x), 100, 260, 0, 15, 0xFFFF0055)
        Renderer.printText("Player y: "..tostring(self.plr_obj.y), 100, 300, 0, 15, 0xFFFF0055)
    end
end

function marioPlayer:onHarm(harmEvent)
    local newHealth = self.plr_obj.health-harmEvent.damage
    if(newHealth==1)then
        self.plr_obj:setState(1)
        Audio.playSoundByRole(SoundRoles.PlayerShrink)
    else
        self.plr_obj:setState(2)
        Audio.playSoundByRole(SoundRoles.PlayerHarm)
    end
end

function marioPlayer:onTakeNpc(npcObj)
    if( (npcObj.id==9) and (self.plr_obj.stateID == 1 ) )then
        self.plr_obj.health = 2
        self.plr_obj:setState(2)
        Audio.playSoundByRole(SoundRoles.PlayerGrow)
    end
end

return marioPlayer

