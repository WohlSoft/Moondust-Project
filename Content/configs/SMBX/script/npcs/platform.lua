class 'platform'

local AI_FALLING = 0
local AI_RIDING = 1

local ST_OFF = -1
local ST_ON = 1

local DIR_AUTO = -1
local DIR_LEFT = 1
local DIR_LEFT_UP = 2
local DIR_UP = 3
local DIR_UP_RIGHT = 4
local DIR_RIGHT = 5
local DIR_RIGHT_DOWN = 6
local DIR_DOWN = 7
local DIR_DOWN_LEFT = 8

local RAIL_DIAGONAL_1 = 74 --left-top, bottom-right
local RAIL_DIAGONAL_2 = 73 --left-bottom, top-right
local RAIL_HORIZONTAL = 71
local RAIL_VERTICAL = 72
local REVERSER_1 = 70
local REVERSER_2 = 100
local CHECK_DELAY = 32

function platform:initProps()
    self.direction = DIR_AUTO
    self.speed = 2
    self.state = self.npc_obj.direction
    self.npc_obj.gravity = 1
    self.found = false
    self.check_time_left = CHECK_DELAY
end

function platform:__init(npc_obj)
    self.npc_obj = npc_obj
    self.contacts = npc_obj:installContactDetector()
    self:initProps()
end

function platform:onActivated()
    self:initProps()
end

function platform:isCollidesCenter(blk)
    if(blk==nil)then
        return false;
    elseif(self.npc_obj.center_x > blk.right)then
        return false;
    elseif(self.npc_obj.center_x < blk.left)then
        return false;
    elseif(self.npc_obj.center_y > blk.bottom)then
        return false;
    elseif(self.npc_obj.center_y < blk.top)then
        return false;
    else
        return true;
    end
end

function platform:onLoop(tickTime)
    if(self.state==ST_ON)then
        if(self.found)then
            if(self.direction==DIR_LEFT) then
                self.npc_obj.speedX=-self.speed
                self.npc_obj.speedY=0
            elseif(self.direction==DIR_LEFT_UP) then
                self.npc_obj.speedX=-self.speed
                self.npc_obj.speedY=-self.speed
            elseif(self.direction==DIR_UP) then
                self.npc_obj.speedX=0
                self.npc_obj.speedY=-self.speed
            elseif(self.direction==DIR_UP_RIGHT) then
                self.npc_obj.speedX=self.speed
                self.npc_obj.speedY=-self.speed
            elseif(self.direction==DIR_RIGHT) then
                self.npc_obj.speedX=self.speed
                self.npc_obj.speedY=0
            elseif(self.direction==DIR_RIGHT_DOWN) then
                self.npc_obj.speedX=self.speed
                self.npc_obj.speedY=self.speed
            elseif(self.direction==DIR_DOWN) then
                self.npc_obj.speedX=0
                self.npc_obj.speedY=self.speed
            elseif(self.direction==DIR_DOWN_LEFT) then
                self.npc_obj.speedX=-self.speed
                self.npc_obj.speedY=self.speed
            else
                self.npc_obj.gravity=1
            end
        end

        if(self.check_time_left>=0)then
            self.check_time_left=self.check_time_left-tickTime
        else
            self.check_time_left=CHECK_DELAY
            if(self.contacts:detected())then
                local bgos= self.contacts:getBGOs()
                self.found=false
                for K,Blk in pairs(bgos) do
                    if(Blk.right==1)then
                        Renderer.printText("meow", 10, 10)
                    end
                    if(self:isCollidesCenter(Blk)) then
                        if(Blk.id==RAIL_DIAGONAL_1)then
                            self.found=true
                            if(self.npc_obj.speedX>0)then
                                self.direction=DIR_RIGHT_DOWN
                            else
                                self.direction=DIR_LEFT_UP
                            end
                        elseif(Blk.id==RAIL_DIAGONAL_2)then
                            self.found=true
                            if(self.npc_obj.speedX>0)then
                                self.direction=DIR_UP_RIGHT
                            else
                                self.direction=DIR_DOWN_LEFT
                            end
                        elseif(Blk.id==RAIL_HORIZONTAL)then
                            self.found=true
                            if(self.npc_obj.speedX>0)then
                                self.direction=DIR_RIGHT
                            else
                                self.direction=DIR_LEFT
                            end
                        elseif(Blk.id==RAIL_VERTICAL)then
                            self.found=true
                            if(self.npc_obj.speedY>0)then
                                self.direction=DIR_DOWN
                            else
                                self.direction=DIR_UP
                            end
                        elseif(Blk.id==REVERSER_1 or Blk.id==REVERSER_2)then
                            self.found=true
                            if(self.direction==DIR_LEFT) then
                                self.direction=DIR_RIGHT
                            elseif(self.direction==DIR_LEFT_UP) then
                                self.direction=DIR_RIGHT_DOWN
                            elseif(self.direction==DIR_UP) then
                                self.direction=DIR_DOWN
                            elseif(self.direction==DIR_UP_RIGHT) then
                                self.direction=DIR_DOWN_LEFT
                            elseif(self.direction==DIR_RIGHT) then
                                self.direction=DIR_LEFT
                            elseif(self.direction==DIR_RIGHT_DOWN) then
                                self.direction=DIR_LEFT_UP
                            elseif(self.direction==DIR_DOWN) then
                                self.direction=DIR_UP
                            elseif(self.direction==DIR_DOWN_LEFT) then
                                self.direction=DIR_UP_RIGHT
                            end
                            self.npc_obj.speedX=-self.npc_obj.speedX
                            self.npc_obj.speedY=-self.npc_obj.speedY
                        end
                    end
                end
                if(self.found)then
                    self.npc_obj.gravity=0
                else
                    self.npc_obj.gravity=1
                end
            end
        end
    else
        self.npc_obj.speedX=0
        self.npc_obj.speedY=0
        if(self.contacts:detected())then
            local plrs= self.contacts:getPlayers()
            for K,Plr in pairs(plrs) do
                if(Plr.bottom==self.npc_obj.top)then
                    self.state=ST_ON
                end                    
            end
        end
    end
end

return platform
