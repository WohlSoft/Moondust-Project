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
#include "../../scene_level.h"
#include <Utils/maths.h>
#include <data_configs/config_manager.h>


bool LVL_Npc::isInited()
{
    return m_isInited;
}

void LVL_Npc::init()
{
    if(m_isInited)
        return;

    phys_setup.gravityAccel = ConfigManager::g_setup_npc.phs_gravity_accel;
    phys_setup.max_vel_y =   ConfigManager::g_setup_npc.phs_max_fall_speed;
    transformTo_x(data.id);
    setDirection(data.direct);

    if(isLuaNPC && !data.generator)
    {
        try
        {
            lua_onInit();
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
        }
    }

    m_isInited = true;
    m_treemap.addToScene();
    m_scene->m_layers.registerItem(data.layer, this);
    m_momentum_relative.saveOld();
    m_momentum.saveOld();
}

void LVL_Npc::setScenePointer(LevelScene *_pointer)
{
    m_scene = _pointer;
    detector_player_pos._scene = _pointer;
}

LevelNPC LVL_Npc::getData()
{
    return data;
}

void LVL_Npc::setDirection(int dir)
{
    if(dir == 0)
        dir = (Maths::rand32() % 2) ? -1 : 1;
    _direction = Maths::sgn(dir);
    double gfxW = static_cast<double>(setup->setup.gfx_w);
    double gfxH = static_cast<double>(setup->setup.gfx_h);
    double offsetX = static_cast<double>(setup->setup.gfx_offset_x);
    double offsetY = static_cast<double>(setup->setup.gfx_offset_y);
    double physW = static_cast<double>(setup->setup.width);
    double physH = static_cast<double>(setup->setup.height);
    double imgOffsetX = -(((gfxW - physW) / -2.0) + (-offsetX * static_cast<double>(_direction)));
    double imgOffsetY = -(-gfxH + physH + offsetY);
    offset.setPoint(imgOffsetX, imgOffsetY);
}

int LVL_Npc::direction()
{
    return _direction;
}

void LVL_Npc::transformTo(unsigned long id, int type)
{
    if(id <= 0) return;

    if(type == 2) //block
    {
        LevelBlock def = FileFormats::CreateLvlBlock();
        if(transformedFromBlockData.get())
            def = *transformedFromBlockData;
        else
        {
            def.layer = data.layer;
            def.w = static_cast<long>(round(m_momentum.w));
            def.h = static_cast<long>(round(m_momentum.h));
        }
        def.x = static_cast<long>(round(m_momentum.x));
        def.y = static_cast<long>(round(m_momentum.y));
        def.id = id;
        LVL_Block *res = m_scene->spawnBlock(def);
        if(res)
        {
            res->transformedFromNpcID = data.id;
            res->setCenterPos(m_momentum.centerX(), m_momentum.centerY());
            res->m_momentum.saveOld();
            res->m_momentum_relative.saveOld();
            if(res->m_parent)
                res->m_momentum = res->m_momentum_relative;
            res->data.x = long(round(res->m_momentum.x));
            res->data.y = long(round(res->m_momentum.y));
            res->dataInitial.x = long(round(res->m_momentum.x));
            res->dataInitial.y = long(round(res->m_momentum.y));
        }
        //}
        this->unregister();
    }

    if(type == 1) //Other NPC
        transformTo_x(id);
}

void LVL_Npc::setDefaults()
{
    if(!setup) return;

    setDirection(_direction);//Re-apply offset preferences
    motionSpeed = ((!data.nomove) && (setup->setup.movement)) ? (setup->setup.speed) : 0.0;
    is_static   = setup->setup.scenery;
    is_activity = setup->setup.activity;
    is_shared_animation = setup->setup.shared_ani;
    keep_position_on_despawn = setup->setup.keep_position;
}

