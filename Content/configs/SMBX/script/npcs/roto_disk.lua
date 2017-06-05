class 'roto_disk'

function roto_disk:initProps()
    -- Animation properties
    --self.radius = 4*32-self.npc_obj.width/2-16
    self.radius = 4*32-32
    self.direction = self.npc_obj.direction

    self.center_x = self.def_initX
    if(self.direction<0) then
        self.iteration=2.75
        self.angle = 90
        self.center_y = self.def_initY - self.radius
    else
        self.iteration=-2.75
        self.angle = 270
        self.center_y = self.def_initY + self.radius
    end
end

function roto_disk:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_initX = self.npc_obj.center_x
    self.def_initY = self.npc_obj.center_y
    self.def_coeff = 3.1415926 / 180.0
    self:initProps()
end

function roto_disk:onActivated()
    self:initProps()
end

function roto_disk:onLoop(tickTime)
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

return roto_disk
