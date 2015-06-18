#ifndef BINDING_CORE_GLOBALFUNCS_AUDIO_H
#define BINDING_CORE_GLOBALFUNCS_AUDIO_H


#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_GlobalFuncs_Audio
{
public:
    static void playSound(long soundID);
    static void playSoundByRole(int role);

    static luabind::scope bindToLua();
    static void bindConstants(lua_State* L);
};

#endif // BINDING_CORE_GLOBALFUNCS_AUDIO_H
