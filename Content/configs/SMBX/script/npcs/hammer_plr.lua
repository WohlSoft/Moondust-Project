class 'HammerPlr'

-- function HammerPlr:initProps()
-- end

function HammerPlr:__init(npc_obj)
    self.npc_obj = npc_obj
    self.contacts = npc_obj:installContactDetector()
    self.block_detector = self.npc_obj:installInAreaDetector(-(npc_obj.width/2), -(npc_obj.height/2), npc_obj.width/2, npc_obj.height/2, {1})
end

function HammerPlr:onActivated()
    -- self:initProps()
end

function HammerPlr:onLoop(tickTime)
    if(self.block_detector:detected())then
        local Blocks= self.block_detector:getBlocks()
        for K,BLOCK in pairs(Blocks) do
            if(BLOCK.id==457)then
                BLOCK:remove(true)
            end
        end
    end
    if(self.contacts:detected())then
        local Blocks= self.contacts:getBlocks()
        for K,BLOCK in pairs(Blocks) do
            if(BLOCK.id==457)then
                BLOCK:remove()
            end
        end

        local NPCs= self.contacts:getNPCs()
        for K,NPC in pairs(NPCs) do
            if( (NPC.id~=171) and (npc_isCoin(NPC.id)==false) )then
                if(NPC.kickedByHammer ~= nil)then
                    NPC.kickedByHammer = NPC.kickedByHammer-tickTime
                    if( NPC.kickedByHammer < 0 )then
                        NPC.kickedByHammer = nil
                    end
                end
                if( NPC.killableByHammer and (NPC.kickedByHammer == nil) )then
                    NPC:harm(1, NPC_DAMAGE_BY_KICK)
                    NPC.kickedByHammer = 100
                    Audio.playSoundByRole(SoundRoles.PlayerKick)
                end
                break
            end
        end
    end
end

return HammerPlr

