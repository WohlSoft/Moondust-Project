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

#include "gfx_effect.h"
#include <data_configs/config_manager.h>
#include <common_features/logger.h>
#include <graphics/gl_renderer.h>
#include <gui/pge_msgbox.h>
#include <common_features/fmt_format_ne.h>
#include <Utils/maths.h>

void Scene::launchEffect(unsigned long effectID,
                         double   startX,
                         double   startY,
                         int      animationLoops,
                         int      delay,
                         double   velocityX,
                         double   velocityY,
                         double   gravity,
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
    launchEffect(effectDef, false);
}

void Scene::launchStaticEffectC(unsigned long    effectID,
                                double  startX,
                                double  startY,
                                int     animationLoops,
                                int     delay,
                                double  velocityX,
                                double  velocityY,
                                double  gravity,
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
    launchEffect(effectDef, true);
}

void Scene::launchEffect(const SpawnEffectDef &effect_def, bool centered)
{
    Scene_Effect _effect;

    if(ConfigManager::lvl_effects_indexes.contains(effect_def.id))
        _effect.m_setup = &ConfigManager::lvl_effects_indexes[effect_def.id];
    else
    {
        PGE_MsgBox oops(this, fmt::format_ne("Can't launch effect {0}", effect_def.id), PGE_MsgBox::msg_error);
        oops.exec();
        return;
    }

    long tID = ConfigManager::getEffectTexture(effect_def.id);

    if(tID >= 0)
        _effect.m_texture = ConfigManager::level_textures[size_t(tID)];
    else
    {
        PGE_MsgBox oops(this, fmt::format_ne("Can't load texture for effect-{0}", effect_def.id), PGE_MsgBox::msg_error);
        oops.exec();
        return;
    }

    _effect.m_velocityX = effect_def.velocityX;
    _effect.m_velocityY = effect_def.velocityY;
    _effect.m_zIndex    = effect_def.zIndex;
    _effect.m_phys_setup.decelerate_x = effect_def.decelerate_x;
    _effect.m_phys_setup.decelerate_y = effect_def.decelerate_y;
    _effect.m_phys_setup.max_vel_x = effect_def.max_vel_x;
    _effect.m_phys_setup.max_vel_y = effect_def.max_vel_y;
    _effect.m_phys_setup.min_vel_x = effect_def.min_vel_x;
    _effect.m_phys_setup.min_vel_y = effect_def.min_vel_y;
    _effect.m_gravity = effect_def.gravity;

    _effect.m_direction = effect_def.direction;
    if(_effect.m_direction == 0)
        _effect.m_direction = (Maths::rand32() % 2) ? -1 : 1;

    _effect.m_frameStyle = _effect.m_setup->framestyle;
    int32_t frame1 = 0;
    int32_t frameE = -1;
    int32_t frms = 0;
    frms = int32_t(_effect.m_setup->frames);

    switch(_effect.m_frameStyle)
    {
    case 1:
        frms  = int32_t(_effect.m_setup->frames) * 2;
        frame1 = _effect.m_direction < 0 ? 0 : int(_effect.m_setup->frames);
        frameE = _effect.m_direction < 0 ? int(_effect.m_setup->frames) - 1 : -1;
        break;

    case 2:
        frms  = int32_t(_effect.m_setup->frames) * 4;
        frame1 = int32_t(_effect.m_direction) < 0 ? 0 : (frms - (int32_t(_effect.m_setup->frames)) * 3);
        frameE = int32_t(_effect.m_direction) < 0 ? (frms - (int32_t(_effect.m_setup->frames)) * 3) - 1 : (frms / 2) - 1;
        break;

    case 3:
        break;

    default:
    case 0:
        frms = int32_t(_effect.m_setup->frames);
        frame1 = 0;
        frameE = -1;
        break;
    }

    pLogDebug(fmt::format_ne("Effect-{0} FST{1}, FRM-{2}  ({3}..{4})", effect_def.id, _effect.m_frameStyle, frms, frame1, frameE).c_str());

    if(frms <= 0)
        frms = 1;
    _effect.m_animator.construct(true, frms, _effect.m_setup->framespeed, frame1, frameE);
    _effect.m_posRect.setSize(_effect.m_texture.w, _effect.m_texture.h / frms);

    if(!effect_def.frame_sequence.empty())
    {
        frms = static_cast<int32_t>(effect_def.frame_sequence.size());
        _effect.m_animator.setFrameSequance(effect_def.frame_sequence);
    }

    if(centered)
        _effect.m_posRect.setPos(effect_def.startX - _effect.m_posRect.width() / 2,
                                 effect_def.startY - _effect.m_posRect.height() / 2);
    else
        _effect.m_posRect.setPos(effect_def.startX, effect_def.startY);

    if(effect_def.delay > 0)
    {
        _effect.m_limitLifeTime = true;
        _effect.m_lifeTime = effect_def.delay;
    }

    _effect.m_startupDelay = effect_def.start_delay;

    if(effect_def.animationLoops > 0)
    {
        _effect.m_animator.setOnceMode(true, effect_def.animationLoops);
        _effect.m_limitLoops = true;
    }

    _effect.init();
    WorkingEffects.push_back(_effect);
}


