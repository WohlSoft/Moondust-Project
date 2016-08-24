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
                if(bgo)
                {
                    l_pushBgo(bgo);
                }
                break;
            }
        case PGE_Phys_Object::LVLPhysEnv:
            {
                LVL_PhysEnv *env = static_cast<LVL_PhysEnv*>(cEL);
                if(env)
                {
                    environments_map[intptr_t(env)] = env->env_type;
                }
                break;
            }
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                if( blk->isHidden )
                    break;

                if(blk->setup->setup.lava)
                {
                    l_pushBlk(blk);
                    doKill = true;
                    killReason = DAMAGE_LAVABURN;
                }
                break;
            }
        case PGE_Phys_Object::LVLPlayer:
            {
                LVL_Player*plr = static_cast<LVL_Player*>(cEL);
                if(plr)
                {
                    l_pushPlr(plr);
                }
            }
        case PGE_Phys_Object::LVLNPC:
            {
                LVL_Npc *npc= static_cast<LVL_Npc*>(cEL);
                if(npc)
                {
                    if(npc->data.friendly) break;
                    if(npc->isGenerator) break;

                    if(!npc->isActivated)
                        break;
                    l_pushNpc(npc);
                }
                break;
            }
        }
    }

    if(doHurt)
        harm(hurtDamage, killReason);
    if(doKill)
        kill(killReason);
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

}

void LVL_Npc::collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
{
    Q_UNUSED(blocksHit);

}
