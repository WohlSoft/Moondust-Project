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

#include "lvl_effect.h"
#include "../../data_configs/config_manager.h"

#include <graphics/gl_renderer.h>

#include "../scene_level.h"

#include <gui/pge_msgbox.h>
#include <fontman/font_manager.h>

void LevelScene::launchStaticEffect(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity)
{
    Scene_Effect _effect;
    if(ConfigManager::lvl_effects_indexes.contains(effectID))
        _effect.setup = &ConfigManager::lvl_effects_indexes[effectID];
    else
    {
        PGE_MsgBox oops(this, QString("Can't launch effect %1").arg(effectID), PGE_MsgBox::msg_error);
        oops.exec();
        return;
    }

    long tID = ConfigManager::getEffectTexture(effectID);
    if( tID >= 0 )
    {
        _effect.texture = ConfigManager::level_textures[tID];
    }
    _effect.m_posX = startX;
    _effect.m_posY = startY;
    _effect.m_velocityX = velocityX;
    _effect.m_velocityY = velocityY;
    _effect.gravity = gravity;
    _effect.animated=_effect.setup->animated;
    _effect.animator.construct(_effect.setup->animated, _effect.setup->frames, _effect.setup->framespeed, 0, -1);

    if(delay>0)
    {
        _effect._limit_delay=true;
        _effect._delay=delay;
    }
    else if(animationLoops>0)
    {
        _effect.animator.setOnceMode(true);
    }

    _effect.init();
    WorkingEffects.push_back(_effect);
}

void LevelScene::processEffects(float ticks)
{
    for(int i=0; i<WorkingEffects.size(); i++)
    {
        WorkingEffects[i].update(ticks);
        if(WorkingEffects[i].finished())
        {
            WorkingEffects.removeAt(i);
            i--;
        }
    }
}





Scene_Effect::Scene_Effect()
{
     animated=false;
     animator_ID=0;

     _limit_delay=false;
     _delay=0;

     m_velocityX=0;
     m_velocityY=0;

     gravity=0;

     m_posX=0;
     m_posY=0;
     width=0;
     height=0;
     _finished=false;
     setup=NULL;
     timeStep=(1000.f/65.f);
}

Scene_Effect::Scene_Effect(const Scene_Effect &e)
{
    animated=e.animated;
    animator_ID=e.animator_ID;
    animator=e.animator;
    texture = e.texture;
    setup = e.setup;

    _limit_delay=e._limit_delay;
    _delay=e._delay;

    m_velocityX=e.m_velocityX;
    m_velocityY=e.m_velocityY;
    gravity = e.gravity;

    m_posX=e.m_posX;
    m_posY=e.m_posY;
    width=e.width;
    height=e.height;
    _finished=e._finished;
    timeStep= (1000.f/65.f);//(1000.f/(float)PGE_Window::TicksPerSecond);
}

Scene_Effect::~Scene_Effect()
{}

void Scene_Effect::init()
{
    width=texture.w;
    height=texture.h;
}

float Scene_Effect::posX()
{
    return m_posX;
}

float Scene_Effect::posY()
{
    return m_posY;
}

bool Scene_Effect::finished()
{
    return _finished;
}

void Scene_Effect::update(float ticks)
{
    animator.manualTick(ticks);
    if(_limit_delay)
    {
        _delay-=ticks;
        if(_delay<=0.0f) _finished=true;
    }
    if(animator.isFinished())
        _finished=true;
    iterateStep(ticks);
}

void Scene_Effect::iterateStep(float ticks)
{
    m_posX+= m_velocityX * (timeStep/ticks);
    m_posY+= m_velocityY * (timeStep/ticks);
    m_velocityY+= (gravity*(timeStep/ticks))*0.2;
    if(m_velocityY>gravity)
       m_velocityY=gravity;
}

void Scene_Effect::render(double camX, double camY)
{
    AniPos x(0,1);
    if(animated) //Get current animated frame
        x = animator.image();
    GlRenderer::renderTexture(&texture, posX()-camX, posY()-camY, width, height, x.first, x.second);
}



