class 'vinehead'

function vinehead:mod(a, b)
    return a - math.floor(a/b)*b
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
    vine.center_x = self.npc_obj.center_x
    if(self.npc_obj.y>0)then
        vine.y = self.npc_obj.y - (self:mod(self.npc_obj.y, 32))
    else
        vine.y = self.npc_obj.y+(32-self:mod(self.npc_obj.y, 32))
    end
end

function vinehead:initProps()
    -- Animation properties
    --self.radius = 4*32-self.npc_obj.width/2-16
    self.init_y = self.npc_obj.y
    self.passed_height = 32
    self.firstTime=true
    self.speed = -2
end

function vinehead:__init(npc_obj)
    self.npc_obj = npc_obj
    self.init_y = 0
    self.passed_height = 0
    self:initProps()
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
    end
end

return vinehead

