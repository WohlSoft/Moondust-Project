#include "lua_global_constants.h"
#include "../../lua_defines.h"
#include "../../../scenes/scene_level.h"
#include "../../../scenes/level/lvl_npc.h"

void Binding_Global_Constants::bindToLua(lua_State *L)
{
    using namespace luabind;
    object _G = globals(L);
    /***
    Globally visible constants of engine.<br>
    <b>To use them you don't need use module name.</b>
    @module GlobalConstants
    */

    /***
    @section Common
    */

    /***
    Game engine identifier. Can be used to detect PGE Engine ("PGE_Engine") or LunaLua ("LunaLua").
    @field GAME_ENGINE "PGE Engine"
    */
    _G["GAME_ENGINE"] = GAME_ENGINE;

    /***
    Version of PGE Engine. Version is stored in next format:<br>X.Y.Z.P<br>
    <ul>
        <li>X - major version</li>
        <li>Y - minor version</li>
        <li>Z and P - patches of various level</li>
    <ul>
    @field PGE_ENGINE_VER "X.Y.Z.P"
    */
    _G["PGE_ENGINE_VER"] = V_FILE_VERSION V_FILE_RELEASE;

    /***
    Generator algorithms
    @section GENERATOR_TYPE
    */
    //Level Scene enums
    /***
    "Appear" spawn algorithm.
    @field GENERATOR_APPEAR 0
    */
    _G["GENERATOR_APPEAR"] = LevelScene::GENERATOR_APPEAR;
    /***
    "Warp" spawn algorithm.
    @field GENERATOR_WARP 1
    */
    _G["GENERATOR_WARP"] = LevelScene::GENERATOR_WARP;
    /***
    "Projectile shoot" spawn algorithm.
    @field GENERATOR_PROJECTILE 2
    */
    _G["GENERATOR_PROJECTILE"] = LevelScene::GENERATOR_PROJECTILE;


    /***
    Algorithm of object spawn
    @section SPAWN_TYPE
    */

    /***
    Spawn object into left direction.
    @field SPAWN_LEFT 2
    */
    _G["SPAWN_LEFT"] = LevelScene::SPAWN_LEFT;
    /***
    Spawn object into right direction.
    @field SPAWN_RIGHT 4
    */
    _G["SPAWN_RIGHT"] = LevelScene::SPAWN_RIGHT;
    /***
    Spawn object in up direction.
    @field SPAWN_UP 1
    */
    _G["SPAWN_UP"] = LevelScene::SPAWN_UP;
    /***
    Spawn object into down direction.
    @field SPAWN_DOWN 3
    */
    _G["SPAWN_DOWN"] = LevelScene::SPAWN_DOWN;

    /***
    Reason of NPC damage
    @section NPC_DAMAGE_REASON
    */

    //NPC Class enums

    /***
    No reason of damage
    @field NPC_DAMAGE_NOREASON 0
    */
    _G["NPC_DAMAGE_NOREASON"] =       LVL_Npc::DAMAGE_NOREASON;
    /***
    NPC was stomped by player or by heavy object
    @field NPC_DAMAGE_STOMPED 1
    */
    _G["NPC_DAMAGE_STOMPED"] =        LVL_Npc::DAMAGE_STOMPED;
    /***
    NPC got a kick attack
    @field NPC_DAMAGE_BY_KICK 2
    */
    _G["NPC_DAMAGE_BY_KICK"] =        LVL_Npc::DAMAGE_BY_KICK;
    /***
    NPC attacked by player's beat
    @field NPC_DAMAGE_BY_KICK 3
    */
    _G["NPC_DAMAGE_BY_PLAYER_ATTACK"]=LVL_Npc::DAMAGE_BY_PLAYER_ATTACK;
    /***
    NPC is a takable object which was taken (for example, coin, power-up, equipment, weapon, etc.)
    @field NPC_DAMAGE_TAKEN 4
    */
    _G["NPC_DAMAGE_TAKEN"] =          LVL_Npc::DAMAGE_TAKEN;
    /***
    Custom damage reason. Engine doesn't uses any reasons past of this constant.
    @field NPC_DAMAGE_CUSTOM_REASON 5
    */
    _G["NPC_DAMAGE_CUSTOM_REASON"] =  LVL_Npc::DAMAGE_CUSTOM_REASON;

    /***
    Direction of NPC appearence from the warp
    @section NPC_WARP_DIRECTION
    */

    /***
    NPC is warping up
    @field NPC_WARP_TOP 1
    */
    _G["NPC_WARP_TOP"] =    LVL_Npc::WARP_TOP;
    /***
    NPC is warping left
    @field NPC_WARP_LEFT 2
    */
    _G["NPC_WARP_LEFT"] =   LVL_Npc::WARP_LEFT;
    /***
    NPC is warping down
    @field NPC_WARP_BOTTOM 3
    */
    _G["NPC_WARP_BOTTOM"] = LVL_Npc::WARP_BOTTOM;
    /***
    NPC is warping right
    @field NPC_WARP_RIGHT 4
    */
    _G["NPC_WARP_RIGHT"] =  LVL_Npc::WARP_RIGHT;

    /***
    Key codes of player controller
    @section PLAYER_KEY_CODES
    */

    //Key codes

    /***
    A "Start" key
    @field KEY_START 0
    */
    _G["KEY_START"] = ControllableObject::KEY_START;
    /***
    "Drop" or "Select" key
    @field KEY_DROP 1
    */
    _G["KEY_DROP"] = ControllableObject::KEY_DROP;
    /***
    "Jump" key
    @field KEY_JUMP 7
    */
    _G["KEY_JUMP"] = ControllableObject::KEY_JUMP;
    /***
    "Run" key
    @field KEY_RUN 6
    */
    _G["KEY_RUN"] = ControllableObject::KEY_RUN;
    /***
    "Alt-Run" key
    @field KEY_ALT_RUN 8
    */
    _G["KEY_ALT_RUN"] = ControllableObject::KEY_ALT_RUN;
    /***
    "Alt-Jump" key
    @field KEY_ALT_JUMP 9
    */
    _G["KEY_ALT_JUMP"] = ControllableObject::KEY_ALT_JUMP;
    /***
    "Left" key
    @field KEY_LEFT 2
    */
    _G["KEY_LEFT"] = ControllableObject::KEY_LEFT;
    /***
    "Right" key
    @field KEY_RIGHT 3
    */
    _G["KEY_RIGHT"] = ControllableObject::KEY_RIGHT;
    /***
    "Up" key
    @field KEY_UP 4
    */
    _G["KEY_UP"] = ControllableObject::KEY_UP;
    /***
    "Down" key
    @field KEY_DOWN 5
    */
    _G["KEY_DOWN"] = ControllableObject::KEY_DOWN;

    //Aliases for LunaLUA compatibility

    /***
    "Drop" key, <b>[Alias for LunaLua compatibility]</b>
    @field KEY_STR @{KEY_START}
    */
    _G["KEY_STR"] = ControllableObject::KEY_START;
    /***
    "Drop" key, <b>[Alias for LunaLua compatibility]</b>
    @field KEY_SEL @{KEY_DROP}
    */
    _G["KEY_SEL"] = ControllableObject::KEY_DROP;
    /***
    "Alt-Jump" key, <b>[Alias for LunaLua compatibility]</b>
    @field KEY_SPINJUMP @{KEY_ALT_JUMP}
    */
    _G["KEY_SPINJUMP"] = ControllableObject::KEY_ALT_JUMP;
    /***
    "Alt-Run" key, <b>[Alias for LunaLua compatibility]</b>
    @field KEY_X @{KEY_ALT_RUN}
    */
    _G["KEY_X"] = ControllableObject::KEY_ALT_RUN;
}

