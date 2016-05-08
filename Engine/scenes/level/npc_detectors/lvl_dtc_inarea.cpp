#include "lvl_dtc_inarea.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>
#include <script/bindings/level/classes/luaclass_level_lvl_player.h>
#include "../../scene_level.h"

InAreaDetector::InAreaDetector(LVL_Npc *parent, PGE_RectF range, int filters)
    : BasicDetector(parent)
{
    _filters=filters;
    _srcTrapZone=range;
    _trapZone=range;
    _contacts=0;
}

InAreaDetector::InAreaDetector(const InAreaDetector &dtc) :
    BasicDetector(dtc)
{
    _filters=dtc._filters;
    _srcTrapZone=dtc._srcTrapZone;
    _trapZone=dtc._trapZone;
    _contacts=dtc._detected;
    detectedPLR=dtc.detectedPLR;
    detectedBLK=dtc.detectedBLK;
    detectedBGO=dtc.detectedBGO;
    detectedNPC=dtc.detectedNPC;
}

InAreaDetector::~InAreaDetector()
{}

void InAreaDetector::processDetector()
{
    _contacts = 0;
    _detected=false;
    _trapZone=_srcTrapZone;

    _trapZone.setPos(
                _parentNPC->posCenterX()+_srcTrapZone.left(),
                _parentNPC->posCenterY()+_srcTrapZone.top()
                );

    if( _parentNPC->direction() < 0 )
    {
        double x_pos = _trapZone.left() + (_parentNPC->posCenterX() - _trapZone.center().x())*2.0;
        _trapZone.setPos( x_pos, _trapZone.y() );
    }

    detectedPLR.clear();
    detectedBLK.clear();
    detectedBGO.clear();
    detectedNPC.clear();
    detectedBlocks.clear();
    detectedBGOs.clear();
    detectedNPCs.clear();
    detectedPlayers.clear();

    QVector<PGE_Phys_Object*> bodies;
    _scene->queryItems(_trapZone, &bodies);

    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*visibleBody=*it;
        if(visibleBody==_parentNPC) continue;
        if(visibleBody==NULL)
            continue;
        if(!visibleBody->isVisible()) //Don't whip elements from hidden layers!
            continue;
        switch(visibleBody->type)
        {
            case PGE_Phys_Object::LVLPlayer: if((_filters&F_PLAYER)==0) continue;
                {
                    LVL_Player* p= dynamic_cast<LVL_Player*>(visibleBody);
                    if(!p) continue;
                    detectedPlayers.push_back(p);
                    detectedPLR[p->data.id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLBGO:    if((_filters&F_BGO)==0) continue;
                {
                    LVL_Bgo* b= dynamic_cast<LVL_Bgo*>(visibleBody);
                    if(!b) continue;
                    detectedBGOs.push_back(b);
                    detectedBGO[b->data.id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLNPC:    if((_filters&F_NPC)==0) continue;
                {
                    LVL_Npc* n= dynamic_cast<LVL_Npc*>(visibleBody);
                    if(!n) continue;
                    detectedNPCs.push_back(n);
                    detectedNPC[n->_npc_id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLBlock:  if((_filters&F_BLOCK)==0) continue;
                {
                    LVL_Block* s= dynamic_cast<LVL_Block*>(visibleBody);
                    if(!s) continue;
                    if(!s->destroyed)
                        detectedBlocks.push_back(s);
                    detectedBLK[s->data.id]=1;
                    break;
                }
            default:continue;
        }
        _detected=true;
        _contacts++;
    }
}

bool InAreaDetector::detected()
{
    return _detected;
}

bool InAreaDetector::detected(long type, long ID)
{
    if(!_detected) return false;
    if(ID==0)
    {
        switch(type)
        {
            case 1: return !detectedBLK.isEmpty(); break;
            case 2: return !detectedBGO.isEmpty(); break;
            case 3: return !detectedNPC.isEmpty(); break;
            case 4: return !detectedPLR.isEmpty(); break;
            default:break;
        }
    }
    else
    {
        switch(type)
        {
            case 1: return detectedBLK.contains(ID); break;
            case 2: return detectedBGO.contains(ID); break;
            case 3: return detectedNPC.contains(ID); break;
            case 4: return detectedPLR.contains(ID); break;
            default:break;
        }
    }
    return false;
}

int InAreaDetector::contacts()
{
    return _contacts;
}

PGE_RectF InAreaDetector::trapZone()
{
    return _trapZone;
}

luabind::adl::object InAreaDetector::getBlocks(lua_State *L)
{
    luabind::object tableOfBlocks = luabind::newtable(L);
    int i = 1;
    for(QList<LVL_Block*>::iterator it=detectedBlocks.begin(); it != detectedBlocks.end();it++)
    {
        if((*it)->type!=PGE_Phys_Object::LVLBlock) continue;
        LVL_Block* block=static_cast<LVL_Block*>(*it);
        if( block )
        {
            tableOfBlocks[i++] = block;
        }
    }
    return tableOfBlocks;
}

luabind::adl::object InAreaDetector::getBGOs(lua_State *L)
{
    luabind::object tableOfBGOs = luabind::newtable(L);
    int i = 1;
    for(QList<LVL_Bgo*>::iterator it = detectedBGOs.begin(); it != detectedBGOs.end();it++)
    {
        if((*it)->type!=PGE_Phys_Object::LVLBGO) continue;
        LVL_Bgo* bgo=static_cast<LVL_Bgo*>(*it);
        if( bgo ){
            tableOfBGOs[i++] = bgo;
        }
    }
    return tableOfBGOs;
}

luabind::adl::object InAreaDetector::getNPCs(lua_State *L)
{
    luabind::object tableOfNPCs = luabind::newtable(L);
    int i = 1;
    for(QList<LVL_Npc*>::iterator it = detectedNPCs.begin(); it != detectedNPCs.end(); it++)
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

luabind::adl::object InAreaDetector::getPlayers(lua_State *L)
{
    luabind::object tableOfPlayers = luabind::newtable(L);
    int i = 1;
    for(QList<LVL_Player*>::iterator it = detectedPlayers.begin(); it != detectedPlayers.end(); it++)
    {
        if((*it)->type!=PGE_Phys_Object::LVLPlayer) continue;
        LVL_Player* player = static_cast<LVL_Player*>(*it);
        Binding_Level_ClassWrapper_LVL_Player* possibleLuaPlayer = dynamic_cast<Binding_Level_ClassWrapper_LVL_Player*>(player);
        if(possibleLuaPlayer){
            tableOfPlayers[i++] = possibleLuaPlayer;
        }else{
            tableOfPlayers[i++] = player;
        }
    }
    return tableOfPlayers;
}


