local itemTypes = {}

local npc_coins       = { 10, 33, 88, 103, 138, 152, 251, 252, 253, 258, 274 }
local npc_radishes    = { 9, 184, 185, 249 }
local npc_fire_plants = { 14, 182, 183 }
local npc_ice_plants  = { 264, 277 }
local npc_lives       = { 90, 186, 187 }
local npc_shells      = { 5, 7, 24, 73, 113, 114, 115, 116, 172, 174 } --Scrabbed: 195
local npc_blueGoopas  = { 55, 119 }

local block_bricks    = { 4, 188, 60, 90 }

function itemTypes.tableContainsValue(tbl, val)
  for i = 1, #tbl do
    if(tbl[i] == val)then
      return true
    end
  end
  return false
end

function itemTypes.isNpcCoin(npcID)
    return itemTypes.tableContainsValue(npc_coins, npcID)
end

function itemTypes.isNpcRadish(npcID)
    return itemTypes.tableContainsValue(npc_radishes, npcID)
end

function itemTypes.isNpcFirePlant(npcID)
    return itemTypes.tableContainsValue(npc_fire_plants, npcID)
end

function itemTypes.isNpcLife(npcID)
    return itemTypes.tableContainsValue(npc_lives, npcID)
end

function itemTypes.isNpcIcePlant(npcID)
    return itemTypes.tableContainsValue(npc_ice_plants, npcID)
end

function itemTypes.isBlueGoopa(npcID)
    return itemTypes.tableContainsValue(npc_blueGoopas, npcID)
end

function itemTypes.isNpcShell(npcID)
    return itemTypes.tableContainsValue(npc_shells, npcID)
end

function itemTypes.isBlockBrick(npcID)
    return itemTypes.tableContainsValue(block_bricks, npcID)
end

return itemTypes
