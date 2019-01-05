/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "lvl_bgo.h"
#include "../../data_configs/config_manager.h"
#include <graphics/gl_renderer.h>
#include "../scene_level.h"

LVL_Bgo::LVL_Bgo(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    type = LVLBGO;
    m_bodytype = Body_STATIC;
    data = FileFormats::CreateLvlBgo();
    animated = false;
    animator_ID = 0;
    setup = nullptr;
    _isInited = false;
}

LVL_Bgo::~LVL_Bgo()
{}

void LVL_Bgo::init()
{
    if(_isInited)
        return;
    transformTo_x(data.id);
    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    _isInited = true;
    m_scene->m_layers.registerItem(data.layer, this);
    m_momentum_relative.saveOld();
    m_momentum.saveOld();
}

void LVL_Bgo::transformTo_x(unsigned long id)
{
    if(_isInited)
    {
        if(data.id == id)
            return;

        if(!ConfigManager::lvl_bgo_indexes.contains(id))
            return;
    }

    data.id = id;
    setup = &ConfigManager::lvl_bgo_indexes[data.id];
    long double targetZ = 0.0L;

    if(setup->setup.zValueOverride)
        targetZ = setup->setup.zValue;
    else
    {
        switch(setup->setup.zLayer)
        {
        case -1:
            targetZ = LevelScene::zOrder.bgoBack2;
            break;
        case 0:
            targetZ = LevelScene::zOrder.bgoBack1;
            break;
        case 1:
            targetZ = LevelScene::zOrder.bgoFront1;
            break;
        case 2:
            targetZ = LevelScene::zOrder.bgoFront1;
            break;
        default://Background-1 layer is default
            targetZ = LevelScene::zOrder.bgoBack1;
        }
        targetZ += setup->setup.zOffset + data.z_offset;
    }

    switch(data.z_mode)
    {
    case LevelBGO::Background2:
        targetZ = LevelScene::zOrder.bgoBack2 + setup->setup.zOffset + data.z_offset;
        break;
    case LevelBGO::Background1:
        targetZ = LevelScene::zOrder.bgoBack1 + setup->setup.zOffset + data.z_offset;
        break;
    case LevelBGO::Foreground1:
        targetZ = LevelScene::zOrder.bgoFront1 + setup->setup.zOffset + data.z_offset;
        break;
    case LevelBGO::Foreground2:
        targetZ = LevelScene::zOrder.bgoFront1 + setup->setup.zOffset + data.z_offset;
        break;
    default:
        break;
    }

    z_index = targetZ;
    m_scene->m_zCounter += 0.0000000000001L;
    z_index += m_scene->m_zCounter;

    if(m_scene->m_zCounter >= 1.0L)
        m_scene->m_zCounter = 0.0L;

    int tID = ConfigManager::getBgoTexture(data.id);

    if(tID >= 0)
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = setup->setup.animated;
        animator_ID = setup->animator_ID;
    }

    if(!_isInited)
    {
        m_momentum.x = data.x;
        m_momentum.y = data.y;
    }
    m_momentum.w = texture.frame_w;
    m_momentum.h = texture.frame_h;
    if(!m_treemap.m_is_registered)
        m_treemap.addToScene();
    else
    {
        m_treemap.updateSize();
        m_momentum_relative.saveOld();
        m_momentum.saveOld();
    }
}

void LVL_Bgo::render(double camX, double camY)
{
    AniPos x(0, 1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_BGO[animator_ID].image();

    GlRenderer::renderTexture(&texture,
                              static_cast<float>(posX() - camX),
                              static_cast<float>(posY() - camY),
                              static_cast<float>(m_momentum.w),
                              static_cast<float>(m_momentum.h),
                              static_cast<float>(x.first),
                              static_cast<float>(x.second));
}

bool LVL_Bgo::isInited()
{
    return _isInited;
}

unsigned long LVL_Bgo::lua_getID()
{
    return data.id;
}
