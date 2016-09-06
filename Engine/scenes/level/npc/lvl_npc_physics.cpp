/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../lvl_player.h"
#include "../lvl_block.h"
#include "../lvl_physenv.h"
#include "../lvl_bgo.h"
#include "../../scene_level.h"

#include <cassert>

void LVL_Npc::processContacts()
{
    /* *********************** Check all collided sides ***************************** */
    bool doHurt = false;
    int  hurtDamage=0;
    bool doKill = false;
    int  killReason = DAMAGE_NOREASON;

    for(ObjectCollidersIt it=l_contactAny.begin(); it!=l_contactAny.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBGO:
            {
                LVL_Bgo *bgo= static_cast<LVL_Bgo*>(cEL);
                assert(bgo);
                l_pushBgo(bgo);
                break;
            }
        case PGE_Phys_Object::LVLPhysEnv:
            {
                LVL_PhysEnv *env = static_cast<LVL_PhysEnv*>(cEL);
                assert(env);
                environments_map[intptr_t(env)] = env->env_type;
                break;
            }
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                assert(blk);
                if( blk->m_isHidden )
                    break;

                l_pushBlk(blk);

                if(blk->setup->setup.lava)
                {
                    doKill = true;
                    killReason = DAMAGE_LAVABURN;
                }
                break;
            }
        case PGE_Phys_Object::LVLPlayer:
            {
                LVL_Player*plr = static_cast<LVL_Player*>(cEL);
                assert(plr);
                l_pushPlr(plr);
            }
        case PGE_Phys_Object::LVLNPC:
            {
                LVL_Npc *npc= static_cast<LVL_Npc*>(cEL);
                assert(npc);
                if(npc->killed) break;
                if(npc->data.friendly) break;
                if(npc->isGenerator) break;
                if(!npc->isActivated) break;
                l_pushNpc(npc);
                break;
            }
        }
    }

    if(doKill)
        kill(killReason);
    else if(doHurt)
        harm(hurtDamage, killReason);
}

void LVL_Npc::preCollision()
{
    environments_map.clear();
    contacted_bgos.clear();
    contacted_npc.clear();
    contacted_blocks.clear();
    contacted_players.clear();
}

void LVL_Npc::postCollision()
{
    if(m_crushedHard)
    {
        kill(DAMAGE_BY_KICK);
    }
}

void LVL_Npc::collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
{
    Q_UNUSED(blocksHit);
}

bool LVL_Npc::preCollisionCheck(PGE_Phys_Object *body)
{
    Q_ASSERT(body);
    switch(body->type)
    {
        case LVLBlock:
        {
            if(this->disableBlockCollision)
                return true;
            LVL_Block* blk = static_cast<LVL_Block*>(body);
            if(blk->m_destroyed)
                return true;
        }
        break;
        case LVLNPC:
        {
            if(this->disableBlockCollision)
                return true;
        }
        default:;
    }
    return false;
}
