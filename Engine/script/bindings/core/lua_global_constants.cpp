#include "lua_global_constants.h"
#include "../../lua_defines.h"

void Binding_Global_Constants::bindToLua(lua_State *L)
{
    using namespace luabind;
    object _G = globals(L);
    _G["GAME_ENGINE"] = GAME_ENGINE;
    _G["PGE_ENGINE_VER"] = _FILE_VERSION _FILE_RELEASE;
}

