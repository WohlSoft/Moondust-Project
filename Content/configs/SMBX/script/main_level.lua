-- utils
smbx_utils = require("utils/smbx_utils")

-- Item Types (Is coin, shell, radish, fire plant, etc.) Checker
itemTypes = require("utils/item_types")

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

local gfx_itemslot = Graphics.loadImage( Paths.CommonGFX().."itemslot.png" )
local gfx_heart0   = Graphics.loadImage( Paths.CommonGFX().."heart_n.png" )
local gfx_heart1   = Graphics.loadImage( Paths.CommonGFX().."heart.png" )
local gfx_1up      = Graphics.loadImage( Paths.CommonGFX().."1up.png" )
local gfx_x        = Graphics.loadImage( Paths.CommonGFX().."ex.png" )
local gfx_star     = Graphics.loadImage( Paths.CommonGFX().."stars.png" )
local gfx_coin     = Graphics.loadImage( Paths.CommonGFX().."coins.png" )

function onDrawHUD(camera, state)
    if(state.characterID >=3)then
        local hp1 = gfx_heart1;
        local hp0 = gfx_heart0;
        for i=0,2,1
        do
            local hp = nil
            if(state.health >= (i + 1))then
                hp = hp1;
            else
                hp = hp0;
            end
            Graphics.drawImage(hp, camera.centerX - 42 + (32 * i), camera.renderY + 16)
        end
    else
        Graphics.drawImage(gfx_itemslot, camera.centerX-(gfx_itemslot.w/2.0), camera.renderY+16 )
    end

    Graphics.drawImage(gfx_1up,  camera.centerX-165, camera.renderY+26 )
    Graphics.drawImage(gfx_x,    camera.centerX-125, camera.renderY+27 )
    Renderer.printText(tostring(state.lives), camera.centerX - 103, camera.renderY+27, 3, 15, 0xFFFFFFFF)
    
    if( state.leeks ~= 0 ) then
        Graphics.drawImage(gfx_x,    camera.centerX - 125, camera.renderY+47 )
        Graphics.drawImage(gfx_star, camera.centerX - 149, camera.renderY+46 )
        Renderer.printText(tostring(state.leeks), camera.centerX - 103, camera.renderY+47, 3, 15, 0xFFFFFFFF)
    end
    
    Graphics.drawImage(gfx_x,    camera.centerX+112, camera.renderY+27 )
    Graphics.drawImage(gfx_coin, camera.centerX+88, camera.renderY + 26 )
    Renderer.printText(tostring(state.coins), camera.centerX + 152, camera.renderY + 27, 3, 15, 0xFFFFFFFF)

    Renderer.printText(tostring(state.points), camera.centerX + 152, camera.renderY + 47, 3, 15, 0xFFFFFFFF)
end

function onLoop(tickTime)
    -- Renderer.printText("A tiny test: time of tick is: "..tostring(tickTime), 10, 10, 0, 15, 0xFF0000FF)
    pSwitch.process(tickTime)
end

-- Processes player powerup system
function ProcessPlayerPowerUP(plr, npc)
    if(npc_isCoin(npc.id))then
        plr.globalState:addCoins(1)
        if(plr.globalState.coins >= 100)then
            plr.globalState:setCoins( plr.globalState.coins-100 )
            plr.globalState:addLives(1)
            Audio.playSoundByRole(SoundRoles.Bonus1up)
            if(plr.globalState.lives>99)then
                plr.globalState:setLives(99)
            end
        end
    elseif( npc_isLife(npc.id) )then
        Audio.playSoundByRole(SoundRoles.Bonus1up)
        plr.globalState:addLives(1)
        if(plr.globalState.lives>99)then
            plr.globalState:setLives(99)
        end
    elseif( npc.id==188 )then
        Audio.playSoundByRole(SoundRoles.Bonus1up)
        plr.globalState:addLives(3)
        if(plr.globalState.lives>99)then
            plr.globalState:setLives(99)
        end
    elseif( npc_isRadish(npc.id) )then
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

function ShootFireball(plr)
    local thrownNPC=plr:spawnNPC(13, GENERATOR_APPEAR, SPAWN_UP, false)
    if(thrownNPC~=nil)then
        thrownNPC.motionSpeed = 5 + math.abs(plr.speedX)
        thrownNPC.direction = plr.direction
        if(plr:getKeyState(KEY_UP))then
            thrownNPC.speedY = -4
        else
            thrownNPC.speedY = 3
        end
        thrownNPC.center_x = plr.center_x + plr.direction*(plr.width/2)
        thrownNPC.center_y = plr.center_y-16
        thrownNPC.special1 = 0
        thrownNPC.controller:setCharacter( plr.characterID )
        Audio.playSound(18)
    end
end

function ShootHammer(plr)
    local thrownNPC=plr:spawnNPC(171, GENERATOR_APPEAR, SPAWN_UP, false)
    if(thrownNPC~=nil)then
        thrownNPC.direction = plr.direction
        if(plr:getKeyState(KEY_UP))then
            thrownNPC.speedY = -7
            thrownNPC.motionSpeed = 5 + math.abs(plr.speedX)
        else
            thrownNPC.speedY = -3
            thrownNPC.motionSpeed = 5 + math.abs(plr.speedX)
        end
        thrownNPC.center_x = plr.center_x + plr.direction*(plr.width/2)
        thrownNPC.center_y = plr.center_y-16
        thrownNPC.special1 = 0
        -- thrownNPC.controller:setCharacter( plr.characterID )
        Audio.playSound(18)
    end
end


-- is NPC a coin?
function npc_isCoin(npcID)
    return itemTypes.isNpcCoin(npcID)
end

-- Is NPC a Radish?
function npc_isRadish(npcID)
    return itemTypes.isNpcRadish(npcID)
end

-- is NPC a life?
function npc_isLife(npcID)
    return itemTypes.isNpcLife(npcID)
end

-- Is NPC a Fire Plant?
function npc_isFirePlant(npcID)
    return itemTypes.isNpcFirePlant(npcID)
end

-- Is NPC an Ice Plant?
function npc_isIcePlant(npcID)
    return itemTypes.isNpcIcePlant(npcID)
end
-- Is NPC a Blue Goopa?
function npc_isBloeGoopa(npcID)
    return itemTypes.isBlueGoopa(npcID)
end

-- Is NPC a Shell?
function npc_isShell(npcID)
    return itemTypes.isNpcShell(npcID)
end

-- is block a bricks?
function block_isBricks(blockID)
    return itemTypes.isBlockBrick(npcID)
end

