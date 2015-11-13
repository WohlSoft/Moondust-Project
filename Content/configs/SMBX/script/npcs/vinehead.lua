class 'vinehead'

function vinehead:round(num) 
    if num >= 0 then return math.floor(num+.5) 
    else return math.ceil(num-.5) end
end

function vinehead:spawnVine()
    local spawnID=0
    if(self.npc_obj.id==226)then
        spawnID = 213
    elseif(self.npc_obj.id==225)then
        spawnID = 214
    elseif(self.npc_obj.id==227)then
        spawnID = 224
    end

    local vine=self.npc_obj:spawnNPC(spawnID, GENERATOR_APPEAR, SPAWN_UP, false)
    vine.speedX = 0
    vine.speedY = 0

    -- Let's align Y value to 32px grid!
    local rY = self:round(self.npc_obj.y)
    local gridY = rY - (math.fmod(rY, 32))
    if(rY<0)then
        if(rY < (gridY-16) )then
            gridY = gridY-32
        end
    else
        if(rY > (gridY+16) )then
            gridY = gridY+32
        end
    end

    vine.center_x = self.npc_obj.center_x
    vine.y = gridY
end

function vinehead:initProps()
    -- Animation properties
    --self.radius = 4*32-self.npc_obj.width/2-16
    self.init_y = self.npc_obj.y
    self.passed_height = 0
    self.firstTime=true
    self.speed = -2
end

function vinehead:__init(npc_obj)
    self.npc_obj = npc_obj
    self.init_y = 0
    self.passed_height = 0
    self.firstTime=false
    self.contacts = npc_obj:installContactDetector()
end

function vinehead:onActivated()
    self:initProps()
end

function vinehead:onLoop(tickTime)
    if(self.firstTime)then
        self:spawnVine()
        self.firstTime=false
    end

    self.npc_obj.speedY=self.speed
    self.passed_height = math.abs(self.init_y-self.npc_obj.y)
    if(self.passed_height>=32)then
        self:spawnVine()
        self.init_y = self.init_y-32
        -- selfDestroy on contact with solid blocks
        if(self.contacts:detected())then
            local blocks= self.contacts:getBlocks()
            for K,Blk in pairs(blocks) do
                if(Blk.isSolid and Blk.y>=self.npc_obj.y)then
                    self.npc_obj:unregister()
                end
            end
        end
    end
end

return vinehead

