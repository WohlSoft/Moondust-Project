/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_base_object.h"
#include <scenes/scene_level.h>
#include <graphics/gl_renderer.h>
#include <Utils/maths.h>

#include "lvl_subtree.h"

double PGE_Phys_Object::SMBXTicksToTime(double ticks)
{
    return ticks * m_smbxTickTime;
}

PGE_Phys_Object::PGE_Phys_Object(LevelScene *_parent) :
    PGE_physBody(),
    /*****Renderer flags*******/
    m_scene(_parent),
    /*****Renderer flags*END***/
    type(LVLUnknown),
    m_treemap(this)
{
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

    texId = 0;
    z_index = 0.0L;
    m_slippery_surface = false;
    m_parentSection = NULL;
    m_paused = false;
    m_is_visible = true;
    m_accelX = 0;
    m_accelY = 0;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if(m_treemap.m_is_registered)
    {
        m_treemap.delFromScene();
    }
}

void PGE_Phys_Object::registerInTree()
{
    m_treemap.addToScene();
}

void PGE_Phys_Object::unregisterFromTree()
{
    m_treemap.delFromScene();
}

double PGE_Phys_Object::posX()
{
//    LVL_SubTree *st = nullptr;
//    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
//    {
//        return m_momentum_relative.x - st->m_offsetX;
//    }
    return m_momentum.x;
}

double PGE_Phys_Object::posXrelative()
{
    return m_momentum_relative.x;
}

double PGE_Phys_Object::posY()
{
//    LVL_SubTree *st = nullptr;
//    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
//    {
//        return m_momentum_relative.y - st->m_offsetY;
//    }
    return m_momentum.y;
}

double PGE_Phys_Object::posYrelative()
{
    return m_momentum_relative.y;
}

double PGE_Phys_Object::posCenterX()
{
//    LVL_SubTree *st = nullptr;
//    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
//    {
//        return m_momentum_relative.centerX() - st->m_offsetX;
//    }
    return m_momentum.centerX();
}

double PGE_Phys_Object::posCenterXrelative()
{
    return m_momentum_relative.centerX();
}

double PGE_Phys_Object::posCenterY()
{
//    LVL_SubTree *st = nullptr;
//    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
//    {
//        return m_momentum_relative.centerY() - st->m_offsetY;
//    }
    return m_momentum.centerY();
}

double PGE_Phys_Object::posCenterYrelative()
{
    return m_momentum_relative.centerY();
}

void PGE_Phys_Object::setCenterX(double x)
{
    m_momentum.setCenterX(x);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.x = m_momentum.x + st->m_offsetX;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setCenterXrelative(double x)
{
    m_momentum.setCenterX(x);
    m_momentum_relative.setCenterX(x);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setCenterY(double y)
{
    m_momentum.setCenterY(y);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.y = m_momentum.y + st->m_offsetY;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setCenterYrelative(double y)
{
    m_momentum.setCenterY(y);
    m_momentum_relative.setCenterY(y);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
    }
    m_treemap.updatePos();
}

double PGE_Phys_Object::width()
{
    return m_momentum.w;
}

double PGE_Phys_Object::widthRelative()
{
    return m_momentum_relative.w;
}

double PGE_Phys_Object::height()
{
    return m_momentum.h;
}

double PGE_Phys_Object::heightRelative()
{
    return m_momentum_relative.h;
}

double PGE_Phys_Object::top()
{
    return posY();
}

double PGE_Phys_Object::topRelative()
{
    return posYrelative();
}

void PGE_Phys_Object::setTop(double tp)
{
    m_momentum.setTop(tp);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.y = m_momentum.y + st->m_offsetY;
    }
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setTopRelative(double tp)
{
    m_momentum.setTop(tp);
    m_momentum_relative.setTop(tp);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
    }
    m_treemap.updatePosAndSize();
}

double PGE_Phys_Object::bottom()
{
    return m_momentum.bottom();
}

double PGE_Phys_Object::bottomRelative()
{
    return m_momentum_relative.bottom();
}

void PGE_Phys_Object::setBottom(double btm)
{
    m_momentum.setBottom(btm);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.y = m_momentum.y + st->m_offsetY;
        m_momentum_relative.h = m_momentum.h;
    }
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setBottomRelative(double btm)
{
    m_momentum.setBottom(btm);
    m_momentum_relative.setBottom(btm);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
        m_momentum.h = m_momentum_relative.h;
    }
    m_treemap.updatePosAndSize();
}

