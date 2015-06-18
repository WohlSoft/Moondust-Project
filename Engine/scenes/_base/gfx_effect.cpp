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

#include "gfx_effect.h"
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>
#include <gui/pge_msgbox.h>

void Scene::launchStaticEffectC(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity, int direction, Scene_Effect_Phys phys)
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
    _effect.m_velocityX = velocityX;
    _effect.m_velocityY = velocityY;
    _effect.phys_setup=phys;
    _effect.gravity = gravity;
    _effect.animated=_effect.setup->animated;
    if(direction==0) direction = (rand()%2) ? -1 : 1;
    _effect.direction=direction;
    _effect.frameStyle = _effect.setup->framestyle;
    int frame1=0;
    int frameE=-1;
    int frms=0;
    frms=_effect.setup->frames;

    switch(_effect.frameStyle)
    {
    case 0:
        frms=_effect.setup->frames;
        frame1=0;
        frameE=-1;
        break;
    case 1:
        frms=_effect.setup->frames*2;
        frame1= _effect.direction<0? 0 : _effect.setup->frames  -1;
        frameE= _effect.direction<0 ?    _effect.setup->frames: -1;
        break;
    case 2:
        frms=_effect.setup->frames*4;
        frame1= _effect.direction<0 ? 0 : (int)(frms-(_effect.setup->frames)*3)-1;
        frameE= _effect.direction<0 ?     (int)(frms-(_effect.setup->frames)*3) : (frms/2)-1;
        break;
    case 3:
        break;
    default: break;
    }
    _effect.animator.construct(true, frms, _effect.setup->framespeed, frame1, frameE);
    _effect.posRect.setSize(_effect.texture.w, _effect.texture.h/frms);
    _effect.posRect.setPos(startX-_effect.posRect.width()/2, startY-_effect.posRect.height()/2);

    if(delay>0)
    {
        _effect._limit_delay=true;
        _effect._delay=delay;
    }

    if(animationLoops>0)
    {
        _effect.animator.setOnceMode(true, animationLoops);
    }

    _effect.init();
    WorkingEffects.push_back(_effect);
}



void Scene::launchStaticEffect(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity, int direction, Scene_Effect_Phys phys)
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
    _effect.m_velocityX = velocityX;
    _effect.m_velocityY = velocityY;
    _effect.phys_setup=phys;
    _effect.gravity = gravity;
    _effect.animated=_effect.setup->animated;
    if(direction==0) direction = (rand()%2) ? -1 : 1;
    _effect.direction=direction;
    _effect.frameStyle = _effect.setup->framestyle;
    int frame1=0;
    int frameE=-1;
    int frms=0;
    frms=_effect.setup->frames;

    switch(_effect.frameStyle)
    {
    case 0:
        frms=_effect.setup->frames;
        frame1=0;
        frameE=-1;
        break;
    case 1:
        frms=_effect.setup->frames*2;
        frame1= _effect.direction<0? 0 : _effect.setup->frames  -1;
        frameE= _effect.direction<0 ?    _effect.setup->frames: -1;
        break;
    case 2:
        frms=_effect.setup->frames*4;
        frame1= _effect.direction<0 ? 0 : (int)(frms-(_effect.setup->frames)*3)-1;
        frameE= _effect.direction<0 ?     (int)(frms-(_effect.setup->frames)*3) : (frms/2)-1;
        break;
    case 3:
        break;
    default: break;
    }
    _effect.animator.construct(true, frms, _effect.setup->framespeed, frame1, frameE);
    _effect.posRect.setSize(_effect.texture.w, _effect.texture.h/frms);
    _effect.posRect.setPos(startX, startY);

    if(delay>0)
    {
        _effect._limit_delay=true;
        _effect._delay=delay;
    }

    if(animationLoops>0)
    {
        _effect.animator.setOnceMode(true, animationLoops);
    }

    _effect.init();
    WorkingEffects.push_back(_effect);
}

void Scene::processEffects(float ticks)
{
    for(int i=0; i<WorkingEffects.size(); i++)
    {
        WorkingEffects[i].update(ticks);
        if(WorkingEffects[i].finished() || !isVizibleOnScreen(WorkingEffects[i].posRect))
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

     direction=0;
     frameStyle=0;

     _limit_delay=false;
     _delay=0;

     m_velocityX=0;
     m_velocityY=0;

     gravity=0;

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
    phys_setup=e.phys_setup;
    gravity = e.gravity;

    posRect=e.posRect;

    _finished=e._finished;
    timeStep= (1000.f/65.f);//(1000.f/(float)PGE_Window::TicksPerSecond);
}

Scene_Effect::~Scene_Effect()
{}

void Scene_Effect::init()
{}

float Scene_Effect::posX()
{
    return posRect.x();
}

float Scene_Effect::posY()
{
    return posRect.y();
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
    posRect.setX(posRect.x()+m_velocityX * (timeStep/ticks));
    posRect.setY(posRect.y()+m_velocityY * (timeStep/ticks));

    float accelCof=ticks/1000.0f;
    if(phys_setup.decelerate_x!=0)
    {
        float decX=phys_setup.decelerate_x*accelCof;
        if(m_velocityX>0)
        {
            if((m_velocityX-decX>0.0))
                m_velocityX-=decX;
            else
                m_velocityX=0;
        } else if(m_velocityX<0) {
            if((m_velocityX+decX<0.0))
                m_velocityX+=decX;
            else
                m_velocityX=0;
        }
    }

    if(phys_setup.decelerate_y!=0)
    {
        float decY=phys_setup.decelerate_y*accelCof;
        if(m_velocityY>0)
        {
            if((m_velocityY-decY>0.0))
                m_velocityY-=decY;
            else
                m_velocityY=0;
        } else if(m_velocityY<0) {
            if((m_velocityY+decY<0.0))
                m_velocityY+=decY;
            else
                m_velocityY=0;
        }
    }

    if(gravity!=0.0f)
    {
        m_velocityY+= gravity*accelCof;
    }

    if((phys_setup.max_vel_x!=0)&&(m_velocityX>phys_setup.max_vel_x)) m_velocityX=phys_setup.max_vel_x;
    if((phys_setup.mix_vel_x!=0)&&(m_velocityX<phys_setup.mix_vel_x)) m_velocityX=phys_setup.mix_vel_x;
    if((phys_setup.max_vel_y!=0)&&(m_velocityY>phys_setup.max_vel_y)) m_velocityY=phys_setup.max_vel_y;
    if((phys_setup.mix_vel_y!=0)&&(m_velocityY<phys_setup.mix_vel_y)) m_velocityY=phys_setup.mix_vel_y;
}

void Scene_Effect::render(double camX, double camY)
{
    AniPos x(0,1);
    if(animated) //Get current animated frame
        x = animator.image();
    GlRenderer::renderTexture(&texture, posX()-camX, posY()-camY, posRect.width(), posRect.height(), x.first, x.second);
}



Scene_Effect_Phys::Scene_Effect_Phys()
{
    mix_vel_x=0;
    mix_vel_y=0;
    max_vel_x=0;
    max_vel_y=0;
    decelerate_x=0;
    decelerate_y=0;
}
