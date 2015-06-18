class 'podoboo'

local AI_JUMPING = 0
local AI_FALLING = 1
local AI_IDLE = 2
local SMBXTickTime = 1000.0/65.0

function ticksToTime(ticks)
	return ticks*SMBXTickTime
end

function initProps(thenpc)
    -- Config
    thenpc.def_jumpingUpTicks = ticksToTime(30)         -- The ticks where podoboo def_jumpingUpSpeed is beeing forced. (jumping up)
    thenpc.def_idleTicks = ticksToTime(150)             -- The ticks where podoboo is idleing
    thenpc.def_gravity = 0.55              -- The gravity which is used for the podoboo
    thenpc.def_jumpingUpSpeed = -6         -- The speed when the podoboo is jumping up.
    thenpc.startingY = thenpc.npc_obj.y+34   -- The starting y position, this is needed for detecting, when idleing is needed.
    
    -- Currents
    thenpc.cur_jumpingUpTicks = 0          -- The tick counter from 0 to 30 when forcing 
    thenpc.cur_idleTicks = 0               -- The idle timer from 0 to 150
    thenpc.cur_mode = AI_JUMPING           -- The current "AI"-Mode
    thenpc.npc_obj.gravity = 0
    thenpc.npc_obj.y = thenpc.startingY
end

function podoboo:__init(npc_obj)
    self.npc_obj = npc_obj
    initProps(self)
end

function podoboo:onActivated()
    initProps(self)
end

function podoboo:onLoop(tickTime)
    --Renderer.printText("podo ticks: "..tickTime.." left Up"..self.cur_jumpingUpTicks, 20, 440)
    --Renderer.printText("podo y: "..self.startingY, 20, 440)
    --Renderer.printText("podo cy: "..self.npc_obj.y, 20, 480)
    --Renderer.printText("podo cy < y: "..tostring(self.startingY < self.npc_obj.y), 20, 520)
    if(self.cur_mode == AI_JUMPING)then
        -- Renderer.printText("AI_JUMPING", 20, 400)
        if(self.cur_jumpingUpTicks == 0)then
            self.npc_obj.gravity = self.def_gravity
        end
        if(self.def_jumpingUpTicks > self.cur_jumpingUpTicks)then
            self.npc_obj.speedY = self.def_jumpingUpSpeed
            self.cur_jumpingUpTicks = self.cur_jumpingUpTicks + tickTime
            if(self.def_jumpingUpTicks <= self.cur_jumpingUpTicks)then
                self.cur_mode = AI_FALLING
                self.cur_jumpingUpTicks = 0
            end
        end
    elseif(self.cur_mode == AI_FALLING)then
        --Renderer.printText("AI_FALLING", 20, 400)
        self.cur_idleTicks = self.cur_idleTicks + tickTime
        if(self.npc_obj.y > self.startingY)then
            self.npc_obj.speedY = 0
            self.npc_obj.y = self.startingY
            self.npc_obj.gravity = 0
            self.cur_mode = AI_IDLE
        end
    elseif(self.cur_mode == AI_IDLE)then
        --Renderer.printText("AI_IDLE", 20, 400)
        if(self.cur_idleTicks >= self.def_idleTicks)then
            self.cur_idleTicks = 0
            self.cur_mode = AI_JUMPING
        else
            self.cur_idleTicks = self.cur_idleTicks + tickTime
        end
    end    
end

return podoboo
