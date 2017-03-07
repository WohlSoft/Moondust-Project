/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <Utils/maths.h>
#include "../../scene_level.h"

void LVL_Npc::lua_setSequenceLeft(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE)
    {
        luaL_error(frames.interpreter(), "setSequenceLeft exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceL(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequenceRight(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE)
    {
        luaL_error(frames.interpreter(), "setSequenceRight exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceR(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequence(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE)
    {
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
    std::vector<PGE_Phys_Object *> bodies;
    PGE_RectF posRectC = m_momentum.rectF().withMargin(2.0);
    m_scene->queryItems(posRectC, &bodies);

    int found = 0;
    for(PGE_RenderList::iterator it = bodies.begin(); it != bodies.end(); it++)
    {
        PGE_Phys_Object *item = *it;
        if(item->type != PGE_Phys_Object::LVLNPC) continue;
        LVL_Npc *body = dynamic_cast<LVL_Npc *>(item);
        if(!body) continue;
        if(body == this) continue;
        if(!body->isVisible()) continue;
        if(body->killed) continue;
        if(body->_npc_id != _npc_id) continue;
        if(!body->isActivated)
        {
            body->Activate();
            m_scene->active_npcs.push_back(body);
            found++;
        }
    }
    return found;
}

LVL_Npc *LVL_Npc::lua_spawnNPC(int npcID, int sp_type, int sp_dir, bool reSpawnable)
{
    LevelNPC def = data;
    def.id = static_cast<unsigned long>(npcID);
    def.x = Maths::lRound(posX());
    def.y = Maths::lRound(posY());
    def.direct = _direction;
    def.generator = false;
    def.layer = "Spawned NPCs";
    def.attach_layer = "";
    def.event_activate = "";
    def.event_die = "";
    def.event_talk = "";
    def.event_emptylayer = "";
    return m_scene->spawnNPC(def,
                            (LevelScene::NpcSpawnType)sp_type,
                            (LevelScene::NpcSpawnDirection)sp_dir, reSpawnable);
}

PlayerPosDetector *LVL_Npc::lua_installPlayerPosDetector()
{
    std::set<BasicDetector * >::iterator i = detectors.find(&detector_player_pos);
    if(i == detectors.end())
        detectors.insert(&detector_player_pos);
    return &detector_player_pos;
}

InAreaDetector *LVL_Npc::lua_installInAreaDetector(double left, double top, double right, double bottom, luabind::object filters)
{
    int ltype = luabind::type(filters);
    std::vector<int> _filters;
    if(ltype == LUA_TNIL)
    {
        _filters = std::vector<int>({InAreaDetector::F_BLOCK,
                                     InAreaDetector::F_BGO,
                                     InAreaDetector::F_NPC,
                                     InAreaDetector::F_PLAYER
                                    });
    }
    else
    {
        if(ltype != LUA_TTABLE)
        {
            luaL_error(filters.interpreter(), "installPlayerInAreaDetector exptected int-array, got %s", lua_typename(filters.interpreter(), ltype));
            return NULL;
        }
        _filters = luabind_utils::convArrayTo<int>(filters);
    }

    int tfilters = 0;
    for(int &filter : _filters)
    {
        switch(filter)
        {
        case 1:
            tfilters |= InAreaDetector::F_BLOCK;
            break;
        case 2:
            tfilters |= InAreaDetector::F_BGO;
            break;
        case 3:
            tfilters |= InAreaDetector::F_NPC;
            break;
        case 4:
            tfilters |= InAreaDetector::F_PLAYER;
            break;
        }
    }

    PGE_RectF r;
    r.setLeft(left);
    r.setTop(top);
    r.setRight(right);
    r.setBottom(bottom);

    detectors_inarea.push_back(InAreaDetector(this, r, tfilters));
    detectors.insert(&detectors_inarea.back());
    return &detectors_inarea.back();
}

ContactDetector *LVL_Npc::lua_installContactDetector()
{
    detectors_contact.push_back(ContactDetector(this));
    detectors.insert(&detectors_contact.back());
    return &detectors_contact.back();
}
