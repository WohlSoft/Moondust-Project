#ifndef PLAYERINAREADETECTOR_H
#define PLAYERINAREADETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/rectf.h>
#include <QHash>
#include <QList>

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class LevelScene;
class LVL_Block;
class LVL_Bgo;
class LVL_Npc;
class LVL_Player;

class InAreaDetector : public BasicDetector
{
public:
    enum Filters
    {
        F_BLOCK=  1,
        F_BGO=    2,
        F_NPC=    4,
        F_PLAYER= 8
    };
    InAreaDetector(LVL_Npc * parent, PGE_RectF range, int filters);
    InAreaDetector(const InAreaDetector &dtc);
    ~InAreaDetector();
    void processDetector();
    bool detected();
    bool detected(long type, long ID);
    int contacts();   //! number of detected items
    PGE_RectF trapZone();

    luabind::object getBlocks(lua_State *L);
    luabind::object getBGOs(lua_State *L);
    luabind::object getNPCs(lua_State *L);
    luabind::object getPlayers(lua_State *L);

private:
    QHash<int, int > detectedPLR;
    QHash<int, int > detectedBLK;
    QHash<int, int > detectedBGO;
    QHash<int, int > detectedNPC;
    QList<LVL_Block*>   detectedBlocks;
    QList<LVL_Bgo*>     detectedBGOs;
    QList<LVL_Npc*>     detectedNPCs;
    QList<LVL_Player*>  detectedPlayers;
    int _filters;
    int _contacts;
    PGE_RectF _srcTrapZone;
    PGE_RectF _trapZone;
};

#endif // PLAYERINAREADETECTOR_H
