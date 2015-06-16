class 'peachPlayer' (BasePlayer)

-- local i = 0

function peachPlayer.__init(self)
    BasePlayer.__init(self)
end

function peachPlayer:onLoop()
    if(Settings.isDebugInfoShown())then
        Renderer.printText("Player runs :O", 100, 230, -1, 15, 0xFFFF0055)
        Renderer.printText("Player x: "..tostring(self.x), 100, 260, -1, 15, 0xFFFF0055)
        Renderer.printText("Player y: "..tostring(self.y), 100, 300, -1, 15, 0xFFFF0055)
        -- i = i + 1
        -- if(i % 100 == 0)then
        --     self.speedY = -15
        -- end
        BasePlayer.onLoop(self)
    end
    
end

return peachPlayer

