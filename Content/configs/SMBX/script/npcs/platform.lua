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
local CHECK_DELAY = 24

function platform:initProps()
    self.direction = DIR_AUTO
    self.speed = 2
    self.state = self.npc_obj.direction
    self.npc_obj.gravity = 1
    self.found = false
    self.needCorrection = false
    self.oldSpeedX = 0
    self.oldSpeedY = 0
    self.check_time_left = CHECK_DELAY
    self.watchForMaxHeight = true --Until path will be catched
    self.maxHeight = self.npc_obj.y
    self.RecentMaxHeight = self.npc_obj.y
end

function platform:__init(npc_obj)
    self.npc_obj = npc_obj
    self.npc_obj.maxVelX = 0.0
    self.npc_obj.maxVelY = 0.0
    self.contacts = npc_obj:installContactDetector()
    self:initProps()
end

function platform:onActivated()
    self:initProps()
end

function platform:findXfromY( blk, y )
    local ratio = blk.width / blk.height
    return blk.left + ((y - blk.top) * ratio)
end

function platform:findYfromX( blk, x )
    local ratio = blk.height / blk.width
    return blk.top + ((x - blk.left) * ratio)
end

function platform:findXfromY_R( blk, y )
    local ratio = blk.width / blk.height
    return blk.left + blk.width - ((y - blk.top) * ratio)
end

function platform:lookForCorrection( blk )
    self.oldSpeedX = self.npc_obj.speedX
    self.oldSpeedY = self.npc_obj.speedY
    if(blk.id==RAIL_DIAGONAL_1)then
        local diagonal = self:findXfromY(blk, self.npc_obj.center_y)
        if( self.npc_obj.center_x < diagonal)then
            self.npc_obj.speedX = self.npc_obj.speedX+diagonal-self.npc_obj.center_x
            self.needCorrection = true
        elseif( self.npc_obj.center_x > diagonal)then
            self.npc_obj.speedX = self.npc_obj.speedX+diagonal-self.npc_obj.center_x
            self.needCorrection = true
        end
    elseif(blk.id==RAIL_DIAGONAL_2)then
        local diagonal = self:findXfromY_R(blk, self.npc_obj.center_y)
        if( self.npc_obj.center_x < diagonal)then
            self.npc_obj.speedX = self.npc_obj.speedX+diagonal-self.npc_obj.center_x
            self.needCorrection = true
        elseif( self.npc_obj.center_x > diagonal)then
            self.npc_obj.speedX = self.npc_obj.speedX+diagonal-self.npc_obj.center_x
            self.needCorrection = true
        end
    elseif(blk.id==RAIL_HORIZONTAL)then
        if(self.npc_obj.center_y < blk.center_y)then
            self.npc_obj.speedY = blk.center_y-self.npc_obj.center_y
            self.needCorrection = true
        elseif(self.npc_obj.center_y > blk.center_y)then
            self.npc_obj.speedY = blk.center_y-self.npc_obj.center_y
            self.needCorrection = true
        end
    elseif(blk.id==RAIL_VERTICAL)then
        if(self.npc_obj.center_x < blk.center_x)then
            self.npc_obj.speedX = blk.center_x-self.npc_obj.center_x
            self.needCorrection = true
        elseif(self.npc_obj.center_x > blk.center_x)then
            self.npc_obj.speedX = blk.center_x-self.npc_obj.center_x
            self.needCorrection = true
        end
    end
end


function platform:isCollidesCenter(blk)
    if(blk==nil)then
        return false;
    end
    
    local speedLeft = -1
    local speedRight = -1
    local speedTop = -1
    local speedBottom = -1

    if(self.npc_obj.speedX > 0)then
        speedRight = 4
        -- Renderer.printText("111111!", 10, 10)
    elseif(self.npc_obj.speedX < 0)then
        speedLeft = 4
        -- Renderer.printText("222222!", 10, 10)
    else
        speedLeft = 4
        speedRight = 4
        -- Renderer.printText("333333!", 10, 10)
    end
    if(self.npc_obj.speedY > 0)then
        speedBottom = 4
    elseif(self.npc_obj.speedY < 0)then
        speedTop = 4
    else
        speedTop = 4
        speedBottom = 4
    end
    
    if(self.npc_obj.center_x-speedLeft > blk.right)then
        return false;
    elseif(self.npc_obj.center_x+speedRight < blk.left)then
        return false;
    elseif(self.npc_obj.center_y-speedTop > blk.bottom)then
        return false;
    elseif(self.npc_obj.center_y+speedBottom < blk.top)then
        return false;
    else
        return true;
    end
end

