class 'dummy_boss'

-- This is a temporary and dummy script which will spawn rewards for "Legacy bosses" on kill
-- When true bosses algorithms will be implemented, this script will be removed

local TOTAL_BOSSES_16 = 0
local TOTAL_BOSSES_39 = 0
local TOTAL_BOSSES_ETC = 0

function dummy_boss:initProps()
end

function dummy_boss:add()
    if(self.npc_obj.id==15)then
       TOTAL_BOSSES_16=TOTAL_BOSSES_16+1
    elseif(self.npc_obj.id==39)then
       TOTAL_BOSSES_39=TOTAL_BOSSES_39+1
    else
       TOTAL_BOSSES_ETC=TOTAL_BOSSES_ETC+1
    end
end

function dummy_boss:substr()
    if(self.npc_obj.id==15)then
       TOTAL_BOSSES_16=TOTAL_BOSSES_16-1
    elseif(self.npc_obj.id==39)then
       TOTAL_BOSSES_39=TOTAL_BOSSES_39-1
    else
       TOTAL_BOSSES_ETC=TOTAL_BOSSES_ETC-1
    end
end

function dummy_boss:allKilled()
    if(self.npc_obj.id==15)then
       if TOTAL_BOSSES_16<=0 then return true end
    elseif(self.npc_obj.id==39)then
       if TOTAL_BOSSES_39<=0 then return true end
    else
       if TOTAL_BOSSES_ETC<=0 then return true end
    end
        return false
end

function dummy_boss:__init(npc_obj)
    self.npc_obj = npc_obj
    if(self.npc_obj.isBoss)then
        self:add()
    end
    self:initProps()
end

function dummy_boss:onActivated()
    self:initProps()
end

function dummy_boss:onLoop(TickTime)
end

function dummy_boss:onKill(killEvent)
    if(self.npc_obj.isBoss) then
        self:substr()
        if(self:allKilled()) then
            --Spawn rewards
            local spawnID = 16
            if(self.npc_obj.id==15)then
               spawnID=16
               Audio.playSound(20)
            elseif(self.npc_obj.id==39)then
               spawnID=41
               Audio.playSound(41)
            else
               Audio.playSound(20)
               return
            end

            local reward=self.npc_obj:spawnNPC(spawnID, GENERATOR_APPEAR, SPAWN_UP, false)
            reward.speedX = 0
            reward.speedY = -4.5
            reward.gravity = 1.0
            reward.center_x = self.npc_obj.center_x
            reward.center_y = self.npc_obj.center_y
        end
    end
end

return dummy_boss

