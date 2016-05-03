class 'pswitch'

function pswitch:__init(npc_obj)
    self.npc_obj = npc_obj
end

function pswitch:onKill(killEvent)
    pSwitch.pstart()
end

return pswitch

