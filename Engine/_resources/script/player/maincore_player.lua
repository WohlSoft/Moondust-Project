class 'luaPlayer' (BasePlayer)

function luaPlayer:__setupEvents()
    if(not self.isInvalid and self.controller)then
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

        self.Has_onTakeNpc = false
        if(type(self.controller.onTakeNpc) == "function")then
            self.Has_onTakeNpc = true
        end

        self.Has_onKillNpc = false
        if(type(self.controller.onKillNpc) == "function")then
            self.Has_onKillNpc = true
        end
    end
end

-- local i = 0
function luaPlayer.__init(self)
    BasePlayer.__init(self)

    self:__setupEvents()
end

function luaPlayer:baseReInit(theID)
    -- Init all variable by default
    self.isInvalid = false -- If this is true, then events won't be forwarded to the algorithm controller.
    self.controller = nil -- This is the controller class

    -- Setup NPC Controller
    if(player_class_table[theID])then
        self.controller = player_class_table[theID](self)
    else
        self.isInvalid = true
    end
    self:__setupEvents()
end

-- This function will be called if we have all data we need.
function luaPlayer:onInit()
    self:baseReInit(self.characterID)
    BasePlayer.onInit(self)
end

function luaPlayer:onLoop(tickTime)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onLoop)then
            self.controller:onLoop(tickTime)
        end
    end
    BasePlayer.onLoop(self, tickTime)
end

function luaPlayer:onHarm(harmEvent)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onHarm)then
            self.controller:onHarm(harmEvent)
        end
    end
    BasePlayer.onHarm(self, harmEvent)
end

function luaPlayer:onTransform(charID, state)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onTransform)then
            self.controller:onTransform(charID, state)
        end
    end

    self:baseReInit(charID)
    BasePlayer.onTransform(self, charID, state)
end

function luaPlayer:onTakeNpc(npcObj)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onTakeNpc)then
            self.controller:onTakeNpc(npcObj)
        end
    end
    BasePlayer.onTakeNpc(self, onTakeNpc)
end

function luaPlayer:onKillNpc(npcObj)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onKillNpc)then
            self.controller:onKillNpc(npcObj)
        end
    end
    BasePlayer.onKillNpc(self, onTakeNpc)
end

return luaPlayer

