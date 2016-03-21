class 'shell'

local AI_IDLING = 0
local AI_RUNNING = 1

--self.npc_obj:setSequence({0})
--self.npc_obj:setSequence({0,1,2,3})

function shell:initProps()
    -- Currents
    self.cur_mode = AI_IDLING
end

function shell:__init(npc_obj)
    self.npc_obj = npc_obj
    self:initProps()
end

function shell:onActivated()
    self:initProps()
end

function shell:onLoop(tickTime)
end

function shell:onHarm(harmEvent)
    harmEvent.cancel=true
    harmEvent.damage=0
    self:toggleState()
end

function shell:onKill(killEvent)
    killEvent.cancel=true
    self:toggleState()
end

function shell:toggleState()
    if(self.cur_mode == AI_IDLING)then
        self.cur_mode=AI_RUNNING
        self.npc_obj.motionSpeed = 300
        self.npc_obj.frameDelay = 32
        self.npc_obj:setSequence({0,1,2,3})
        Audio.playSoundByRole(SoundRoles.PlayerKick)
    else
        self.cur_mode=AI_IDLING
        self.npc_obj.motionSpeed = 0
        self.npc_obj.speedX = 0
        self.npc_obj:setSequence({0})
    end    
end

return shell

