/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../lvl_npc.h"
#include <script/lua_engine.h>
#include "../lvl_scene_ptr.h"

void LVL_Npc::lua_setSequenceLeft(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceLeft exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceL(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequenceRight(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceRight exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceR(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequence(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequence exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequence(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setOnceAnimation(bool en)
{
    animator.setOnceMode(en);
}

bool LVL_Npc::lua_animationIsFinished()
{
    return animator.animationFinished();
}

int LVL_Npc::lua_frameDelay()
{
    return animator.frameSpeed();
}

void LVL_Npc::lua_setFrameDelay(int ms)
{
    animator.setFrameSpeed(ms);
}

int LVL_Npc::lua_activate_neighbours()
{
    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = posRect.withMargin(2.0);
    LvlSceneP::s->queryItems(posRectC, &bodies);

    int found=0;
    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object* item=*it;
        if(item->type!=PGE_Phys_Object::LVLNPC) continue;
        LVL_Npc*body=dynamic_cast<LVL_Npc*>(item);
        if(!body) continue;
        if(body==this) continue;
        if(!body->isVisible()) continue;
        if(body->killed) continue;
        if(body->data.id!=data.id) continue;
        if(!body->isActivated)
        {
            body->Activate();
            LvlSceneP::s->active_npcs.push_back(body);
            found++;
        }
    }
    return found;
}

LVL_Npc *LVL_Npc::lua_spawnNPC(int npcID, int sp_type, int sp_dir, bool reSpawnable)
{
    LevelNPC def = data;
    def.id=npcID;
    def.x=round(posX());
    def.y=round(posY());
    def.generator=false;
    def.layer="Spawned NPCs";
    return LvlSceneP::s->spawnNPC(def,
                           (LevelScene::NpcSpawnType)sp_type,
                           (LevelScene::NpcSpawnDirection)sp_dir, reSpawnable);
}

PlayerPosDetector *LVL_Npc::lua_installPlayerPosDetector()
{

    if(!detectors.contains(&detector_player_pos))
        detectors.push_back(&detector_player_pos);
    return &detector_player_pos;
}

InAreaDetector *LVL_Npc::lua_installInAreaDetector(float left, float top, float right, float bottom, luabind::object filters)
{
    int ltype = luabind::type(filters);
    QList<int> _filters;
    if(ltype == LUA_TNIL){
        _filters = QList<int>({InAreaDetector::F_BLOCK,
                               InAreaDetector::F_BGO,
                               InAreaDetector::F_NPC,
                               InAreaDetector::F_PLAYER});
    } else {
        if(ltype != LUA_TTABLE){
            luaL_error(filters.interpreter(), "installPlayerInAreaDetector exptected int-array, got %s", lua_typename(filters.interpreter(), ltype));
            return NULL;
        }
        _filters = luabind_utils::convArrayTo<int>(filters);
    }

    int tfilters = 0;
    foreach(int filter, _filters)
    {
        switch(filter)
        {
            case 1: tfilters |= InAreaDetector::F_BLOCK; break;
            case 2: tfilters |= InAreaDetector::F_BGO; break;
            case 3: tfilters |= InAreaDetector::F_NPC; break;
            case 4: tfilters |= InAreaDetector::F_PLAYER; break;
        }
    }

    PGE_RectF r;
        r.setLeft(left);
        r.setTop(top);
        r.setRight(right);
        r.setBottom(bottom);

    detectors_inarea.push_back(InAreaDetector(this, r, tfilters));
    detectors.push_back(&detectors_inarea.last());
    return &detectors_inarea.last();
}


