class 'lakitu'

local AI_NOPLAYER_ANIM = 0
local AI_FOUNDPLAYER_ANIM = 1
local AI_READYTHROW_ANIM = 2
local AI_THROW_ANIM = 3

function lakitu:__init(npc_obj)
    self.npc_obj = npc_obj
    
    self.player_pos_detector = self.npc_obj:installPlayerPosDetector()
    self.max_speed = 6
    self.speed = 0.007
    
    self.animateFlyL = {0,1,2,1}
    self.animateFlyR = {2,3,4,3}
    self.animateThrowL = {0,1,2,1}
    self.animateThrowR = {2,3,4,3}
    self.def_throwInterval = smbx_utils.ticksToTime(100)
    self.cur_mode = AI_NOPLAYER_ANIM 

    self:initProps()
end

function lakitu:initProps()
    self.npc_obj.speedX = 0
    self.npc_obj.speedY = 0
    self.npc_obj.gravity = 0
    self.npc_obj.gravity_accel = 0
    self.throwTimer = 0
    self.throwID = 48
    if(self.npc_obj.id==284)then
        self.throwID=self.npc_obj.special1
    end
    self.cur_mode = AI_NOPLAYER_ANIM 
    -- self.npc_obj:setSequence(self.animateShy)
end

function lakitu:updateThrower(tickTime)
    self.throwTimer = self.throwTimer+tickTime
    if (self.throwTimer>self.def_throwInterval) then
        self.throwTimer=0
        self:throwNPC()
    end    
end

function lakitu:throwNPC()
    local thrownNPC=self.npc_obj:spawnNPC(self.throwID, GENERATOR_APPEAR, SPAWN_UP, false)
    if(thrownNPC~=nil)then
        thrownNPC.speedX = self.npc_obj.direction * 3
        thrownNPC.speedY = -3
        thrownNPC.center_x = self.npc_obj.center_x + self.npc_obj.direction*(self.npc_obj.width/2)
        thrownNPC.center_y = self.npc_obj.top-16
        thrownNPC.special1 = 0
    end
end

function lakitu:onActivated()
    self:initProps()
end

function lakitu:isOverMaxRightwardSpeed()
    return self.npc_obj.speedX > self.max_speed
end

function lakitu:isOverMaxLeftwardSpeed()
    return self.npc_obj.speedX < -self.max_speed
end

function lakitu:isOverMaxDownwardSpeed()
    return self.npc_obj.speedY > self.max_speed
end

function lakitu:isOverMaxUpwardSpeed()
    return self.npc_obj.speedY < -self.max_speed
end

function lakitu:checkOverMaxSpeed()
    if (self:isOverMaxRightwardSpeed()) then
        self.npc_obj.speedX = self.max_speed
    end
    
    if (self:isOverMaxLeftwardSpeed()) then
        self.npc_obj.speedX = -self.max_speed
    end
    
    if (self:isOverMaxDownwardSpeed()) then
        self.npc_obj.speedY = self.max_speed
    end
    
    if (self:isOverMaxUpwardSpeed()) then
        self.npc_obj.speedY = -self.max_speed
    end
end

function lakitu:isPlayerRightOflakitu()
    return self.npc_obj.center_x < self.player_pos_detector:positionX()
end

function lakitu:isPlayerBelowOflakitu()
    return self.npc_obj.center_y < self.player_pos_detector:positionY()-150
end

function lakitu:decreaseAcceleration(tickTime)
    if ((self.npc_obj.speedX > 0) and (self:isPlayerRightOflakitu()==true))then
        self.npc_obj.speedX = self.npc_obj.speedX - (self.speed * 2 * tickTime)
    elseif ((self.npc_obj.speedX < 0) and (self:isPlayerRightOflakitu()==false)) then
        self.npc_obj.speedX = self.npc_obj.speedX + (self.speed * 2 * tickTime)
        else
                self.npc_obj.speedX = 0
    end
    
    if ((self.npc_obj.speedY > 0) and (self:isPlayerBelowOflakitu()==true))then
        self.npc_obj.speedY = self.npc_obj.speedY - (self.speed * 2 * tickTime)
    elseif ((self.npc_obj.speedY < 0) and (self:isPlayerBelowOflakitu()==false)) then
        self.npc_obj.speedY = self.npc_obj.speedY + (self.speed * 2 * tickTime)
        else
                self.npc_obj.speedY = 0
    end
end

function lakitu:onLoop(tickTime)
        self.npc_obj.direction = self.player_pos_detector:directedTo()
        --horizontal movement
        if (self:isPlayerRightOflakitu()) then
            self.npc_obj.speedX = self.npc_obj.speedX+(self.speed * tickTime)
        else
            self.npc_obj.speedX = self.npc_obj.speedX-(self.speed * tickTime)
        end

        --verticle movement
        if (self:isPlayerBelowOflakitu()) then
            self.npc_obj.speedY = self.npc_obj.speedY + (self.speed * tickTime)
        else
            self.npc_obj.speedY = self.npc_obj.speedY - (self.speed * tickTime)
        end

        if(self.throwID~=0)then
            self:updateThrower(tickTime)
        end

        self:checkOverMaxSpeed()
end

return lakitu


