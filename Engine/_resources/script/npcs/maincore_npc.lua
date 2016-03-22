class 'luaNPC' (BaseNPC)

function luaNPC:__setupEvents()
    if(not self.isInvalid and self.controller)then
        self.Has_onActivated = false
        if(type(self.controller.onActivated) == "function")then
            self.Has_onActivated = true
        end

        self.Has_onLoop = false
        if(type(self.controller.onLoop) == "function")then
            self.Has_onLoop = true
        end

        self.Has_onKill = false
        if(type(self.controller.onKill) == "function")then
            self.Has_onKill = true
        end

        self.Has_onHarm = false
        if(type(self.controller.onHarm) == "function")then
            self.Has_onHarm = true
        end

        self.Has_onTransform = false
        if(type(self.controller.onTransform) == "function")then
            self.Has_onTransform = true
        end
    end
end

-- Note: This function is useless, because we need the id data.
function luaNPC:__init()
    BaseNPC.__init(self)

    self:__setupEvents()
end

function luaNPC:baseReInit(theID)
    -- Init all variable by default
    self.isInvalid = false -- If this is true, then events won't be forwarded to the algorithm controller.
    self.controller = nil -- This is the controller class

    -- Setup NPC Controller
    if(npc_class_table[theID])then
        self.controller = npc_class_table[theID](self)
    else
        self.isInvalid = true
    end
    self:__setupEvents()
end



-- This function will be called if we have all data we need.
function luaNPC:onInit()
    self:baseReInit(self.id)
    BaseNPC.onInit(self)
end

function luaNPC:onActivated()
    if(not self.isInvalid and self.controller)then
        if(self.Has_onActivated)then
            self.controller:onActivated()
        end
    end
    BaseNPC.onActivated(self)
end

function luaNPC:onLoop(tickTime)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onLoop)then
            self.controller:onLoop(tickTime)
        end
    end
    BaseNPC.onLoop(self, tickTime)
end

function luaNPC:onKill(killEvent)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onKill)then
            self.controller:onKill(killEvent)
        end
    end
    BaseNPC.onKill(self, killEvent)
end

function luaNPC:onHarm(harmEvent)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onHarm)then
            self.controller:onHarm(harmEvent)
        end
    end
    BaseNPC.onHarm(self, harmEvent)
end

function luaNPC:onTransform(id)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onTransform)then
            self.controller:onTransform(id)
        end
    end

    self:baseReInit(id)
    BaseNPC.onTransform(self, id)
end

return luaNPC
