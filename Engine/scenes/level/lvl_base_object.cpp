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

#include "lvl_base_object.h"
#include <scenes/scene_level.h>
#include <graphics/gl_renderer.h>
#include <common_features/maths.h>

#include <QVector>

const double PGE_Phys_Object::m_smbxTickTime = 1000.0 / 65.0/*15.285f*/;
//1000.f/65.f; Thanks to Rednaxela for hint, 15.6 is a true frame time in SMBX Engine!
//BUT, Experimentally was found that in real is approximately is 15.285

double PGE_Phys_Object::SMBXTicksToTime(double ticks)
{
    return ticks * m_smbxTickTime;
}

PGE_Phys_Object::PGE_Phys_Object(LevelScene *_parent) :
    PGE_physBody(),

    /*****Renderer flags*******/
    _vizible_on_screen(false),
    _render_list(false),
    _scene(_parent),
    _is_registered(false),
    /*****Renderer flags*END***/

    m_posX_registered(0.0),
    m_posY_registered(0.0),
    m_width_registered(1.0),
    m_height_registered(1.0),
    m_width_half(0.5),
    m_height_half(0.5),
    m_width_toRegister(1.0),
    m_height_toRegister(1.0),
    type(LVLUnknown)
{
    m_width_half = 0.0;
    m_height_half = 0.0;

    switch(type)
    {
    case LVLPlayer:
        m_filterID = 1;
        break;

    case LVLNPC:
        m_filterID = 2;
        break;

    default:
        break;
    }

    z_index = 0.0L;
    m_slippery_surface = false;
    _parentSection = NULL;
    m_width_registered = 0.0;
    m_height_registered = 0.0;
    m_width_toRegister = 0.0;
    m_height_toRegister = 0.0;
    m_posX_registered = 0.0;
    m_posY_registered = 0.0;
    m_paused = false;
    m_is_visible = true;
    m_accelX = 0;
    m_accelY = 0;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if(_is_registered) _scene->unregisterElement(this);
}

void PGE_Phys_Object::registerInTree()
{
    if(!_is_registered) _scene->registerElement(this);

    _is_registered = true;
}

void PGE_Phys_Object::unregisterFromTree()
{
    if(_is_registered) _scene->unregisterElement(this);

    _is_registered = false;
}

double PGE_Phys_Object::posX()
{
    return m_momentum.x;
}

double PGE_Phys_Object::posY()
{
    return m_momentum.y;
}

double PGE_Phys_Object::posCenterX()
{
    return m_momentum.centerX();
}

double PGE_Phys_Object::posCenterY()
{
    return m_momentum.centerY();
}

void PGE_Phys_Object::setCenterX(double x)
{
    m_momentum.setCenterX(x);
    _syncPosition();
}

void PGE_Phys_Object::setCenterY(double y)
{
    m_momentum.setCenterY(y);
    _syncPosition();
}

double PGE_Phys_Object::width()
{
    return m_momentum.w;
}

double PGE_Phys_Object::height()
{
    return m_momentum.h;
}

double PGE_Phys_Object::top()
{
    return posY();
}

void PGE_Phys_Object::setTop(double tp)
{
    m_momentum.setTop(tp);
    m_height_toRegister = m_momentum.h;
    m_height_half = m_height_toRegister / 2.0;
    _syncPositionAndSize();
}

double PGE_Phys_Object::bottom()
{
    return posY() + m_height_registered;
}

void PGE_Phys_Object::setBottom(double btm)
{
    m_momentum.setBottom(btm);
    m_height_toRegister = m_momentum.h;
    m_height_half = m_height_toRegister / 2.0;
    _syncPositionAndSize();
}

double PGE_Phys_Object::left()
{
    return posX();
}

void PGE_Phys_Object::setLeft(double lf)
{
    m_momentum.setLeft(lf);
    m_width_toRegister = m_momentum.w;
    m_height_half = m_width_toRegister / 2.0;
    _syncPositionAndSize();
}

double PGE_Phys_Object::right()
{
    return m_momentum.right();
}