double PGE_Phys_Object::left()
{
    return posX();
}

double PGE_Phys_Object::leftRelative()
{
    return posXrelative();
}

void PGE_Phys_Object::setLeft(double lf)
{
    m_momentum.setLeft(lf);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.x = m_momentum.x + st->m_offsetX;
    }
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setLeftRelative(double lf)
{
    m_momentum.setLeft(lf);
    m_momentum_relative.setLeft(lf);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
    }
    m_treemap.updatePosAndSize();
}

double PGE_Phys_Object::right()
{
    return m_momentum.right();
}

double PGE_Phys_Object::rightRelative()
{
    return m_momentum_relative.right();
}

void PGE_Phys_Object::setRight(double rt)
{
    m_momentum.setRight(rt);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.x = m_momentum.x + st->m_offsetX;
        m_momentum_relative.w = m_momentum.w;
    }
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setRightRelative(double rt)
{
    m_momentum.setRight(rt);
    m_momentum_relative.setRight(rt);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
        m_momentum.w = m_momentum_relative.w;
    }
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setSize(double w, double h)
{
    m_momentum.w = w;
    m_momentum.h = h;
    m_momentum_relative.w = w;
    m_momentum_relative.w = w;
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setWidth(double w)
{
    m_momentum.w = w;
    m_momentum_relative.w = w;
    m_treemap.updatePosAndSize();
}

void PGE_Phys_Object::setHeight(double h)
{
    m_momentum.h = h;
    m_momentum_relative.h = h;
    m_treemap.updatePosAndSize();
}

double PGE_Phys_Object::luaPosX()
{
    if(m_parent)
        return m_momentum_relative.x;
    else
        return m_momentum.x;
}

double PGE_Phys_Object::luaPosY()
{
    if(m_parent)
        return m_momentum_relative.y;
    else
        return m_momentum.y;
}

double PGE_Phys_Object::luaWidth()
{
    if(m_parent)
        return m_momentum_relative.w;
    else
        return m_momentum.w;
}

double PGE_Phys_Object::luaHeight()
{
    if(m_parent)
        return m_momentum_relative.h;
    else
        return m_momentum.h;
}

double PGE_Phys_Object::luaPosCenterX()
{
    if(m_parent)
        return m_momentum_relative.centerX();
    else
        return m_momentum.centerX();
}

double PGE_Phys_Object::luaPosCenterY()
{
    if(m_parent)
        return m_momentum_relative.centerY();
    else
        return m_momentum.centerY();
}

double PGE_Phys_Object::luaLeft()
{
    if(m_parent)
        return m_momentum_relative.left();
    else
        return m_momentum.left();
}

double PGE_Phys_Object::luaTop()
{
    if(m_parent)
        return m_momentum_relative.top();
    else
        return m_momentum.top();
}

double PGE_Phys_Object::luaRight()
{
    if(m_parent)
        return m_momentum_relative.right();
    else
        return m_momentum.right();
}

double PGE_Phys_Object::luaBottom()
{
    if(m_parent)
        return m_momentum_relative.bottom();
    else
        return m_momentum.bottom();
}

void PGE_Phys_Object::luaSetPosX(double x)
{
    if(m_parent)
        setPosXrelative(x);
    else
        setPosX(x);
}

void PGE_Phys_Object::luaSetPosY(double y)
{
    if(m_parent)
        setPosYrelative(y);
    else
        setPosY(y);
}

void PGE_Phys_Object::luaSetWidth(double w)
{
    setWidth(w);
}

void PGE_Phys_Object::luaSetHeight(double h)
{
    setHeight(h);
}

void PGE_Phys_Object::luaSetPosCenterX(double x)
{
    if(m_parent)
        setCenterXrelative(x);
    else
        setCenterX(x);
}

void PGE_Phys_Object::luaSetPosCenterY(double y)
{
    if(m_parent)
        setCenterYrelative(y);
    else
        setCenterY(y);
}

void PGE_Phys_Object::luaSetPos(double x, double y)
{
    if(m_parent)
        setRelativePos(x, y);
    else
        setPos(x, y);
}

void PGE_Phys_Object::luaSetCenterPos(double x, double y)
{
    if(m_parent)
        setRelativeCenterPos(x, y);
    else
        setCenterPos(x, y);
}

void PGE_Phys_Object::luaSetCenterX(double x)
{
    if(m_parent)
        setCenterXrelative(x);
    else
        setCenterX(x);
}

void PGE_Phys_Object::luaSetCenterY(double y)
{
    if(m_parent)
        setCenterYrelative(y);
    else
        setCenterY(y);
}

void PGE_Phys_Object::luaSetSize(double w, double h)
{
    setSize(w, h);
}

void PGE_Phys_Object::luaSetLeft(double l)
{
    if(m_parent)
        setLeftRelative(l);
    else
        setLeft(l);
}

void PGE_Phys_Object::luaSetTop(double t)
{
    if(m_parent)
        setTopRelative(t);
    else
        setTop(t);
}

void PGE_Phys_Object::luaSetRight(double r)
{
    if(m_parent)
        setRightRelative(r);
    else
        setRight(r);
}

void PGE_Phys_Object::luaSetBottom(double b)
{
    if(m_parent)
        setBottomRelative(b);
    else
        setBottom(b);
}

void PGE_Phys_Object::setPos(double x, double y)
{
    m_momentum.x = x;
    m_momentum.y = y;
    m_treemap.updatePos();
}

void PGE_Phys_Object::setRelativePos(double x, double y)
{
    m_momentum.x = x;
    m_momentum.y = y;
    m_momentum_relative.x = x;
    m_momentum_relative.y = y;
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setPosX(double x)
{
    m_momentum.x = x;
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.x = m_momentum.x + st->m_offsetX;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setPosXrelative(double x)
{
    m_momentum.x = x;
    m_momentum_relative.x = x;
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setPosY(double y)
{
    m_momentum.y = y;
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.y = m_momentum.y + st->m_offsetY;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setPosYrelative(double y)
{
    m_momentum.y = y;
    m_momentum_relative.y = y;
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setCenterPos(double x, double y)
{
    m_momentum.setCenterPos(x, y);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum_relative.x = m_momentum.x + st->m_offsetX;
        m_momentum_relative.y = m_momentum.y + st->m_offsetY;
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::setRelativeCenterPos(double x, double y)
{
    m_momentum.setCenterPos(x, y);
    m_momentum_relative.setCenterPos(x, y);
    LVL_SubTree *st = nullptr;
    if(m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_parent)) != nullptr))
    {
        m_momentum.x = m_momentum_relative.x - st->m_offsetX;
        m_momentum.y = m_momentum_relative.y - st->m_offsetY;
    }
    m_treemap.updatePos();
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
    m_momentum_relative.velXsrc = x;
    m_momentum.velY = y;
    m_momentum_relative.velY = y;
}

void PGE_Phys_Object::setSpeedX(double x)
{
    m_momentum.velXsrc = x;
    m_momentum_relative.velXsrc = x;
}

void PGE_Phys_Object::setSpeedY(double y)
{
    m_momentum.velY = y;
    m_momentum_relative.velY = y;
}

void PGE_Phys_Object::setDecelX(double x)
{
    phys_setup.decelerate_x = x;
}

void PGE_Phys_Object::setDecelY(double y)
{
    phys_setup.decelerate_y = y;
}


void PGE_Phys_Object::applyAccel(double x, double y)
{
    m_accelX = x;
    m_accelY = y;
}

//void PGE_Phys_Object::_syncPosition()
//{
//    LVL_SubTree *st = dynamic_cast<LVL_SubTree *>(m_parent);
//    if(_is_registered && m_parent && st)
//        st->unregisterElement(this);
////FIXME: Implement a valid way to process offseted position change
//    //if(st)
//    //{
//    //    m_momentum_relative = m_momentum;
//    //    m_momentum_relative.x += st->m_offsetX;
//    //    m_momentum_relative.y += st->m_offsetY;
//    //    m_momentum_relative.saveOld();
//    //    m_posX_registered = m_momentum_relative.x;
//    //    m_posY_registered = m_momentum_relative.y;
//    //} else {
//    //    m_posX_registered = m_momentum.x;
//    //    m_posY_registered = m_momentum.y;
//    //}
//    m_treemap.m_posX_registered = m_momentum.x;
//    m_treemap.m_posY_registered = m_momentum.y;
//    if(m_parent && st)
//        st->registerElement(this);
//    else
//        m_scene->updateElement(this);
//    _is_registered = true;
//}

//void PGE_Phys_Object::_syncPositionAndSize()
//{
//    LVL_SubTree *st = dynamic_cast<LVL_SubTree *>(m_parent);
//    if(_is_registered && m_parent && st)
//        st->unregisterElement(this);
////FIXME: Implement a valid way to process offseted position change
////    if(st)
////    {
////        m_momentum_relative = m_momentum;
////        m_momentum_relative.x += st->m_offsetX;
////        m_momentum_relative.y += st->m_offsetY;
////        m_momentum_relative.saveOld();
////        m_posX_registered = m_momentum_relative.x;
////        m_posY_registered = m_momentum_relative.y;
////    } else {
////        m_posX_registered = m_momentum.x;
////        m_posY_registered = m_momentum.y;
////    }

//    m_treemap.m_posX_registered = m_momentum.x;
//    m_treemap.m_posY_registered = m_momentum.y;
//    m_treemap.m_width_registered = m_width_toRegister;
//    m_treemap.m_height_registered = m_height_toRegister;
//    if(m_parent && st)
//        st->registerElement(this);
//    else
//        m_scene->updateElement(this);
//    _is_registered = true;
//}

void PGE_Phys_Object::_syncSection(bool sync_position)
{
    int sID = m_scene->findNearestSection(long(posX()), long(posY()));
    LVL_Section *sct = m_scene->getSection(sID);
    if(sct)
        setParentSection(sct);
    if(sync_position) m_treemap.updatePos();
}

void PGE_Phys_Object::renderDebug(double _camX, double _camY)
{
    switch(type)
    {
    case LVLUnknown:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
        break;
    case LVLSubTree:
        GlRenderer::renderRect(float(m_momentum.x - _camX), float(m_momentum.y - _camY), float(m_momentum.w) - 1.0f, float(m_momentum.h) - 1.0f, 0.0f, 1.0f, 0.0f, 0.2f, true);
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
    m_parentSection = sct;
}

LVL_Section *PGE_Phys_Object::sct()
{
    return m_parentSection;
}

long double PGE_Phys_Object::zIndex()
{
    return z_index;
}

void PGE_Phys_Object::update()
{
    m_treemap.updatePos();
}
void PGE_Phys_Object::update(double)
{
    m_treemap.updatePos();
}






bool operator<(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{
    return lhs.z_index > rhs.z_index;
}


bool operator>(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{
    return lhs.z_index < rhs.z_index;
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
