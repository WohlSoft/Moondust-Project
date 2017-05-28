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
    self.npc_obj.speedX = 0
    self.npc_obj.speedY = 0
    self.wasSpawned = false
    self.npc_obj.gravity = 0
    if(self.state == ST_ON)then
        if(self.npc_obj.spawnedGenType ~= BaseNPC.SPAWN_APPEAR) then
            self.wasSpawned = true
                if self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_UP      then self.direction = DIR_UP
            elseif self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_LEFT    then self.direction = DIR_LEFT
            elseif self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_RIGHT   then self.direction = DIR_RIGHT
            elseif self.npc_obj.spawnedGenDirection == BaseNPC.SPAWN_DOWN    then self.direction = DIR_DOWN
            end
        end
    end
    self.found = false
    self.needCorrection = false
    self.lead = nil
    self.oldSpeedX = 0
    self.oldSpeedY = 0
    self.check_time_left = 0
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

function platform:getRelativePositionH( blk )
    if(self.npc_obj.center_x < blk.center_x)then
        return 1.0
    else
        return -1.0
    end    
end

function platform:getRelativePositionV( blk )
    if(self.npc_obj.center_y < blk.center_y)then
        return 1.0
    else
        return -1.0
    end    
end

function platform:atEdge( blk, dir )
    if(blk==nil)then
        return true
    end
    if( self.check_time_left>=0 )then
        return false
    end
    local radius = 4.0
    if(dir == DIR_LEFT)then
        if(self.npc_obj.center_x < blk.left+radius and self.npc_obj.center_x >= blk.left-radius)then
            return true
        end
    elseif(dir == DIR_RIGHT)then
        if(self.npc_obj.center_x > blk.right-radius and self.npc_obj.center_x <= blk.right+radius)then
            return true
        end
    elseif(dir == DIR_UP)then
        if(self.npc_obj.center_y < blk.top-radius and self.npc_obj.center_y <= blk.top-radius)then
            return true
        end
    elseif(dir == DIR_DOWN)then
        if(self.npc_obj.center_y > blk.bottom-radius and self.npc_obj.center_y >= blk.bottom+radius)then
            return true
        end
    elseif(dir == DIR_LEFT_UP)then
        if(self.npc_obj.center_x < blk.left+radius and self.npc_obj.center_x >= blk.left-radius 
            and self.npc_obj.center_y < blk.top-radius and self.npc_obj.center_y <= blk.top-radius )then
            return true
        end
    elseif(dir == DIR_UP_RIGHT)then
        if(self.npc_obj.center_x > blk.right-radius and self.npc_obj.center_x <= blk.right+radius
            and self.npc_obj.center_y < blk.top-radius and self.npc_obj.center_y <= blk.top-radius )then
            return true
        end
    elseif(dir == DIR_RIGHT_DOWN)then
        if(self.npc_obj.center_x > blk.right-radius and self.npc_obj.center_x <= blk.right+radius
            and self.npc_obj.center_y > blk.bottom-radius and self.npc_obj.center_y >= blk.bottom+radius )then
            return true
        end
    elseif(dir == DIR_DOWN_LEFT)then
        if(self.npc_obj.center_x < blk.left+radius and self.npc_obj.center_x >= blk.left-radius
            and self.npc_obj.center_y > blk.bottom-radius and self.npc_obj.center_y >= blk.bottom+radius )then
            return true
        end
    end
    return false
end

function platform:sameAsLead(blk)
    if(self.lead == nil)then
        return false;
    elseif( self.lead.id == blk.id )then -- Ignore elements of same ID
        return false;
    elseif( self.lead.center_x == blk.center_x and self.lead.center_y == blk.center_y )then
        return true;
    end
    return false;
end

function platform:isCrossesWith(blk, tbl, id)
    if(self.lead ~= nil)then
        if( self.lead.id == blk.id )then
            return false;
        end
    end
    for K,Elm in pairs(tbl) do
        if( ( Elm.id == id) and (Elm.center_x == blk.center_x) ) then
            return true;
        end
    end
    return false;
end

