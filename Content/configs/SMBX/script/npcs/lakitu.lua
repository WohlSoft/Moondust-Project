class 'lakitu'

local AI_SHY_ANIM = 0
local AI_CHASE_ANIM = 1

function lakitu:__init(npc_obj)
    self.npc_obj = npc_obj
	
	self.player_pos_detector = self.npc_obj:installPlayerPosDetector()
	self.max_speed = 6
	self.speed = 0.007
	
        self.animateShy = {0}
        self.animateChasing = {1}
        self.cur_mode = AI_SHY_ANIM

	self:initProps()
end

function lakitu:initProps()
	self.npc_obj.speedX = 0
	self.npc_obj.speedY = 0
	self.npc_obj.gravity = 0
	self.npc_obj.gravity_accel = 0
        self.cur_mode = AI_SHY_ANIM
        -- self.npc_obj:setSequence(self.animateShy)
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

        self:checkOverMaxSpeed()
end

return lakitu
