class 'peachPlayer' (BasePlayer)

-- local i = 0

function peachPlayer.__init(self)
    BasePlayer.__init(self)
end

function peachPlayer:onLoop()
    if(Settings.isDebugInfoShown())then
        Renderer.printText("Player runs :O", 100, 240, 0, 15, 0xFFFF0055)
        Renderer.printText("Player x: "..tostring(self.x), 100, 260, 0, 15, 0xFFFF0055)
        Renderer.printText("Player y: "..tostring(self.y), 100, 280, 0, 15, 0xFFFF0055)
        -- i = i + 1
        -- if(i % 100 == 0)then
        --     self.speedY = -15
        -- end
        BasePlayer.onLoop(self)
    end    
end

function peachPlayer:onHarm(harmevent)
    local newHealth = self.health-harmevent.damage
    if(newHealth==2)then
        self:setState(2)
        Audio.playSoundByRole(SoundRoles.PlayerHarm)
    elseif(newHealth==1)then
        self:setState(1)
        Audio.playSoundByRole(SoundRoles.PlayerShrink)
    elseif(newHealth~=0)then
        Audio.playSoundByRole(SoundRoles.PlayerHarm)
    end
    BasePlayer.onHarm(self, harmevent)
end

return peachPlayer

