#ifndef PLAYERINAREADETECTOR_H
#define PLAYERINAREADETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/rectf.h>
#include <unordered_map>
#include <vector>

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
    std::unordered_map<long, long> detectedPLR;
    std::unordered_map<long, long> detectedBLK;
    std::unordered_map<long, long> detectedBGO;
    std::unordered_map<long, long> detectedNPC;
    std::vector<LVL_Block*>     detectedBlocks;
    std::vector<LVL_Bgo*>       detectedBGOs;
    std::vector<LVL_Npc*>       detectedNPCs;
    std::vector<LVL_Player*>    detectedPlayers;
    int _filters;
    int _contacts;
    PGE_RectF _srcTrapZone;
    PGE_RectF _trapZone;
};

#endif // PLAYERINAREADETECTOR_H
