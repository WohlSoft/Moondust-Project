class 'fireball'

function fireball:initProps()
    self:setCharacter(1)
end

function fireball:__init(npc_obj)
    self.npc_obj = npc_obj
    self.cur_character = 1
    self.def_normalJumpsSpeedY = -4.5    
    self.contacts = npc_obj:installContactDetector()
    
    self:initProps()
end

function fireball:onActivated()
    self:initProps()
end

function fireball:onLoop(tickTime)
    if(self.npc_obj.onGround)then
        self.npc_obj.speedY = self.def_normalJumpsSpeedY
    end
    if(self.contacts:detected())then
        local NPCs= self.contacts:getNPCs()
        for K,NPC in pairs(NPCs) do
            if(NPC.id~=13)then
                if(NPC.killableByFire)then
                    NPC:harm(1, NPC_DAMAGE_BY_KICK)
                    Audio.playSoundByRole(SoundRoles.PlayerKick)
                end
                self.npc_obj:kill(NPC_DAMAGE_NOREASON)
                Audio.playSoundByRole(SoundRoles.BlockHit)
                break
            end
        end
    end
end

function fireball:setCharacter(charID)
    self.ani_sequence = {charID-1, charID, charID+1, charID+2}
    self.npc_obj:setSequence(self.ani_sequence)
end


return fireball