function platform:onLoop(tickTime)
    if(self.state==ST_ON)then
        if(self.found)then
            if(self.needCorrection)then
                self.npc_obj.speedX = self.oldSpeedX
                self.npc_obj.speedY = self.oldSpeedY
            end
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
                self.npc_obj.gravity = 1.0
            end
        end

        if( self.check_time_left>=0 )then
            self.check_time_left = self.check_time_left-tickTime
        else
            -- self.check_time_left=CHECK_DELAY
            local RecentDirection = self.direction
            if(self.contacts:detected())then
                local bgos= self.contacts:getBGOs()
                self.found=false
                for K,Blk in pairs(bgos) do
                    if(self:isCollidesCenter(Blk)) then
                        if(Blk.id==RAIL_DIAGONAL_1)then
                            self.found=true
                            -- X<0
                            if( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY == 0) )then
                                self.direction = DIR_LEFT_UP
                            -- X>0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY == 0) )then
                                self.direction = DIR_RIGHT_DOWN
                            -- Y<0
                            elseif( (self.npc_obj.speedX == 0) and (self.npc_obj.speedY < 0) )then
                                self.direction = DIR_LEFT_UP
                            -- Y>0                                
                            elseif( (self.npc_obj.speedX == 0) and (self.npc_obj.speedY > 0) )then
                                self.direction = DIR_RIGHT_DOWN
                            -- X<0  Y<0
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY < 0) )then
                                self.direction = DIR_LEFT_UP
                            -- X>0  Y>0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY > 0) )then
                                self.direction = DIR_RIGHT_DOWN
                            -- X>0  Y<0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY < 0)
                                and ( math.abs(self.npc_obj.speedY) > math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_LEFT_UP
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY < 0) 
                                and ( math.abs(self.npc_obj.speedY) < math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_RIGHT_DOWN
                            -- X<0  Y>0
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY > 0)
                                and ( math.abs(self.npc_obj.speedY) < math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_LEFT_UP
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY > 0) 
                                and ( math.abs(self.npc_obj.speedY) > math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_RIGHT_DOWN
                            else
                                self.direction = DIR_RIGHT_DOWN
                            end
                            self:lookForCorrection(Blk)
                        elseif(Blk.id==RAIL_DIAGONAL_2)then
                            self.found=true
                            -- X<0
                            if( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY == 0) )then
                                self.direction = DIR_DOWN_LEFT
                            -- X>0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY == 0) )then
                                self.direction = DIR_UP_RIGHT
                            -- Y<0
                            elseif( (self.npc_obj.speedX == 0) and (self.npc_obj.speedY < 0) )then
                                self.direction = DIR_UP_RIGHT
                            -- Y>0                                
                            elseif( (self.npc_obj.speedX == 0) and (self.npc_obj.speedY > 0) )then
                                self.direction = DIR_DOWN_LEFT
                            -- X<0  Y>0
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY > 0) )then
                                self.direction = DIR_DOWN_LEFT
                            -- X>0  Y<0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY < 0) )then
                                self.direction = DIR_UP_RIGHT
                            -- X<0  Y<0
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY < 0)
                                and ( math.abs(self.npc_obj.speedY) > math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_UP_RIGHT
                            elseif( (self.npc_obj.speedX < 0) and (self.npc_obj.speedY < 0) 
                                and ( math.abs(self.npc_obj.speedY) < math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_DOWN_LEFT
                            -- X>0  Y>0
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY > 0)
                                and ( math.abs(self.npc_obj.speedY) > math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_DOWN_LEFT
                            elseif( (self.npc_obj.speedX > 0) and (self.npc_obj.speedY > 0) 
                                and ( math.abs(self.npc_obj.speedY) < math.abs(self.npc_obj.speedX) ) )then
                                self.direction = DIR_UP_RIGHT
                            else
                                self.direction = DIR_UP_RIGHT
                            end
                            self:lookForCorrection(Blk)
                        elseif(Blk.id==RAIL_HORIZONTAL and self.direction~=DIR_UP and self.direction~=DIR_DOWN)then
                            self.found=true
                            if(self.npc_obj.speedX>0)then
                                self.direction=DIR_RIGHT
                            else
                                self.direction=DIR_LEFT
                            end
                            self:lookForCorrection(Blk)
                        elseif(Blk.id==RAIL_VERTICAL and self.direction~=DIR_LEFT and self.direction~=DIR_RIGHT)then
                            self.found=true
                            if(self.npc_obj.speedY>0)then
                                self.direction=DIR_DOWN
                            else
                                self.direction=DIR_UP
                            end
                            self:lookForCorrection(Blk)
                        elseif(Blk.id==REVERSER_1 or Blk.id==REVERSER_2)then
                            -- self.found=true
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
                            self.npc_obj.speedX = -self.npc_obj.speedX --math.abs(Blk.top-self.npc_obj.bottom)
                            if(self.watchForMaxHeight)then
                                if(self.RecentMaxHeight < self.maxHeight)then
                                    self.npc_obj.speedY = -self.npc_obj.speedY+(math.abs(Blk.top-self.npc_obj.bottom)+math.abs(self.npc_obj.speedY))/65.42
                                else
                                    self.npc_obj.speedY = -self.npc_obj.speedY
                                end
                                self.RecentMaxHeight = self.npc_obj.y
                            else
                                self.npc_obj.speedY = -self.npc_obj.speedY +(math.abs(self.npc_obj.speedY))/(1000.0/15.285)
                            end
                            if(self.direction~=DIR_AUTO)then
                                self.check_time_left = CHECK_DELAY
                            else
                                self.check_time_left = CHECK_DELAY/math.abs(self.npc_obj.speedY)
                            end
                        end
                    end
                end
                if(self.found)then
                    self.npc_obj.gravity = 0.0
                    self.watchForMaxHeight = false
                else
                    self.npc_obj.gravity = 1.0
                    self.direction=DIR_AUTO
                end
            end

            if(self.watchForMaxHeight)then
                if(self.npc_obj.y<self.RecentMaxHeight)then
                    self.RecentMaxHeight = self.npc_obj.y
                end
            end

            if(RecentDirection ~= self.direction)then
                self.check_time_left=CHECK_DELAY
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

    -- Renderer.printText("PF:".." I:"..tostring(self.found).." X: "..tostring(self.npc_obj.speedX).." Y: "..tostring(self.npc_obj.speedY), 10, 80)
    
end

return platform
