class 'pswitch'

function pswitch:__init(npc_obj)
    self.npc_obj = npc_obj
end

function pswitch:onKill(killEvent)
    Level.shakeScreenY(10.0, 0.02)
    Audio.playSoundByRole(SoundRoles.BlockSwitch)
    pSwitch.pstart()
end

return pswitch

