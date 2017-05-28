local p_switch = {}

local p_switch_max_time = 14000.0
local p_switch_time_left = 0.0
local p_switch_enabled = false;

function p_switch.toggle()
    local allNPCs = NPC.get();
    local allBlocks = Block.get();

    for K,Blk in pairs(allBlocks)do
        if( (Blk.visible) and (not Blk.is_destroyed) and (Blk.contentNPC_ID == 0) )then
            if(Blk.id==4)then
                if(Blk.transformedFromNpcID==251)then
                    Blk:transformTo(251, 1)
                elseif(Blk.transformedFromNpcID==252)then
                    Blk:transformTo(252, 1)
                elseif(Blk.transformedFromNpcID==253)then
                    Blk:transformTo(253, 1)
                else
                    Blk:transformTo(10, 1)
                end
            elseif(Blk.id == 188)then
                Blk:transformTo(88, 1)
            elseif(Blk.id == 60)then
                Blk:transformTo(88, 1)
            elseif(Blk.id == 89)then
                if(Blk.transformedFromNpcID==258)then
                    Blk:transformTo(258, 1)
                else
                    Blk:transformTo(33, 1)
                end
            elseif(Blk.id == 280)then
                Blk:transformTo(103, 1)
            elseif(Blk.id == 293)then
                Blk:transformTo(138, 1)
            end
        end
    end

    for K,Npc in pairs(allNPCs)do
        if(Npc.visible)then
            if(Npc.id==10)then
                Npc:transformTo(4, 2)
            elseif(Npc.id==33)then
                Npc:transformTo(89, 2)
            elseif(Npc.id==258)then
                Npc:transformTo(89, 2)
            elseif(Npc.id==88)then
                if(Npc.transformedFromBlockID==60)then
                    Npc:transformTo(60, 2)
                else
                    Npc:transformTo(188, 2)
                end
            elseif(Npc.id==103)then
                Npc:transformTo(280, 2)
            elseif(Npc.id==138)then
                Npc:transformTo(293, 2)
            elseif(Npc.id==251)then
                Npc:transformTo(4, 2)
            elseif(Npc.id==252)then
                Npc:transformTo(4, 2)
            elseif(Npc.id==253)then
                Npc:transformTo(4, 2)
            end
        end
    end
    
end

-- Stop p-switch and transform all coins into brick blocks and brick blocks into coins
function p_switch.pstart()
    p_switch_time_left = p_switch_max_time
    if(not p_switch_enabled)then
        p_switch_enabled=true
        p_switch.toggle()
        Level.triggerEvent("P Switch - Start")
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
    -- Trigger stop even on last frame of P-Switch time. Because real triggering of event will happen in next frame
    if((p_switch_time_left <= tickTime) and (p_switch_time_left >= 0.0))then
        Level.triggerEvent("P Switch - End")
    end
    -- Stop P-Switch processing
    if(p_switch_time_left<0.0)then
        p_switch.pend()
    end
end

return p_switch

