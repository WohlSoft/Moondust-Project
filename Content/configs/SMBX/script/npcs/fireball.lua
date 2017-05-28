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
        local Blocks= self.contacts:getBlocks()
        for K,BLOCK in pairs(Blocks) do
            if(BLOCK.collide_npc == PhysBase.COLLISION_ANY)then
                if(BLOCK.id==620)then
                    BLOCK:transformTo(10, 1)
                    self.npc_obj:kill(NPC_DAMAGE_NOREASON)
                    Audio.playSoundByRole(SoundRoles.BlockHit)
                elseif(BLOCK.id==621)then
                    BLOCK:transformTo(109, 2)
                    BLOCK.slippery = false
                    self.npc_obj:kill(NPC_DAMAGE_NOREASON)
                    Audio.playSoundByRole(SoundRoles.BlockHit)
                elseif( (BLOCK.bottom <= self.npc_obj.top) or
                         self.npc_obj.blockedAtLeft or
                         self.npc_obj.blockedAtRight )then
                    self.npc_obj:kill(NPC_DAMAGE_NOREASON)
                    Audio.playSoundByRole(SoundRoles.BlockHit)
                end
            end
        end

        local NPCs= self.contacts:getNPCs()
        for K,NPC in pairs(NPCs) do
            if( (NPC.id~=13) and (npc_isCoin(NPC.id)==false) )then
                if(NPC.killableByFire)then
                    NPC:harm(1, NPC_DAMAGE_BY_KICK, self.npc_obj)
                    Audio.playSoundByRole(SoundRoles.PlayerKick)
                end
                self.npc_obj:kill(NPC_DAMAGE_NOREASON, nil)
                Audio.playSoundByRole(SoundRoles.BlockHit)
                break
            end
        end
    end
end

function fireball:setCharacter(charID)
    local framePos = (charID-1)*4
    self.ani_sequence = {framePos, framePos+1, framePos+2, framePos+3}
    self.npc_obj:setSequence(self.ani_sequence)
end

return fireball

