class 'boo'

local AI_SHY_ANIM = 0
local AI_CHASE_ANIM = 1

function boo:__init(npc_obj)
    self.npc_obj = npc_obj
    
    self.player_pos_detector = self.npc_obj:installPlayerPosDetector()
    self.max_speed = 2
    self.speed = 0.002
    
        self.animateShy = {0}
        self.animateChasing = {1}
        self.cur_mode = AI_SHY_ANIM

    self:initProps()
end

function boo:initProps()
    self.npc_obj.speedX = 0
    self.npc_obj.speedY = 0
    self.npc_obj.gravity = 0
    self.npc_obj.gravity_accel = 0
        self.cur_mode = AI_SHY_ANIM
        self.npc_obj:setSequence(self.animateShy)
end

function boo:onActivated()
    self:initProps()
end

function boo:isOverMaxRightwardSpeed()
    return self.npc_obj.speedX > self.max_speed
end

function boo:isOverMaxLeftwardSpeed()
    return self.npc_obj.speedX < -self.max_speed
end

function boo:isOverMaxDownwardSpeed()
    return self.npc_obj.speedY > self.max_speed
end

function boo:isOverMaxUpwardSpeed()
    return self.npc_obj.speedY < -self.max_speed
end

function boo:checkOverMaxSpeed()
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

function boo:isPlayerRightOfBoo()
    return self.npc_obj.center_x < self.player_pos_detector:positionX()
end

function boo:isPlayerBelowOfBoo()
    return self.npc_obj.center_y < self.player_pos_detector:positionY()
end

function boo:decreaseAcceleration(tickTime)
    if ((self.npc_obj.speedX > 0) and (self.npc_obj.direction > 0)) then
        self.npc_obj.speedX = self.npc_obj.speedX - (self.speed * 2 * tickTime)
    elseif ((self.npc_obj.speedX < 0)  and (self.npc_obj.direction < 0) ) then
        self.npc_obj.speedX = self.npc_obj.speedX + (self.speed * 2 * tickTime)
        else
        self.npc_obj.speedX = 0
    end
    
    if ((self.npc_obj.speedY > 0) and (self:isPlayerBelowOfBoo()==true))then
        self.npc_obj.speedY = self.npc_obj.speedY - (self.speed * 2 * tickTime)
    elseif ((self.npc_obj.speedY < 0) and (self:isPlayerBelowOfBoo()==false)) then
        self.npc_obj.speedY = self.npc_obj.speedY + (self.speed * 2 * tickTime)
        else
                self.npc_obj.speedY = 0
    end
end

function boo:onLoop(tickTime)
    --if (self.player_pos_detector:detected() == true) then
        if (self.player_pos_detector:playerDirection() == self.player_pos_detector:directedTo()) then
                        self.npc_obj.direction = self.player_pos_detector:directedTo()
                        if(self.cur_mode == AI_SHY_ANIM)then
                                self.cur_mode = AI_CHASE_ANIM
                                self.npc_obj:setSequence(self.animateChasing)
                        end
            --horizontal movement
            if (self:isPlayerRightOfBoo()) then
                self.npc_obj.speedX = self.npc_obj.speedX + (self.speed * tickTime)
                --self.npc_obj.direction = 1
            else
                self.npc_obj.speedX = self.npc_obj.speedX - (self.speed * tickTime)
                --self.npc_obj.direction = 0
            end
            
            --verticle movement
            if (self:isPlayerBelowOfBoo()) then
                self.npc_obj.speedY = self.npc_obj.speedY + (self.speed * tickTime)
            else
                self.npc_obj.speedY = self.npc_obj.speedY - (self.speed * tickTime)
            end
            
            self:checkOverMaxSpeed()
        else
            self:decreaseAcceleration(tickTime)
                        if(self.cur_mode == AI_CHASE_ANIM)then
                                self.cur_mode = AI_SHY_ANIM
                                self.npc_obj:setSequence(self.animateShy)
                        end
        end
    --end
end

return boo