void Scene::processEffects(double ticks)
{
    for(size_t i = 0; i < WorkingEffects.size(); i++)
    {
        Scene_Effect &e = WorkingEffects[i];
        e.update(ticks);

        if(e.finished() || (!e.m_limitLifeTime && !e.m_limitLoops && !isVizibleOnScreen(e.m_posRect)))
        {
            WorkingEffects.erase(WorkingEffects.begin() + int(i--));
        }
    }
}


const double Scene_Effect::timeStep = 15.6;
//1000.f/65.f; Thanks to Rednaxela for hint, 15.6 is a true frame time in SMBX Engine!

Scene_Effect::Scene_Effect() :
    m_startupDelay(0.0),
    m_zIndex(-5.0L),
    m_direction(0),
    m_frameStyle(0),
    m_limitLifeTime(false),
    m_lifeTime(0.0),
    m_velocityX(0.0),
    m_velocityY(0.0),
    m_gravity(0.0),
    m_finished(false),
    m_setup(nullptr),
    m_limitLoops(false)
{}

Scene_Effect::Scene_Effect(const Scene_Effect &e) :
    m_startupDelay(e.m_startupDelay),
    m_zIndex(e.m_zIndex),
    m_direction(e.m_direction),
    m_frameStyle(e.m_frameStyle),
    m_limitLifeTime(e.m_limitLifeTime),
    m_lifeTime(e.m_lifeTime),
    m_velocityX(e.m_velocityX),
    m_velocityY(e.m_velocityY),
    m_gravity(e.m_gravity),
    m_phys_setup(e.m_phys_setup),
    m_posRect(e.m_posRect),
    m_finished(e.m_finished),
    m_setup(e.m_setup),
    m_texture(e.m_texture),
    m_limitLoops(e.m_limitLoops),
    m_animator(e.m_animator)
{}

Scene_Effect::~Scene_Effect()
{}

void Scene_Effect::init()
{}

double Scene_Effect::posX()
{
    return m_posRect.x();
}

double Scene_Effect::posY()
{
    return m_posRect.y();
}

bool Scene_Effect::finished()
{
    return m_finished;
}

void Scene_Effect::update(double ticks)
{
    if(m_startupDelay > 0)
    {
        m_startupDelay -= ticks;

        if(m_startupDelay <= 0)
            ticks = (-m_startupDelay);
        else
            return;

        if(ticks == 0.0)
            return;
    }

    m_animator.manualTick(ticks);

    if(m_limitLifeTime)
    {
        m_lifeTime -= ticks;

        //I changed this because we don't want the animator to stop our effect if we specify a custom delay also!
        if((m_lifeTime <= 0.0) || m_animator.isFinished())
            m_finished = true;
    }

    if(!m_limitLifeTime & m_animator.isFinished())
        m_finished = true;

    iterateStep(ticks);
}

void Scene_Effect::iterateStep(double ticks)
{
    m_posRect.setX(m_posRect.x() + m_velocityX * (ticks / timeStep));
    m_posRect.setY(m_posRect.y() + m_velocityY * (ticks / timeStep));
    double accelCof = ticks / 1000.0;

    if(m_phys_setup.decelerate_x != 0.0)
    {
        double decX = m_phys_setup.decelerate_x * accelCof;

        if(m_velocityX > 0)
        {
            if((m_velocityX - decX > 0.0))
                m_velocityX -= decX;
            else
                m_velocityX = 0;
        }
        else if(m_velocityX < 0)
        {
            if((m_velocityX + decX < 0.0))
                m_velocityX += decX;
            else
                m_velocityX = 0;
        }
    }

    if(m_phys_setup.decelerate_y != 0.0)
    {
        double decY = m_phys_setup.decelerate_y * accelCof;

        if(m_velocityY > 0)
        {
            if((m_velocityY - decY > 0.0))
                m_velocityY -= decY;
            else
                m_velocityY = 0;
        }
        else if(m_velocityY < 0)
        {
            if((m_velocityY + decY < 0.0))
                m_velocityY += decY;
            else
                m_velocityY = 0;
        }
    }

    if(m_gravity != 0.0)
        m_velocityY += m_gravity * accelCof;

    if((m_phys_setup.max_vel_x != 0.0) && (m_velocityX > m_phys_setup.max_vel_x)) m_velocityX = m_phys_setup.max_vel_x;

    if((m_phys_setup.min_vel_x != 0.0) && (m_velocityX < m_phys_setup.min_vel_x)) m_velocityX = m_phys_setup.min_vel_x;

    if((m_phys_setup.max_vel_y != 0.0) && (m_velocityY > m_phys_setup.max_vel_y)) m_velocityY = m_phys_setup.max_vel_y;

    if((m_phys_setup.min_vel_y != 0.0) && (m_velocityY < m_phys_setup.min_vel_y)) m_velocityY = m_phys_setup.min_vel_y;
}

void Scene_Effect::render(double camX, double camY)
{
    if(m_startupDelay > 0)
        return;

    AniPos x(0, 1);
    x = m_animator.image();
    GlRenderer::renderTexture(&m_texture,
                              float(posX() - camX),
                              float(posY() - camY),
                              float(m_posRect.width()),
                              float(m_posRect.height()),
                              float(x.first),
                              float(x.second));
}

Scene_Effect_Phys::Scene_Effect_Phys()
{
    min_vel_x = 0;
    min_vel_y = 0;
    max_vel_x = 0;
    max_vel_y = 0;
    decelerate_x = 0;
    decelerate_y = 0;
}