void LVL_Npc::transformTo_x(unsigned long id)
{
    if(m_isInited)
    {
        if((_npc_id == id) || (id == 0) || !ConfigManager::lvl_npc_indexes.contains(id))
            return;
        setup = &ConfigManager::lvl_npc_indexes[id];
        transformedFromNpcID = _npc_id;
        _npc_id = id;
    }
    else
    {
        _npc_id = id;
        m_momentum.x = data.x;
        m_momentum.y = data.y;
        _syncSection(false);
    }

    long double targetZ = 0;

    if(setup->setup.foreground)
        targetZ = LevelScene::zOrder.npcFront;
    else if(setup->setup.background)
        targetZ = LevelScene::zOrder.npcBack;
    else
        targetZ = LevelScene::zOrder.npcStd;

    z_index = targetZ + static_cast<long double>(setup->setup.z_offset);
    m_scene->m_zCounter += 0.0000000000001L;
    z_index += m_scene->m_zCounter;

    if(m_scene->m_zCounter >= 1.0L)
        m_scene->m_zCounter = 0.0L;

    int tID = ConfigManager::getNpcTexture(_npc_id);
    if(tID >= 0)
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = ((setup->setup.frames > 1) || (setup->setup.framestyle > 0));
        animator_ID = setup->animator_ID;
    }

    warpFrameW = texture.frame_w;
    warpFrameH = texture.frame_h;

    if(m_isInited)
    {
        double oldCX = m_momentum.centerX();
        double oldB = m_momentum.bottom();
        m_momentum.w = setup->setup.width;
        m_momentum.h = setup->setup.height;
        m_momentum.x = oldCX - m_momentum.w / 2.0;
        m_momentum.y = oldB - m_momentum.h;
    }
    else
    {
        m_momentum.w = setup->setup.width;
        m_momentum.h = setup->setup.height;
    }

    if(m_isInited)
        m_treemap.updatePosAndSize();

    if(data.generator)
    {
        m_isGenerator = true;
        generatorDirection = data.generator_direct;
        generatorTimeLeft  = 1;
        generatorType      = data.generator_type;
        deActivatable     = true;
        activationTimeout = 150;
        m_blocked[1] = Block_NONE;
        m_blocked[2] = Block_NONE;
        m_contactPadding = 1.0;
        m_disableBlockCollision = true;
        setGravityScale(0.0);
        m_bodytype = Body_STATIC;
        m_collisionCheckPolicy = CollisionCheckPolicy_CENTER_CONTACTS_ONLY;
        return;
    }

    deActivatable = ((setup->setup.deactivation) || (!setup->setup.activity));
    offSectionDeactivate = setup->setup.deactivate_off_room;
    activationTimeout = setup->setup.deactivationDelay;
    m_disableBlockCollision = !setup->setup.collision_with_blocks;
    disableNpcCollision  = setup->setup.no_npc_collisions;
    m_contactPadding = setup->setup.contact_padding;
    frameSize.setSize(setup->setup.gfx_w, setup->setup.gfx_h);
    animator.construct(texture, *setup);
    setDefaults();
    setGravityScale(setup->setup.gravity ? 1.0 : 0.0);
    m_blocked[1] = Block_NONE;

    if(setup->setup.block_player)
        m_blocked[1] = Block_LEFT | Block_RIGHT;

    if(setup->setup.block_player_top)
        m_blocked[1] |= Block_TOP;

    if(setup->setup.block_player && setup->setup.block_player_top)
        m_blocked[1] = Block_ALL;

    if(data.friendly || setup->setup.takable)
        m_blocked[1] = Block_NONE;

    m_blocked[2] = Block_NONE;

    if(setup->setup.block_npc)
        m_blocked[2] = Block_LEFT | Block_RIGHT;

    if(setup->setup.block_npc_top)
        m_blocked[2] |= Block_TOP;

    if(setup->setup.block_npc && setup->setup.block_npc_top)
        m_blocked[2] = Block_ALL;

    if(data.friendly)
        m_blocked[2] = Block_NONE;

    if(m_isInited)
    {
        int leftHealth = setup->setup.health - (setup->setup.health - health);
        health = (leftHealth > 0) ? leftHealth : 1;
        animator.start();
    }
    else
        health = (setup->setup.health > 0) ? setup->setup.health : 1;

    if(m_isInited)
    {
        try
        {
            lua_onTransform(_npc_id);
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
        }
    }
}
