#include "lvl_dtc_contact.h"
#include "../lvl_npc.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>
#include <script/bindings/level/classes/luaclass_level_lvl_player.h>
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
    _detected = !(  _parentNPC->contacted_blocks.empty()&&
                    _parentNPC->contacted_bgos.empty()&&
                    _parentNPC->contacted_npc.empty()&&
                    _parentNPC->contacted_players.empty() );
}

bool ContactDetector::detected()
{
    return _detected;
}

luabind::adl::object ContactDetector::getBlocks(lua_State *L)
{
    luabind::object tableOfBlocks = luabind::newtable(L);
    int i = 1;
    for(LVL_Npc::CollisionTable::iterator it=_parentNPC->contacted_blocks.begin();
        it!=_parentNPC->contacted_blocks.end();
        it++)
    {
        PGE_Phys_Object *obj = *it;
        if(obj->type != PGE_Phys_Object::LVLBlock)
            continue;
        LVL_Block* block = static_cast<LVL_Block*>(obj);
        if(block)
            tableOfBlocks[i++] = block;
    }
    return tableOfBlocks;
}

luabind::adl::object ContactDetector::getBGOs(lua_State *L)
{
    luabind::object tableOfBGOs = luabind::newtable(L);
    int i = 1;
    for(LVL_Npc::CollisionTable::iterator it=_parentNPC->contacted_bgos.begin();
        it!=_parentNPC->contacted_bgos.end();
        it++)
    {
        PGE_Phys_Object *obj = *it;
        if(obj->type != PGE_Phys_Object::LVLBGO)
            continue;
        LVL_Bgo* bgo = static_cast<LVL_Bgo*>(obj);
        if(bgo)
            tableOfBGOs[i++] = bgo;
    }
    return tableOfBGOs;
}

luabind::adl::object ContactDetector::getNPCs(lua_State *L)
{
    luabind::object tableOfNPCs = luabind::newtable(L);
    int i = 1;
    for(LVL_Npc::CollisionTable::iterator it=_parentNPC->contacted_npc.begin();
        it != _parentNPC->contacted_npc.end();
        it++)
    {
        PGE_Phys_Object *obj = *it;
        if(obj->type != PGE_Phys_Object::LVLNPC)
            continue;
        LVL_Npc* npc = static_cast<LVL_Npc*>(obj);
        Binding_Level_ClassWrapper_LVL_NPC* possibleLuaNPC = dynamic_cast<Binding_Level_ClassWrapper_LVL_NPC*>(npc);
        if(possibleLuaNPC )
            tableOfNPCs[i++] = possibleLuaNPC;
        else
            tableOfNPCs[i++] = npc;
    }
    return tableOfNPCs;
}

luabind::adl::object ContactDetector::getPlayers(lua_State *L)
{
    luabind::object tableOfPlayers = luabind::newtable(L);
    int i = 1;
    for(LVL_Npc::CollisionTable::iterator it=_parentNPC->contacted_players.begin();
        it!=_parentNPC->contacted_players.end();
        it++)
    {
        PGE_Phys_Object *obj = *it;
        if(obj->type != PGE_Phys_Object::LVLPlayer)
            continue;
        LVL_Player* player = static_cast<LVL_Player*>(obj);
        Binding_Level_ClassWrapper_LVL_Player* possibleLuaPlayer = dynamic_cast<Binding_Level_ClassWrapper_LVL_Player*>(player);
        if(possibleLuaPlayer)
            tableOfPlayers[i++] = possibleLuaPlayer;
        else
            tableOfPlayers[i++] = player;
    }
    return tableOfPlayers;
}