function platform:onLoop(tickTime)
    if(self.state==ST_ON)then
        if(self.found or self.wasSpawned)then
            self.wasSpawned = false
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
                local usefulBGOs = {}
                local byTypes = {}
                byTypes[RAIL_DIAGONAL_1] = 0
                byTypes[RAIL_DIAGONAL_2] = 0
                byTypes[RAIL_HORIZONTAL] = 0
                byTypes[RAIL_VERTICAL] = 0
                byTypes[REVERSER_1] = 0
                byTypes[REVERSER_2] = 0
                local isAtEdge = false

                self.found=false
                local countOfUSeful = 0
                for K,Blk in pairs(bgos) do
                    if( self:isCollidesCenter(Blk)
                        and ( Blk.id == RAIL_DIAGONAL_1 
                        or Blk.id == RAIL_DIAGONAL_2
                        or Blk.id == RAIL_HORIZONTAL
                        or Blk.id == RAIL_VERTICAL
                        or Blk.id == REVERSER_1
                        or Blk.id == REVERSER_2 ) ) then
                        countOfUSeful = countOfUSeful+1
                        usefulBGOs[countOfUSeful] = Blk
                        byTypes[Blk.id] = byTypes[Blk.id] + 1
                    end
                end
                if(self.lead~=nil)then
                    isAtEdge = (self:atEdge(self.lead, self.direction) and countOfUSeful <= 2)
                    -- Renderer.printText("Count Of elements: "..tostring(self.lead.id), 10, 40)
                end
                -- Renderer.printText("Count Of elements: "..tostring(countOfUSeful).. " "..tostring(isAtEdge), 10, 10)

                for K,Blk in pairs(usefulBGOs) do
                    -- if(self:isCollidesCenter(Blk)) then
                        countOfUSeful = countOfUSeful+1
                        local isNSameAsLead = not self:sameAsLead(Blk);
                        if(Blk.id==RAIL_DIAGONAL_1 and isNSameAsLead and self.direction~=DIR_UP_RIGHT and self.direction~=DIR_DOWN_LEFT )then
                            self.found=true
                            self.lead = Blk
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
                        elseif(Blk.id==RAIL_DIAGONAL_2 and isNSameAsLead and self.direction~=DIR_LEFT_UP and self.direction~=DIR_RIGHT_DOWN)then
                            self.found=true
                            self.lead = Blk
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
                        elseif(Blk.id==RAIL_HORIZONTAL and (not self:isCrossesWith(Blk, usefulBGOs, RAIL_VERTICAL) ) and
                            ((self.direction~=DIR_UP and self.direction~=DIR_DOWN) or
                             (isNSameAsLead and isAtEdge --or (self:atEdge(Blk, DIR_LEFT) or self:atEdge(Blk, DIR_RIGHT)
                                and byTypes[RAIL_HORIZONTAL]<=1 and byTypes[RAIL_VERTICAL]<=1 ) ) )then
                            self.found=true
                            self.lead = Blk
                            if( self.npc_obj.speedX > 0 ) then
                                self.direction=DIR_RIGHT
                            elseif(self.npc_obj.speedX < 0)then
                                self.direction=DIR_LEFT
                            else
                                if( self:getRelativePositionH( Blk ) == 1 )then
                                    self.direction=DIR_RIGHT
                                else
                                    self.direction=DIR_LEFT
                                end
                            end
                            self:lookForCorrection(Blk)
                        elseif(Blk.id==RAIL_VERTICAL and (not self:isCrossesWith(Blk, usefulBGOs, RAIL_HORIZONTAL) ) and
                            ((self.direction~=DIR_LEFT and self.direction~=DIR_RIGHT) or  
                             (isNSameAsLead and isAtEdge --or (self:atEdge(Blk, DIR_UP) or self:atEdge(Blk, DIR_DOWN)
                                 and byTypes[RAIL_HORIZONTAL]<=1 and byTypes[RAIL_VERTICAL]<=1 ) ) )then
                            self.found=true
                            self.lead = Blk
                            if(self.npc_obj.speedY>0)then
                                self.direction=DIR_DOWN
                            elseif(self.npc_obj.speedY<0)then
                                self.direction=DIR_UP
                            else
                                if( self:getRelativePositionV( Blk ) == 1 )then
                                    self.direction=DIR_DOWN
                                else
                                    self.direction=DIR_UP
                                end
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
                    -- end
                end
                if(self.found)then
                    self.npc_obj.gravity = 0.0
                    self.watchForMaxHeight = false
                else
                    self.npc_obj.gravity = 1.0
                    self.direction=DIR_AUTO
                    self.lead = nil
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
            local plrs = self.contacts:getPlayers()
            for K,Plr in pairs(plrs) do
                if(Plr.bottom == self.npc_obj.top and Plr.onGround)then
                    self.state=ST_ON
                end                    
            end
        end
    end

    -- Renderer.printText("PF:".." I:"..tostring(self.found).." X: "..tostring(self.npc_obj.speedX).." Y: "..tostring(self.npc_obj.speedY), 10, 80)
    
end

return platform
