class 'fire_bar'

function fire_bar:initProps()
    -- Animation properties
    self.radius = self.npc_obj.special1*16
    --self.radius = self.npc_obj.special1*self.npc_obj.width
    self.direction = self.npc_obj.direction
    self.center_x = self.def_initX
    if(self.direction<0) then
        self.iteration=2.8
        self.angle = 90
        self.center_y = self.def_initY - self.radius
    else
        self.iteration=-2.8
        self.angle = 270
        self.center_y = self.def_initY + self.radius
    end
end

function fire_bar:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_initX = self.npc_obj.center_x
    self.def_initY = self.npc_obj.center_y
    self.def_coeff = 3.1415926 / 180.0
    self:initProps()
end

function fire_bar:onActivated()
    self:initProps()
    --self.npc_obj:activateNeighbours()
end

function fire_bar:onLoop(tickTime)
    self.angle = self.angle + smbx_utils.speedConv(self.iteration, tickTime)
    if(self.direction<0) then
        if(self.angle<-360-180) then
                self.angle=self.angle+360
        end
    else
        if(self.angle<360) then
                self.angle=self.angle+360
        end
    end

    -- keep X position if not movable flag is enabled
    if(not self.npc_obj.not_movable)then
        self.npc_obj.center_x = self.radius * math.cos(self.angle * self.def_coeff) + self.center_x;
    end
    self.npc_obj.center_y = self.radius * math.sin(self.angle * self.def_coeff) + self.center_y;
end

return fire_bar
