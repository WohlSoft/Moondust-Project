#ifndef CONTACTDETECTOR_H
#define CONTACTDETECTOR_H

#include "lvl_base_detector.h"

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class ContactDetector : public BasicDetector
{
public:
    ContactDetector(LVL_Npc* parentNPC);
    ContactDetector(const ContactDetector &cdt);
    ~ContactDetector();
    void processDetector();
    bool detected();

    luabind::object getBlocks(lua_State *L);
    luabind::object getBGOs(lua_State *L);
    luabind::object getNPCs(lua_State *L);
    luabind::object getPlayers(lua_State *L);
};

#endif // CONTACTDETECTOR_H