void PGE_Phys_Object::setRight(double rt)
{
    m_momentum.setRight(rt);
    m_width_toRegister = m_momentum.w;
    m_height_half = m_width_toRegister / 2.0;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setSize(double w, double h)
{
    m_momentum.w = w;
    m_momentum.h = h;
    m_width_toRegister = w;
    m_height_toRegister = h;
    m_width_half = m_width_toRegister / 2.0;
    m_height_half = m_height_toRegister / 2.0;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setWidth(double w)
{
    m_momentum.w = w;
    m_width_toRegister = w;
    m_width_half = m_width_toRegister / 2.0;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setHeight(double h)
{
    m_momentum.h = h;
    m_height_toRegister = h;
    m_height_half = m_height_toRegister / 2.0;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setPos(double x, double y)
{
    m_momentum.x = x;
    m_momentum.y = y;
    _syncPosition();
}

void PGE_Phys_Object::setPosX(double x)
{
    m_momentum.x = x;
    _syncPosition();
}

void PGE_Phys_Object::setPosY(double y)
{
    m_momentum.y = y;
    _syncPosition();
}

void PGE_Phys_Object::setCenterPos(double x, double y)
{
    m_momentum.setCenterPos(x, y);
    _syncPosition();
}

double PGE_Phys_Object::speedX()
{
    return m_momentum.velXsrc;
}

double PGE_Phys_Object::speedY()
{
    return m_momentum.velY;
}

double PGE_Phys_Object::speedXsum()
{
    return m_momentum.velX;
}

double PGE_Phys_Object::speedYsum()
{
    return m_momentum.velY;
}

void PGE_Phys_Object::setSpeed(double x, double y)
{
    m_momentum.velXsrc = x;
    m_momentum.velY = y;
}

void PGE_Phys_Object::setSpeedX(double x)
{
    m_momentum.velXsrc = x;
}

void PGE_Phys_Object::setSpeedY(double y)
{
    m_momentum.velY = y;
}

void PGE_Phys_Object::setDecelX(double x)
{
    phys_setup.decelerate_x = x;
}

void PGE_Phys_Object::applyAccel(double x, double y)
{
    m_accelX = x;
    m_accelY = y;
}

void PGE_Phys_Object::_syncPosition()
{
    if(_is_registered) _scene->unregisterElement(this);

    m_posX_registered = m_momentum.x;
    m_posY_registered = m_momentum.y;
    _scene->registerElement(this);
    _is_registered = true;
}

void PGE_Phys_Object::_syncPositionAndSize()
{
    if(_is_registered) _scene->unregisterElement(this);

    m_posX_registered = m_momentum.x;
    m_posY_registered = m_momentum.y;
    m_width_registered = m_width_toRegister;
    m_height_registered = m_height_toRegister;
    _scene->registerElement(this);
    _is_registered = true;
}

void PGE_Phys_Object::_syncSection(bool sync_position)
{
    int sID = _scene->findNearestSection(long(posX()), long(posY()));
    LVL_Section *sct = _scene->getSection(sID);

    if(sct)
        setParentSection(sct);

    if(sync_position) _syncPosition();
}

void PGE_Phys_Object::renderDebug(double _camX, double _camY)
{
    switch(type)
    {
    case LVLUnknown:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
        break;

    case LVLBlock:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, false);
        break;

    case LVLBGO:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, false);
        break;

    case LVLNPC:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, false);
        break;

    case LVLPlayer:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, false);
        break;

    case LVLEffect:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, false);
        break;

    case LVLWarp:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 0.0f, 0.0f, 0.5f, true);
        break;

    case LVLSpecial:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, true);
        break;

    case LVLPhysEnv:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, true);
        break;
    }
}

void PGE_Phys_Object::setParentSection(LVL_Section *sct)
{
    _parentSection = sct;
}

LVL_Section *PGE_Phys_Object::sct()
{
    return _parentSection;
}

long double PGE_Phys_Object::zIndex()
{
    return z_index;
}

void PGE_Phys_Object::update()
{
    _syncPosition();
}
void PGE_Phys_Object::update(double)
{
    _syncPosition();
}






bool operator<(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{
    return lhs.z_index > rhs.z_index;
}


bool operator>(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{
    return lhs.z_index < rhs.z_index;
}

PGE_Phys_Object_Phys::PGE_Phys_Object_Phys()
{
    min_vel_x = 0.0;
    min_vel_y = 0.0;
    max_vel_x = 0.0;
    max_vel_y = 0.0;
    grd_dec_x = 0.0;
    decelerate_x = 0.0;
    decelerate_y = 0.0;
    gravityScale = 1.0;
    gravityAccel = 26.0;
}

void PGE_Phys_Object::show()
{
    m_is_visible = true;
}

void PGE_Phys_Object::hide()
{
    m_is_visible = false;
}

void PGE_Phys_Object::setVisible(bool vizible)
{
    m_is_visible = vizible;
}

bool PGE_Phys_Object::isVisible()
{
    return m_is_visible;
}
