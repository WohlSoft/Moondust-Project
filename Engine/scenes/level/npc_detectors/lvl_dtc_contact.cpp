#include "lvl_dtc_contact.h"
#include "../lvl_npc.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>
#include <scenes/scene_level.h>

ContactDetector::ContactDetector(LVL_Npc *parentNPC) :
    BasicDetector(parentNPC)
{

}

ContactDetector::ContactDetector(const ContactDetector &cdt)
    :BasicDetector(cdt)
{

}

ContactDetector::~ContactDetector()
{}

void ContactDetector::processDetector()
{
    if(_parentNPC->collided_center.isEmpty())
        _detected=false;
    else
    {
        _detected=true;
    }
}

bool ContactDetector::detected()
{
    return _detected;
}

luabind::adl::object ContactDetector::getBlocks(lua_State *L)
{
    luabind::object tableOfBlocks = luabind::newtable(L);
    /*
    for(LVL_Npc::PlayerColliders::iterator it=_parentNPC->collided_center.begin(); it!=_parentNPC->collided_center.end();it++)
    {

    }
    */
    return tableOfBlocks;
}

luabind::adl::object ContactDetector::getBGOs(lua_State *L)
{
    luabind::object tableOfBGOs = luabind::newtable(L);
    return tableOfBGOs;
}

luabind::adl::object ContactDetector::getNPCs(lua_State *L)
{
    luabind::object tableOfNPCs = luabind::newtable(L);
    int i = 1;
    for(LVL_Npc::PlayerColliders::iterator it=_parentNPC->collided_center.begin(); it!=_parentNPC->collided_center.end();it++)
    {
        if((*it)->type!=PGE_Phys_Object::LVLNPC) continue;
        LVL_Npc* npc=static_cast<LVL_Npc*>(*it);
        Binding_Level_ClassWrapper_LVL_NPC* possibleLuaNPC = dynamic_cast<Binding_Level_ClassWrapper_LVL_NPC*>(npc);
        if(possibleLuaNPC ){
            tableOfNPCs[i++] = possibleLuaNPC;
        }else{
            tableOfNPCs[i++] = npc;
        }
    }
    return tableOfNPCs;
}

luabind::adl::object ContactDetector::getPlayers(lua_State *L)
{
    luabind::object tableOfPlayers = luabind::newtable(L);
    return tableOfPlayers;
}

