class 'shell'

local AI_IDLING = 0
local AI_RUNNING = 1

--self.npc_obj:setSequence({0})
--self.npc_obj:setSequence({0,1,2,3})

function shell:initProps()
    -- Currents
    self.cur_mode=AI_IDLING
    self.npc_obj.motionSpeed = 0
    self.npc_obj.not_movable = self.wasNotMovable
    if(self.npc_obj.spawnedGenType == BaseNPC.SPAWN_APPEAR) then
        self.npc_obj.speedX = 0
        self.npc_obj.speedY = 0
    end
    self.npc_obj:setSequence({0})
    if(self.npc_obj.spawnedGenType == BaseNPC.SPAWN_PROJECTILE) then
            if self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_LEFT    then self.npc_obj.direction = -1; self:setRunning()
        elseif self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_RIGHT   then self.npc_obj.direction = 1;  self:setRunning()
        end
    end
end

function shell:__init(npc_obj)
    self.npc_obj = npc_obj
    self.wasNotMovable = npc_obj.not_movable
    self.contacts = npc_obj:installContactDetector()
    self:initProps()
end

function shell:onActivated()
    self:initProps()
end

function shell:onLoop(tickTime)
    if(self.cur_mode == AI_RUNNING)then
        if(self.contacts:detected())then
            local Blocks= self.contacts:getBlocks()
            for K,Blk in pairs(Blocks) do
                if(Blk.collide_npc == PhysBase.COLLISION_ANY and (self.npc_obj.blockedAtLeft or self.npc_obj.blockedAtRight) )then
                    if (Blk.top+2 < self.npc_obj.bottom) and (Blk.bottom-2 > self.npc_obj.top) then
                        Blk:hit(2)
                    end
                end
            end
            local NPCs= self.contacts:getNPCs()
            for K,Npc in pairs(NPCs) do
                if(npc_isShell(Npc.id))then
                    if(Npc.controller.cur_mode == AI_RUNNING)then
                        self.npc_obj:kill(NPC_DAMAGE_BY_KICK)
                    end
                end
                if(
                    not npc_isBloeGoopa(Npc.id)
                    and not npc_isCoin(Npc.id)
                    and not npc_isRadish(Npc.id)
                    and not npc_isLife(Npc.id)
                  )then
                    Npc:kill(NPC_DAMAGE_BY_KICK)
                end
            end
        end
    end
end

function shell:onHarm(harmEvent)
    if( (harmEvent.reason_code ~= BaseNPC.DAMAGE_LAVABURN) and (harmEvent.reason_code ~= BaseNPC.DAMAGE_BY_KICK) )then
        harmEvent.cancel=true
        harmEvent.damage=0
        if(harmEvent.killed_by == NpcHarmEvent.player)then
            self.npc_obj.direction = harmEvent.killer_p.direction
        end
        self:toggleState()
    end
end

function shell:onKill(killEvent)
    if( (killEvent.reason_code ~= BaseNPC.DAMAGE_LAVABURN) and (killEvent.reason_code ~= BaseNPC.DAMAGE_BY_KICK) )then
        killEvent.cancel=true
        if(killEvent.killed_by == NpcKillEvent.player)then
            self.npc_obj.direction = killEvent.killer_p.direction
        end
        self:toggleState()
    end
end

function shell:setRunning()
    self.cur_mode=AI_RUNNING
    self.npc_obj.motionSpeed = 7.1
    self.npc_obj.frameDelay = 32
    self.npc_obj.not_movable = false
    self.npc_obj:setSequence({0,1,2,3})
end

function shell:setIdling()
    self.cur_mode=AI_IDLING
    self.npc_obj.motionSpeed = 0
    self.npc_obj.speedX = 0
    self.npc_obj.not_movable = self.wasNotMovable
    self.npc_obj:setSequence({0})
end

function shell:toggleState()
    if(self.cur_mode == AI_IDLING)then
        self:setRunning()
        Audio.playSoundByRole(SoundRoles.PlayerKick)
    else
        self:setIdling()
    end
end

return shell

