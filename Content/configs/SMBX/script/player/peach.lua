class 'peachPlayer' (BasePlayer)

function peachPlayer.__init(self)
    BasePlayer.__init(self)
end

function peachPlayer:onLoop()
	Renderer.printText("Player runs :O", 100, 230, -1, 15, 0xFFFF0055)
	Renderer.printText("Player x: "..self.x, 100, 260, -1, 15, 0xFFFF0055)
	Renderer.printText("Player y: "..self.y, 100, 300, -1, 15, 0xFFFF0055)
	BasePlayer.onLoop(self)
end

return peachPlayer