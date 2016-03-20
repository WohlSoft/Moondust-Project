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

        self.Has_onTransform = false
        if(type(self.controller.onTransform) == "function")then
            self.Has_onTransform = true
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

function luaPlayer:onHarm(harmevent)
    if(not self.isInvalid and self.controller)then
        if(self.Has_onHarm)then
            self.controller:onHarm(harmEvent)
        end
    end
    BasePlayer.onHarm(self, harmevent)
--    local newHealth = self.health-harmevent.damage
--    if(newHealth==2)then
--        self:setState(2)
--        Audio.playSoundByRole(SoundRoles.PlayerHarm)
--    elseif(newHealth==1)then
--        self:setState(1)
--        Audio.playSoundByRole(SoundRoles.PlayerShrink)
--    elseif(newHealth~=0)then
--        Audio.playSoundByRole(SoundRoles.PlayerHarm)
--    end
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

return luaPlayer

