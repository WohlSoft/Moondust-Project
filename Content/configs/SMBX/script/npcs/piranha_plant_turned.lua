class 'piranha_plant_turned'

local AI_SHOWING_UP = 0
local AI_SHOWING_IDLE = 1
local AI_HIDING_DOWN = 2
local AI_HIDING_IDLE = 3

function piranha_plant_turned:hideSprite()
        self.npc_obj:setSpriteWarp(1.0, 1, true)
end

function piranha_plant_turned:updateWarp()
        self.npc_obj:setSpriteWarp(1.0-(math.abs(self.npc_obj.top-self.npc_obj.bottom)/self.def_height), 1, true)
end


function piranha_plant_turned:initProps()
    -- Animation properties

    -- Top position (reset size to initial)
    self.npc_obj.bottom = self.def_bottom
    self.npc_obj.top =    self.def_top
    self.npc_obj.bottom = self.npc_obj.top

    -- Currents
    self.cur_mode = AI_HIDING_IDLE
    -- if physics are paused, than iterations and collisions are disabled. Paused objects will NOT be detected by other
    self.npc_obj.paused_physics = true
    -- FOR AI_SHOWING_UP
    self.cur_showingUpTicks = 0

    -- FOR AI_SHOWING_IDLE
    self.cur_showingIdleTicks = 0

    -- FOR AI_HIDING_DOWN
    self.cur_hidingDownTicks = 0

    -- FOR AI_HIDING_IDLE
    self.cur_hidingIdleTicks = smbx_utils.ticksToTime(70) -- Initial wait time is 70 ticks!

    self:updateWarp()
end

function piranha_plant_turned:__init(npc_obj)
    self.npc_obj = npc_obj
    -- Config
    self.def_top = npc_obj.top
    self.def_height = npc_obj.height
    self.def_bottom = npc_obj.bottom
    self.speed = 1.5
    
    -- FOR AI_SHOWING_UP
    self.def_showingUpTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_SHOWING_IDLE
    self.def_showingIdleTicks = smbx_utils.ticksToTime(50)

    -- FOR AI_HIDING_DOWN
    self.def_hidingDownTicks = smbx_utils.ticksToTime(self.npc_obj.height/self.speed)

    -- FOR AI_HIDING_IDLE
    self.def_hidingIdleTicks = smbx_utils.ticksToTime(75)
    -- If player stands over, reset time to -10 seconds
    -- self.def_hidingIdleTicks_waitPlayer = smbx_utils.ticksToTime(65)

    npc_obj.gravity = 0

    self:initProps()
end

function piranha_plant_turned:onActivated()
    self:initProps()
end

function piranha_plant_turned:onLoop(tickTime)
    if(self.cur_mode == AI_SHOWING_UP)then
        if(self.def_showingUpTicks > self.cur_showingUpTicks)then
            self.cur_showingUpTicks = self.cur_showingUpTicks + tickTime
            self.npc_obj.bottom = self.npc_obj.bottom + smbx_utils.speedConv(self.speed, tickTime)
            self:updateWarp()
        else
            self.cur_mode = AI_SHOWING_IDLE
            self.npc_obj.bottom = self.def_bottom
            self.npc_obj:resetSpriteWarp()
            self.cur_showingUpTicks = 0
        end
    elseif(self.cur_mode == AI_SHOWING_IDLE)then
        if(self.def_showingIdleTicks >= self.cur_showingIdleTicks)then
            self.cur_showingIdleTicks = self.cur_showingIdleTicks + tickTime
        else
            self.cur_mode = AI_HIDING_DOWN
            self.cur_showingIdleTicks = 0
        end
    elseif(self.cur_mode == AI_HIDING_DOWN)then
        if(self.def_hidingDownTicks > self.cur_hidingDownTicks)then
            self.cur_hidingDownTicks = self.cur_hidingDownTicks + tickTime
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
            self.npc_obj.bottom = self.npc_obj.bottom - smbx_utils.speedConv(self.speed, tickTime)
            self:updateWarp()
        else
            self.cur_mode = AI_HIDING_IDLE
            self:hideSprite()
            self.cur_hidingDownTicks = 0
            self.npc_obj.paused_physics=true
        end
    elseif(self.cur_mode == AI_HIDING_IDLE)then
        if(self.def_hidingIdleTicks >= self.cur_hidingIdleTicks)then
            self.cur_hidingIdleTicks = self.cur_hidingIdleTicks + tickTime
        else
            --local players = Player.get()
            -- local goUp = true
            --for _, player in pairs(players) do
            --    if(math.abs(player.center_x - self.npc_obj.center_x) <= 44)then
            --        goUp = false
            --        self.cur_hidingIdleTicks = self.def_hidingIdleTicks_waitPlayer
            --    end
            --end
            -- if(goUp)then
                self.cur_mode = AI_SHOWING_UP
                self.npc_obj.paused_physics = false
                self.cur_hidingIdleTicks = 0
            --end
        end
    end
end

return piranha_plant_turned
