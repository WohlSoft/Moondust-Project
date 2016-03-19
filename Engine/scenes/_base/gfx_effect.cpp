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

#include "gfx_effect.h"
#include <data_configs/config_manager.h>
#include <common_features/logger.h>
#include <graphics/gl_renderer.h>
#include <gui/pge_msgbox.h>

void Scene::launchStaticEffect(long     effectID,
                               float    startX,
                               float    startY,
                               int      animationLoops,
                               int      delay,
                               float    velocityX,
                               float    velocityY,
                               float    gravity,
                               int      direction,
                               Scene_Effect_Phys phys)
{
    SpawnEffectDef effectDef;
    effectDef.id = effectID;
    effectDef.startX = startX;
    effectDef.startY = startY;
    effectDef.animationLoops = animationLoops;
    effectDef.delay = delay;
    effectDef.velocityX = velocityX;
    effectDef.velocityY = velocityY;
    effectDef.gravity = gravity;
    effectDef.direction = direction;
    effectDef.decelerate_x = phys.decelerate_x;
    effectDef.decelerate_y = phys.decelerate_y;
    effectDef.max_vel_x = phys.max_vel_x;
    effectDef.max_vel_y = phys.max_vel_y;
    effectDef.min_vel_x = phys.min_vel_x;
    effectDef.min_vel_y = phys.min_vel_y;
    launchStaticEffect(effectDef, false);
}

void Scene::launchStaticEffectC(long    effectID,
                                float   startX,
                                float   startY,
                                int     animationLoops,
                                int     delay,
                                float   velocityX,
                                float   velocityY,
                                float   gravity,
                                int     direction,
                                Scene_Effect_Phys phys)
{
    SpawnEffectDef effectDef;
    effectDef.id = effectID;
    effectDef.startX = startX;
    effectDef.startY = startY;
    effectDef.animationLoops = animationLoops;
    effectDef.delay = delay;
    effectDef.velocityX = velocityX;
    effectDef.velocityY = velocityY;
    effectDef.gravity = gravity;
    effectDef.direction = direction;
    effectDef.decelerate_x = phys.decelerate_x;
    effectDef.decelerate_y = phys.decelerate_y;
    effectDef.max_vel_x = phys.max_vel_x;
    effectDef.max_vel_y = phys.max_vel_y;
    effectDef.min_vel_x = phys.min_vel_x;
    effectDef.min_vel_y = phys.min_vel_y;
    launchStaticEffect(effectDef, true);
}

