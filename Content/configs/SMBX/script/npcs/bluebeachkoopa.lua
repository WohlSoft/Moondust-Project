class 'bluebeachkoopa'

function bluebeachkoopa:initProps()
    self.ignore_shells = 0
end

function bluebeachkoopa:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_speed = npc_obj.motionSpeed
    self.contacts = npc_obj:installContactDetector()
    self:initProps()
end

function bluebeachkoopa:onActivated()
    self:initProps()
end

function bluebeachkoopa:onLoop(tickTime)
    if(self.ignore_shells <= 0)then
        if(self.contacts:detected())then
            local npcs= self.contacts:getNPCs()
            for K,Npc in pairs(npcs) do
                if(npc_isShell(Npc.id))then
                    Npc.direction = self.npc_obj.direction
                    Npc.controller:toggleState()
                    self.ignore_shells = 300
                    self.npc_obj.speedX = 0
                    self.npc_obj.motionSpeed = 0
                    self.npc_obj:setSequence({2,0})
                    self.npc_obj:setOnceAnimationMode(true)
                end
            end
        end
    else
        self.ignore_shells = self.ignore_shells-tickTime
        if(self.ignore_shells <= 0)then
            self.npc_obj.motionSpeed = self.def_speed
            self.npc_obj:setSequence({0,1})
            self.npc_obj:setOnceAnimationMode(false)
        end
    end
end

return bluebeachkoopa

