class 'dryBones'

local AI_NORMAL    = 0
local AI_COLLAPSED = 1

local ANIM_WALKING = 0
local ANIM_COLLAPSING = 1
local ANIM_ASSEMBLING = 2

function dryBones:initProps()
    self:assemble()
end

function dryBones:__init(npc_obj)
    self.npc_obj = npc_obj
    self.animateWalkL = {0,1}
    self.animateWalkR = {2,3}
    self.animateCollapse1L = {4}
    self.animateCollapse1R = {6}
    self.animateCollapse2L = {5}
    self.animateCollapse2R = {7}

    self.wigglingVal = -1
    self.wigglingDelay = 28

    self.def_collide_player = self.npc_obj.collide_player
    self.def_collide_npc = self.npc_obj.collide_npc
    self.def_en_collide_plr = self.npc_obj.collideWithPlayers

    self.def_motionSpeed = self.npc_obj.motionSpeed
    self.def_collapsed_ticks = smbx_utils.ticksToTime(400)
    self.def_wiggling_ticks = smbx_utils.ticksToTime(300)
    self.def_breaking_ticks = smbx_utils.ticksToTime(20)
    self.def_assembling_ticks = smbx_utils.ticksToTime(380)
end

function dryBones:onActivated()
    self:initProps()
end

function dryBones:onLoop(tickTime)
    if(self.state_cur==AI_COLLAPSED)then
        self.cur_collapsed_ticks = self.cur_collapsed_ticks + tickTime
        if((self.cur_animation==ANIM_WALKING) and (self.cur_collapsed_ticks>=self.def_breaking_ticks))then
            self.npc_obj:setSequenceLeft(self.animateCollapse2L)
            self.npc_obj:setSequenceRight(self.animateCollapse2R)
            self.cur_animation=ANIM_COLLAPSING
        elseif((self.cur_animation==ANIM_COLLAPSING) 
                and (self.cur_collapsed_ticks>=self.def_wiggling_ticks)
                and (self.cur_collapsed_ticks<self.def_assembling_ticks)) then
                self.cur_wiggling_ticks = self.cur_wiggling_ticks-tickTime
                if(self.cur_wiggling_ticks<=0)then
                    self.npc_obj:setGfxOffsetX(self.wigglingVal)
                    self.wigglingVal = self.wigglingVal * -1
                    self.cur_wiggling_ticks = self.cur_wiggling_ticks+self.wigglingDelay
                end
        elseif((self.cur_animation==ANIM_COLLAPSING) and (self.cur_collapsed_ticks>=self.def_assembling_ticks))then
            self.npc_obj:setGfxOffsetX(0.0)
            self.npc_obj:setSequenceLeft(self.animateCollapse1L)
            self.npc_obj:setSequenceRight(self.animateCollapse1R)
            self.cur_animation=ANIM_ASSEMBLING
        elseif((self.cur_animation==ANIM_ASSEMBLING) and (self.cur_collapsed_ticks>=self.def_collapsed_ticks))then
            self.cur_animation=ANIM_WALKING
            self:assemble()
        end
    end
end

function dryBones:onHarm(harmEvent)
    harmEvent.cancel=true
    self:collapse()
end


function dryBones:assemble()
    self.state_cur = AI_NORMAL
    self.npc_obj:setGfxOffsetX(0.0)
    self.npc_obj.motionSpeed = self.def_motionSpeed
    self.npc_obj.collide_player = self.def_collide_player
    self.npc_obj.collide_npc = self.def_collide_npc
    self.npc_obj.collideWithPlayers = self.def_en_collide_plr
    self.npc_obj:setSequenceLeft(self.animateWalkL)
    self.npc_obj:setSequenceRight(self.animateWalkR)
end


function dryBones:collapse()
    self.state_cur = AI_COLLAPSED
    self.cur_animation=ANIM_WALKING
    self.cur_collapsed_ticks = 0
    self.cur_wiggling_ticks = 32
    self.npc_obj.motionSpeed = 0
    self.npc_obj.speedX = 0
    self.npc_obj.collide_player = PhysBase.COLLISION_NONE
    self.npc_obj.collide_npc = PhysBase.COLLISION_NONE
    self.npc_obj.collideWithPlayers = false
    self.npc_obj:setSequenceLeft(self.animateCollapse1L)
    self.npc_obj:setSequenceRight(self.animateCollapse1R)
    Audio.playSound(57)
end

return dryBones