void Scene::launchStaticEffect(SpawnEffectDef effect_def, bool centered)
{
    Scene_Effect _effect;
    if(ConfigManager::lvl_effects_indexes.contains(effect_def.id))
        _effect.setup = &ConfigManager::lvl_effects_indexes[effect_def.id];
    else
    {
        PGE_MsgBox oops(this, QString("Can't launch effect %1").arg(effect_def.id), PGE_MsgBox::msg_error);
        oops.exec();
        return;
    }

    long tID = ConfigManager::getEffectTexture(effect_def.id);
    if( tID >= 0 )
    {
        _effect.texture = ConfigManager::level_textures[tID];
    }
    else
    {
        PGE_MsgBox oops(this, QString("Can't load texture for effect-%1").arg(effect_def.id), PGE_MsgBox::msg_error);
        oops.exec();
        return;
    }

    _effect.m_velocityX = effect_def.velocityX;
    _effect.m_velocityY = effect_def.velocityY;
    _effect.phys_setup.decelerate_x=effect_def.decelerate_x;
    _effect.phys_setup.decelerate_y=effect_def.decelerate_y;
    _effect.phys_setup.max_vel_x=effect_def.max_vel_x;
    _effect.phys_setup.max_vel_y=effect_def.max_vel_y;
    _effect.phys_setup.min_vel_x=effect_def.min_vel_x;
    _effect.phys_setup.min_vel_y=effect_def.min_vel_y;
    _effect.gravity = effect_def.gravity;
    if(effect_def.direction==0)
        effect_def.direction = (rand()%2) ? -1 : 1;
    _effect.direction = effect_def.direction;
    _effect.frameStyle = _effect.setup->framestyle;
    int frame1=0;
    int frameE=-1;
    int frms=0;
    frms = _effect.setup->frames;

    switch(_effect.frameStyle)
    {
    case 1:
        frms=_effect.setup->frames*2;
        frame1= _effect.direction<0? 0 : _effect.setup->frames;
        frameE= _effect.direction<0? _effect.setup->frames-1: -1;
        break;
    case 2:
        frms=_effect.setup->frames*4;
        frame1= _effect.direction<0 ? 0 :  (int)(frms-(_effect.setup->frames)*3);
        frameE= _effect.direction<0 ?  (int)(frms-(_effect.setup->frames)*3)-1 : (frms/2)-1;
        break;
    case 3:
        break;
    default:
    case 0:
        frms=_effect.setup->frames;
        frame1=0;
        frameE=-1;
        break;
    }

    WriteToLog(QtDebugMsg, QString("Effect-%1 FST%2, FRM-%3  (%4..%5)").arg(effect_def.id).arg(_effect.frameStyle).arg(frms)
               .arg(frame1).arg(frameE));

    _effect.animator.construct(true, frms, _effect.setup->framespeed, frame1, frameE);
    _effect.posRect.setSize(_effect.texture.w, _effect.texture.h/frms);

    if(centered)
        _effect.posRect.setPos(effect_def.startX-_effect.posRect.width()/2,
                               effect_def.startY-_effect.posRect.height()/2);
    else
        _effect.posRect.setPos(effect_def.startX, effect_def.startY);

    if( effect_def.delay > 0 )
    {
        _effect._limit_delay=true;
        _effect._delay = effect_def.delay;
    }

    _effect.startup_delay = effect_def.start_delay;

    if( effect_def.animationLoops > 0 )
    {
        _effect.animator.setOnceMode(true, effect_def.animationLoops);
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


const float Scene_Effect::timeStep= (1000.f/65.f);

Scene_Effect::Scene_Effect()
{
     direction=0;
     frameStyle=0;

     startup_delay = 0.0f;

     _limit_delay=false;
     _delay = 0.0f;

     m_velocityX=0;
     m_velocityY=0;

     gravity=0;

     _finished=false;
     setup=NULL;
}

Scene_Effect::Scene_Effect(const Scene_Effect &e)
{
    setup       = e.setup;
    direction   = e.direction;

    startup_delay = e.startup_delay;

    animator    = e.animator;
    texture     = e.texture;

    _limit_delay= e._limit_delay;
    _delay      = e._delay;

    m_velocityX = e.m_velocityX;
    m_velocityY = e.m_velocityY;
    phys_setup  = e.phys_setup;
    gravity     = e.gravity;

    frameStyle  = e.frameStyle;

    posRect     = e.posRect;
    _finished   = e._finished;
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
    if(startup_delay > 0)
    {
        startup_delay -= ticks;
        if(startup_delay <= 0)
            ticks = (-startup_delay);
        else
            return;
        if(ticks==0)
            return;
    }

    animator.manualTick(ticks);
    if(_limit_delay)
    {
        _delay-=ticks;
        //I changed this because we don't want the animator to stop our effect if we specify a custom delay also!
        if((_delay<=0.0f) & animator.isFinished()) _finished=true;
    }
    if(!_limit_delay & animator.isFinished())
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
    if((phys_setup.min_vel_x!=0)&&(m_velocityX<phys_setup.min_vel_x)) m_velocityX=phys_setup.min_vel_x;
    if((phys_setup.max_vel_y!=0)&&(m_velocityY>phys_setup.max_vel_y)) m_velocityY=phys_setup.max_vel_y;
    if((phys_setup.min_vel_y!=0)&&(m_velocityY<phys_setup.min_vel_y)) m_velocityY=phys_setup.min_vel_y;
}

void Scene_Effect::render(double camX, double camY)
{
    if(startup_delay>0)
        return;

    AniPos x(0,1);
    x = animator.image();
    GlRenderer::renderTexture(&texture, posX()-camX, posY()-camY, posRect.width(), posRect.height(), x.first, x.second);
}



Scene_Effect_Phys::Scene_Effect_Phys()
{
    min_vel_x=0;
    min_vel_y=0;
    max_vel_x=0;
    max_vel_y=0;
    decelerate_x=0;
    decelerate_y=0;
}
