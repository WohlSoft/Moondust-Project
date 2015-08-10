class 'luaNPC' (BaseNPC)

-- Note: This function is useless, because we need the id data.
function luaNPC:__init()
    BaseNPC.__init(self)
end

-- This function will be called if we have all data we need.
function luaNPC:onInit()
    -- Init all variable by default
    self.isInvalid = false -- If this is true, then events won't be forwarded to the algorithm controller. 
    self.controller = nil -- This is the controller class
    
    -- Setup NPC Controller
    if(npc_class_table[self.id])then
        self.controller = npc_class_table[self.id](self)
    else
        self.isInvalid = true
    end
    BaseNPC.onInit(self)
end

function luaNPC:onActivated()
    if(not self.isInvalid and self.controller)then
        if(type(self.controller.onActivated) == "function")then
            self.controller:onActivated()
        end
    end
    BaseNPC.onActivated(self)
end

function luaNPC:onLoop(tickTime)
    if(not self.isInvalid and self.controller)then
        if(type(self.controller.onLoop) == "function")then
            self.controller:onLoop(tickTime)
        end
    end
    BaseNPC.onLoop(self, tickTime)
end

function luaNPC:onKill(damageReason)
    if(not self.isInvalid and self.controller)then
        if(type(self.controller.onKill) == "function")then
            self.controller:onKill(damageReason)
        end
    end
    BaseNPC.onKill(self, damageReason)
end

function luaNPC:onHarm(damage, damageReason)
    if(not self.isInvalid and self.controller)then
        if(type(self.controller.onHarm) == "function")then
            self.controller:onHarm(damage, damageReason)
        end
    end
    BaseNPC.onHarm(self, damage, damageReason)
end

return luaNPC
