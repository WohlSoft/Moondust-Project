class 'pswitch'

function pswitch:__init(npc_obj)
    self.npc_obj = npc_obj
end

function pswitch:onKill(killEvent)
    Level.ShakeScreenY(15.0, 0.1)
    pSwitch.pstart()
end

return pswitch

