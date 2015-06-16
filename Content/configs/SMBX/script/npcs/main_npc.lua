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

function luaNPC:onLoop()
    if(not self.isInvalid and self.controller)then
        self.controller:onLoop()
    end
    BaseNPC.onLoop(self)
end

return luaNPC
