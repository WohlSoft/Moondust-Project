local p_switch = {}

local p_switch_max_time = 14000.0
local p_switch_time_left = 0.0
local p_switch_enabled = false;

function p_switch.toggle()
    local allNPCs = NPC.get();
    local allBlocks = Block.get();

    for K,Blk in pairs(allBlocks)do
        if( (not Blk.is_destroyed) and (Blk.contentNPC_ID == 0) )then
            if(Blk.id==4)then
                Blk:transformTo(10, 1)
            elseif(Blk.id == 188)then
                Blk:transformTo(88, 1)
            elseif(Blk.id == 60)then
                Blk:transformTo(88, 1)
            elseif(Blk.id == 89)then
                Blk:transformTo(33, 1)
            end
        end
    end

    for K,Npc in pairs(allNPCs)do
        if(Npc.id==10)then
            Npc:transformTo(4, 2)
        elseif(Npc.id==33)then
            Npc:transformTo(89, 2)
        elseif(Npc.id==258)then
            Npc:transformTo(89, 2)
        elseif(Npc.id==88)then
            Npc:transformTo(60, 2)
        elseif(Npc.id==103)then
            Npc:transformTo(4, 2)
        end
    end
    
end

-- Stop p-switch and transform all coins into brick blocks and brick blocks into coins
function p_switch.pstart()
    p_switch_time_left = p_switch_max_time
    if(not p_switch_enabled)then
        p_switch_enabled=true
        p_switch.toggle()
    end
end

-- Stop p-switch and transform all objects back
function p_switch.pend()
    p_switch_time_left = 0.0
    if(p_switch_enabled)then
        p_switch_enabled=false
        p_switch.toggle()
    end
end

-- Process P-Switch timer
function p_switch.process(tickTime)
    if(not p_switch_enabled)then
        return
    end
    Renderer.printText("P-SWITCH LEFT "..tostring(p_switch_time_left), 10, 10, 0, 15, 0xFF0000FF)
    p_switch_time_left = p_switch_time_left - tickTime
    if(p_switch_time_left<0.0)then
        p_switch.pend()
    end
end

return p_switch

