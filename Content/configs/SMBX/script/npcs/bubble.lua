class 'bubble'

function bubble:__init(npc_obj)
    self.npc_obj = npc_obj
    self.contacts = npc_obj:installContactDetector()
end

function bubble:dropContents()
    local thrownNPC=self.npc_obj:spawnNPC(self.npc_obj.contents, GENERATOR_APPEAR, SPAWN_UP, false)
    if(thrownNPC~=nil) then
        thrownNPC.speedX = 0
        thrownNPC.speedY = 0
        thrownNPC.center_x = self.npc_obj.center_x
        thrownNPC.center_y = self.npc_obj.center_y
        thrownNPC.special1 = 0
    end
end

function bubble:onLoop(tickTime)

    if(self.contacts:detected())then
        local blocks= self.contacts:getBlocks()
        for K,Blk in pairs(blocks) do
            if(Blk.isSolid)then
                self.npc_obj:kill(0)
                return
            end
        end
        local npcs= self.contacts:getNPCs()
        for K,Blk in pairs(npcs) do
            self.npc_obj:kill(0)
            return
        end
        local players= self.contacts:getPlayers()
        for K,Blk in pairs(players) do
            self.npc_obj:kill(0)
            return
        end
    end
end

function bubble:onKill(killEvent)
    self:dropContents()
end

return bubble

