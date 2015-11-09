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
#include "../lvl_scene_ptr.h"
#include <common_features/maths.h>
#include <data_configs/config_manager.h>


bool LVL_Npc::isInited()
{
    return _isInited;
}

void LVL_Npc::init()
{
    if(_isInited) return;

    phys_setup.gravityAccel=ConfigManager::marker_npc.phs_gravity_accel;
    phys_setup.max_vel_y=   ConfigManager::marker_npc.phs_max_fall_speed;

    transformTo_x(data.id);
    setPos(data.x, data.y);
    _syncSection();
    if(isLuaNPC && !data.generator){
        try{
            lua_onInit();
        } catch (luabind::error& e) {
            LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
        }
    }

    _isInited=true;
    LvlSceneP::s->layers.registerItem(data.layer, this);
}

LevelNPC LVL_Npc::getData()
{
    return data;
}

void LVL_Npc::setDirection(int dir)
{
    if(dir==0) dir=(rand()%2) ? -1 : 1;
    _direction=Maths::sgn(dir);
    int imgOffsetX = -((int)round( - ( ( (double)setup->gfx_w - (double)setup->width ) / 2 ) )
                        +(-((double)setup->gfx_offset_x)*_direction));
    int imgOffsetY = -(int)round( - (double)setup->gfx_h + (double)setup->height + (double)setup->gfx_offset_y);
    offset.setPoint(imgOffsetX, imgOffsetY);
}

int LVL_Npc::direction()
{
    return _direction;
}

void LVL_Npc::transformTo(long id, int type)
{
    if(id<=0) return;

    if(type==2)//block
    {
    //        transformTask_block t;
    //        t.block = this;
    //        t.id=id;
    //        t.type=type;

        //LvlSceneP::s->block_transforms.push_back(t);
    }
    if(type==1)//Other NPC
    {
        // :-P
    }
}

void LVL_Npc::setDefaults()
{
    if(!setup) return;
    setDirection(data.direct);
    motionSpeed = ((!data.nomove)&&(setup->movement)) ? ((float)setup->speed) : 0.0f;
    is_scenery  = setup->scenery;
    is_activity = setup->activity;
    is_shared_animation = setup->shared_ani;
}

void LVL_Npc::transformTo_x(long id)
{
    if(_isInited)
    {
        if(data.id==(unsigned)abs(id)) return;
        if(!ConfigManager::lvl_npc_indexes.contains(id))
            return;
        setup = &ConfigManager::lvl_npc_indexes[id];
    }

    data.id=id;

    double targetZ = 0;
    if(setup->foreground)
        targetZ = LevelScene::Z_npcFore;
    else
    if(setup->background)
        targetZ = LevelScene::Z_npcBack;
    else
        targetZ = LevelScene::Z_npcStd;

    z_index += targetZ;

    LevelScene::zCounter += 0.00000001;
    z_index += LevelScene::zCounter;

    long tID = ConfigManager::getNpcTexture(data.id);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = ((setup->frames>1) || (setup->framestyle>0));
        animator_ID = setup->animator_ID;
    }

    warpFrameW = texture.w;
    warpFrameH = texture.h;

    setSize(setup->width, setup->height);

    if(data.generator)
    {
        isGenerator=true;
        generatorDirection=data.generator_direct;
        generatorTimeLeft= 1;
        generatorType    = data.generator_type;

        deActivatable    = true;
        activationTimeout= 150;
        collide_npc=COLLISION_NONE;
        collide_player=COLLISION_NONE;
        disableBlockCollision=true;
        setGravityScale(0.0f);
        return;
    }

    deActivatable = ((setup->deactivation)||(!setup->activity));
    activationTimeout= setup->deactivetionDelay;

    disableBlockCollision=!setup->collision_with_blocks;
    disableNpcCollision  = setup->no_npc_collions;

    setDirection(_direction);
    frameSize.setSize(setup->gfx_w, setup->gfx_h);
    animator.construct(texture, *setup);

    setDefaults();
    setGravityScale(setup->gravity ? 1.0f : 0.f);

    if(setup->block_player)
        collide_player = COLLISION_ANY;
    else
    if(setup->block_player_top)
        collide_player = COLLISION_TOP;
    else
        collide_player = COLLISION_NONE;

    if(setup->block_npc)
        collide_npc = COLLISION_ANY;
    else
    if(setup->block_npc_top)
        collide_npc = COLLISION_TOP;
    else
        collide_npc = COLLISION_NONE;
    if(setup->no_npc_collions)
        collide_npc=COLLISION_NONE;

    if(_isInited)
    {
       int leftHealth=setup->health-(setup->health-health);
       health=(leftHealth>0)? leftHealth : 1;
    }
    else
       health=(setup->health>0)?setup->health : 1;

    if(_isInited)
    {
        lua_onTransform(data.id);
    }
}
