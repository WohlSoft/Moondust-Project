class 'glassChamber'

local glass1 = {0, 1, 2, 3, 4, 5, 6}
local glass2 = {7, 8, 9, 10, 11, 12, 13}
local glass3 = {6, 5, 4, 3, 2, 1, 0}
local glass4 = {13, 12, 11, 10, 9, 8, 7}
local GLASSES = {glass1, glass2, glass3, glass4}

function glassChamber:initProps()
    self.cur_frame = self.frames-self.npc_obj.health
    self.npc_obj:setSequence({self.cur_frame})
end

function glassChamber:__init(npc_obj)
    self.npc_obj = npc_obj
    self.frames = npc_obj.health
    self:initProps()
end

function glassChamber:onActivated()
    self:initProps()
end

function glassChamber:onHarm(harmEvent)
    self.cur_frame=self.frames-(self.npc_obj.health-harmEvent.damage)
    self.npc_obj:setSequence({self.cur_frame})
end

function glassChamber:onKill(killEvent)
    local effect = EffectDef();
        effect.id = 111
        effect.startX = self.npc_obj.center_x
        effect.startY = self.npc_obj.center_y
        effect.gravity = 24.0;
        effect.startDelay = 0.0;
        effect.maxVelX = 25.0;
        effect.velocityX = -1.0;
        effect.velocityY = -12.5;
        effect.maxDelay = 50000;
        effect.loopsCount = 0;
        effect:setSequence(glass1)
        effect.framespeed = 32;
    for i=0,100, 1
    do
            effect:setSequence(GLASSES[math.random(1,4)])
            effect.startX = self.npc_obj.center_x + (self.npc_obj.width/2.0)*math.random(-10, 10)/10.0;
            effect.startY = self.npc_obj.center_y + (self.npc_obj.height/2.0)*math.random(-10, 10)/10.0;
            effect.velocityX = 4.0*math.random(-10, 10)/10.0;
            effect.velocityY = -12.2+6.0*math.random(1, 10)/10.0;
        Effect.runEffect(effect, true);
    end
end

return glassChamber

