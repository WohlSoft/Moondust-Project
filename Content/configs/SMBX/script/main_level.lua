
-- utils
smbx_utils = require("utils/smbx_utils")

-- P-Switch implementation
pSwitch = require("utils/p-switch")

-- ID's of switches
GREEN_SW  = 1
RED_SW    = 2
BLUE_SW   = 3
YELLOW_SW = 4

-- Increase player health
function plr_healthUp(plr)
    if( plr.health < 3 )then
        if( (plr.characterID > 2) and (plr.health == 2) )then
                plr.health = 3
        else
                plr.health = 2
        end
    end
end

function onLoop(tickTime)
    -- Renderer.printText("Oh, shit! "..tostring(tickTime), 10, 10, 0, 15, 0xFF0000FF)
    pSwitch.process(tickTime)
end

-- Processes player powerup system
function ProcessPlayerPowerUP(plr, npc)
    if( npc_isRadish(npc.id) )then
        plr_healthUp(plr)
        if(plr.stateID == 1 )then
            plr:setState(2)
            Audio.playSoundByRole(SoundRoles.PlayerGrow)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    elseif( npc_isFirePlant(npc.id))then
        plr_healthUp(plr)
        if(plr.stateID ~= 3 )then
            plr:setState(3)
            Audio.playSoundByRole(SoundRoles.PlayerGrow)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    elseif( npc_isIcePlant(npc.id))then
        plr_healthUp(plr)
        if(plr.stateID ~= 7 )then
            plr:setState(7)
            Audio.playSoundByRole(SoundRoles.PlayerGrow)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    elseif( npc.id == 34 )then -- Leaf
        plr_healthUp(plr)
        if(plr.stateID ~= 4 )then
            plr:setState(4)
            Audio.playSoundByRole(SoundRoles.PlayerMagic)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    elseif( npc.id == 169 )then -- Tanooki
        plr_healthUp(plr)
        if(plr.stateID ~= 5)then
            plr:setState(5)
            Audio.playSoundByRole(SoundRoles.PlayerMagic)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    elseif( npc.id == 170 )then -- Hammer
        plr_healthUp(plr)
        if(plr.stateID ~= 6)then
            plr:setState(6)
            Audio.playSoundByRole(SoundRoles.PlayerMagic)
            plr:setInvincible(true, 1500, true)
        else
            Audio.playSoundByRole(SoundRoles.PlayerTakeItem)
        end
    end
end

-- Process harm event
function processPlayerHarm(plr, harmEvent)
    local newHealth = plr.health-harmEvent.damage
    if(newHealth==1)then
        plr:setState(1)
        Audio.playSoundByRole(SoundRoles.PlayerShrink)
    else
        plr:setState(2)
        Audio.playSoundByRole(SoundRoles.PlayerHarm)
    end
end

-- is block a bricks?
function block_isBricks(blockID)
    if(blockID==4)then
        return true
    elseif(blockID==188)then
        return true
    elseif(blockID==60)then
        return true
    elseif(blockID==90)then
        return true
    end
    return false
end


-- is NPC a coin?
function npc_isCoin(npcID)
    if(npcID==10)then
        return true
    elseif(npcID==33)then
        return true
    elseif(npcID==88)then
        return true
    elseif(npcID==103)then
        return true
    elseif(npcID==138)then
        return true
    elseif(npcID==152)then
        return true
    elseif((npcID>=251) and (npcID<=253))then
        return true
    elseif(npcID==258)then
        return true
    elseif(npcID==274)then
        return true
    end
    return false
end

-- Is NPC a Radish?
function npc_isRadish(npcID)
    if(npcID==9)then
        return true
    elseif(npcID==184)then
        return true
    elseif(npcID==185)then
        return true
    elseif(npcID==249)then
        return true
    end
    return false
end

-- Is NPC a Fire Plant?
function npc_isFirePlant(npcID)
    if(npcID==14)then
        return true
    elseif(npcID==182)then
        return true
    elseif(npcID==183)then
        return true
    end
    return false
end

-- Is NPC an Ice Plant?
function npc_isIcePlant(npcID)
    if(npcID==264)then
        return true
    elseif(npcID==277)then
        return true
    end
    return false
end

-- Is NPC a Fire Plant?
function npc_isShell(npcID)
    if(npcID==5)then
        return true
    elseif(npcID==7)then
        return true
    elseif(npcID==24)then
        return true
    elseif(npcID==73)then
        return true
    elseif((npcID>=113)and(npcID<=116))then
        return true
    elseif(npcID==172)then
        return true
    elseif(npcID==174)then
        return true
    --elseif(npcID==195)then
    --    return true
    end
    return false
end

