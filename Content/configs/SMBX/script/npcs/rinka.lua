class 'rinka'

function rinka:initProps()
    -- FOR AI_SHOWING_IDLE
    self.cur_shootDelay = 0
    self.target_detected=false
end

function rinka:__init(npc_obj)
    self.npc_obj = npc_obj
    self.def_shootDelay = smbx_utils.ticksToTime(50)
    self.def_speed = 3
    self.player_pos_detector = self.npc_obj:installPlayerPosDetector()
    self:initProps()
end

function rinka:onActivated()
    self:initProps()
end

function rinka:onLoop(tickTime)
    if(not self.target_detected)then
        self.cur_shootDelay = self.cur_shootDelay + tickTime
        if (self.def_shootDelay <= self.cur_shootDelay) then
            pposX = self.player_pos_detector:positionX()
            pposY = self.player_pos_detector:positionY()
            hDirect = self.player_pos_detector:directedTo()
            vDirect = 0
            if(pposY<self.npc_obj.center_y)then
                vDirect=-1
            else
                vDirect=1
            end
            hdist = math.abs(pposX-self.npc_obj.center_x)
            vdist = math.abs(pposY-self.npc_obj.center_y)
            angle = 0
            if(vdist~=0)then
                angle = (180.0/math.pi) * math.atan(hdist/vdist)
            end
            if((hDirect>=0) and vDirect>=0)then
                self.npc_obj.speedX=math.cos(angle)*self.def_speed
                self.npc_obj.speedY=math.sin(angle)*self.def_speed
            elseif((hDirect>=0) and vDirect<0)then
                self.npc_obj.speedX=-math.sin(angle)*self.def_speed*hDirect
                self.npc_obj.speedY=-math.cos(angle)*self.def_speed*vDirect
            elseif((hDirect<0) and vDirect<0)then
                self.npc_obj.speedX=-math.sin(angle)*self.def_speed*hDirect
                self.npc_obj.speedY=-math.cos(angle)*self.def_speed*vDirect
            elseif((hDirect<0) and vDirect>=0)then
                self.npc_obj.speedX=-math.sin(angle)*self.def_speed*hDirect
                self.npc_obj.speedY=-math.cos(angle)*self.def_speed*vDirect
            end
            self.target_detected = true
        end
    end
end

return rinka

