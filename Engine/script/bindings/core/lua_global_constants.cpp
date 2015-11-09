#include "lua_global_constants.h"
#include "../../lua_defines.h"
#include "../../../scenes/scene_level.h"
#include "../../../scenes/level/lvl_npc.h"

void Binding_Global_Constants::bindToLua(lua_State *L)
{
    using namespace luabind;
    object _G = globals(L);
    _G["GAME_ENGINE"] = GAME_ENGINE;
    _G["PGE_ENGINE_VER"] = _FILE_VERSION _FILE_RELEASE;

    //Level Scene enums
    _G["GENERATOR_APPEAR"] = LevelScene::GENERATOR_APPEAR;
    _G["GENERATOR_WARP"] = LevelScene::GENERATOR_WARP;
    _G["GENERATOR_PROJECTILE"] = LevelScene::GENERATOR_WARP;

    _G["SPAWN_LEFT"] = LevelScene::SPAWN_LEFT;
    _G["SPAWN_RIGHT"] = LevelScene::SPAWN_RIGHT;
    _G["SPAWN_UP"] = LevelScene::SPAWN_UP;
    _G["SPAWN_DOWN"] = LevelScene::SPAWN_DOWN;

    //NPC Class enums
    _G["NPC_DAMAGE_NOREASON"] =       LVL_Npc::DAMAGE_NOREASON;
    _G["NPC_DAMAGE_STOMPED"] =        LVL_Npc::DAMAGE_STOMPED;
    _G["NPC_DAMAGE_BY_KICK"] =        LVL_Npc::DAMAGE_BY_KICK;
    _G["NPC_DAMAGE_BY_PLAYER_ATTACK"]=LVL_Npc::DAMAGE_BY_PLAYER_ATTACK;
    _G["NPC_DAMAGE_TAKEN"] =          LVL_Npc::DAMAGE_TAKEN;
    _G["NPC_DAMAGE_CUSTOM_REASON"] =  LVL_Npc::DAMAGE_CUSTOM_REASON;

    _G["NPC_WARP_TOP"] =    LVL_Npc::WARP_TOP;
    _G["NPC_WARP_LEFT"] =   LVL_Npc::WARP_LEFT;
    _G["NPC_WARP_BOTTOM"] = LVL_Npc::WARP_BOTTOM;
    _G["NPC_WARP_RIGHT"] =  LVL_Npc::WARP_RIGHT;

}

