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

#include "lvl_base_object.h"
#include <scenes/level/lvl_scene_ptr.h>
#include <graphics/gl_renderer.h>

#include <QVector>

const float PGE_Phys_Object::_smbxTickTime=1000.f/65.f;

float PGE_Phys_Object::SMBXTicksToTime(float ticks)
{
    return ticks*_smbxTickTime;
}

PGE_Phys_Object::PGE_Phys_Object()
{
    _width_half = 0.0f;
    _height_half = 0.0f;

    z_index = 0.0;
    isRectangle = true;

    slippery_surface = false;
    collide_player = COLLISION_ANY;
    collide_npc = COLLISION_ANY;

    _parentSection=NULL;
    _width = 0.0f;
    _height = 0.0f;
    _realWidth=0.0f;
    _realHeight=0.0f;
    _posX=0.0f;
    _posY=0.0f;
    _velocityX=0.0f;
    _velocityY=0.0f;
    _velocityX_prev=0.0f;
    _velocityY_prev=0.0f;

    _paused=false;

    _accelX=0;
    _accelY=0;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if(LvlSceneP::s) LvlSceneP::s->unregisterElement(this);
}

double PGE_Phys_Object::posX()
{
    return posRect.x();
}

double PGE_Phys_Object::posY()
{
    return posRect.y();
}

double PGE_Phys_Object::posCenterX()
{
    return posRect.center().x();
}

double PGE_Phys_Object::posCenterY()
{
    return posRect.center().y();
}

void PGE_Phys_Object::setCenterX(double x)
{
    setPosX(x-_width_half);
}

void PGE_Phys_Object::setCenterY(double y)
{
    setPosY(y-_height_half);
}

double PGE_Phys_Object::width()
{
    return posRect.width();
}

double PGE_Phys_Object::height()
{
    return posRect.height();
}

double PGE_Phys_Object::top()
{
    return posY();
}

void PGE_Phys_Object::setTop(double tp)
{
    posRect.setTop(tp);
    _realHeight=posRect.height();
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::bottom()
{
    return posY()+_height;
}

void PGE_Phys_Object::setBottom(double btm)
{
    posRect.setBottom(btm);
    _realHeight=posRect.height();
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::left()
{
    return posX();
}

void PGE_Phys_Object::setLeft(double lf)
{
    posRect.setLeft(lf);
    _realWidth=posRect.width();
    _height_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::right()
{
    return posX()+_width;
}

void PGE_Phys_Object::setRight(double rt)
{
    posRect.setLeft(rt);
    _realWidth=posRect.width();
    _height_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setSize(float w, float h)
{
    posRect.setSize(w, h);
    _realWidth=w;
    _realHeight=h;
    _width_half = _realWidth/2.0f;
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setWidth(float w)
{
    posRect.setWidth(w);
    _realWidth=w;
    _width_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setHeight(float h)
{
    posRect.setHeight(h);
    _realHeight=h;
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setPos(double x, double y)
{
    posRect.setPos(x, y);
    _syncPosition();
}

void PGE_Phys_Object::setPosX(double x)
{
    posRect.setX(x);
    _syncPosition();
}

void PGE_Phys_Object::setPosY(double y)
{
    posRect.setY(y);
    _syncPosition();
}

void PGE_Phys_Object::setCenterPos(double x, double y)
{
    setPos(x-_width_half, y-_height_half);
}

double PGE_Phys_Object::speedX()
{
    return _velocityX;
}

double PGE_Phys_Object::speedY()
{
    return _velocityY;
}

void PGE_Phys_Object::setSpeed(double x, double y)
{
    _velocityX=x;
    _velocityY=y;
    _velocityX_prev=_velocityX;
    _velocityY_prev=_velocityY;
}

void PGE_Phys_Object::setSpeedX(double x)
{
    _velocityX=x;
    _velocityX_prev=_velocityX;
}

void PGE_Phys_Object::setSpeedY(double y)
{
    _velocityY=y;
    _velocityY_prev=_velocityY;
}

void PGE_Phys_Object::setDecelX(double x)
{
    phys_setup.decelerate_x=x;
}

void PGE_Phys_Object::applyAccel(double x, double y)
{
    _accelX=x;
    _accelY=y;
}

double PGE_Phys_Object::gravityScale()
{
    return phys_setup.gravityScale;
}

void PGE_Phys_Object::setGravityScale(double scl)
{
    phys_setup.gravityScale = scl;
}

float PGE_Phys_Object::gravityAccel()
{
    return phys_setup.gravityAccel;
}

void PGE_Phys_Object::setGravityAccel(float acl)
{
    phys_setup.gravityAccel=fabs(acl);
}


void PGE_Phys_Object::_syncPosition()
{
    if(LvlSceneP::s) LvlSceneP::s->unregisterElement(this);
    _posX= posRect.x();
    _posY= posRect.y();
    if(LvlSceneP::s) LvlSceneP::s->registerElement(this);
}

void PGE_Phys_Object::_syncPositionAndSize()
{
    if(LvlSceneP::s) LvlSceneP::s->unregisterElement(this);
    _posX= posRect.x();
    _posY= posRect.y();
    _width=_realWidth;
    _height=_realHeight;
    if(LvlSceneP::s) LvlSceneP::s->registerElement(this);
}

void PGE_Phys_Object::_syncSection()
{
    int sID = LvlSceneP::s->findNearestSection(posX(), posY());
    LVL_Section *sct = LvlSceneP::s->getSection(sID);
    if(sct)
    {
        setParentSection(sct);
    }
    _syncPosition();
}

void PGE_Phys_Object::renderDebug(float _camX, float _camY)
{
    switch(type)
    {
    case LVLUnknown:    GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0, 1.0, 1.0, 1.0, false); break;
    case LVLBlock:      GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 0.0, 1.0, 0.0, 1.0, false); break;
    case LVLBGO:        GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 0.0, 0.0, 1.0, 1.0, false); break;
    case LVLNPC:        GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0, 0.0, 1.0, 1.0, false); break;
    case LVLPlayer:     GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0, 0.5, 0.5, 1.0, false); break;
    case LVLEffect:     GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 0.5, 0.5, 0.5, 1.0, false); break;
    case LVLWarp:       GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0, 0.0, 0.0, 0.5f, true); break;
    case LVLSpecial:    GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0,1.0,0,1.0, true);
    case LVLPhysEnv:    GlRenderer::renderRect(posRect.x()-_camX, posRect.y()-_camY, posRect.width()-1.0, posRect.height()-1.0, 1.0f, 1.0f, 0.0f, 0.5f, true); break;
    }
}


void PGE_Phys_Object::iterateStep(float ticks)
{
    if(_paused) return;

    posRect.setX(posRect.x()+_velocityX * (ticks/_smbxTickTime));
    posRect.setY(posRect.y()+_velocityY * (ticks/_smbxTickTime));

    _velocityX_prev=_velocityX;
    _velocityY_prev=_velocityY;

    float G = phys_setup.gravityScale * LvlSceneP::s->globalGravity;
    float accelCof=ticks/1000.0f;

    if(_accelX!=0)
    {
        _velocityX+= _accelX*accelCof;
        _accelX=0;
    }
    else
    if(phys_setup.decelerate_x!=0)
    {
        float decX=phys_setup.decelerate_x*accelCof;
        if(_velocityX>0)
        {
            if((_velocityX-decX>0.0))
                _velocityX-=decX;
            else
                _velocityX=0;
        } else if(_velocityX<0) {
            if((_velocityX+decX<0.0))
                _velocityX+=decX;
            else
                _velocityX=0;
        }
    }

    if(_accelY!=0)
    {
        _velocityY+= _accelY*accelCof*G;
        _accelY=0;
    }

    if(phys_setup.decelerate_y!=0)
    {
        float decY=phys_setup.decelerate_y*accelCof;
        if(_velocityY>0)
        {
            if((_velocityY-decY>0.0))
                _velocityY-=decY;
            else
                _velocityY=0;
        } else if(_velocityY<0) {
            if((_velocityY+decY<0.0))
                _velocityY+=decY;
            else
                _velocityY=0;
        }
    }

    if(phys_setup.gravityAccel != 0.0f)
    {
        _velocityY+= (G*phys_setup.gravityAccel)*accelCof;
    }

    if((phys_setup.max_vel_x!=0)&&(_velocityX>phys_setup.max_vel_x)) _velocityX-=phys_setup.grd_dec_x*accelCof;
    if((phys_setup.min_vel_x!=0)&&(_velocityX<phys_setup.min_vel_x)) _velocityX+=phys_setup.grd_dec_x*accelCof;
    if((phys_setup.max_vel_y!=0)&&(_velocityY>phys_setup.max_vel_y)) _velocityY=phys_setup.max_vel_y;
    if((phys_setup.min_vel_y!=0)&&(_velocityY<phys_setup.min_vel_y)) _velocityY=phys_setup.min_vel_y;

}

void PGE_Phys_Object::updateCollisions()
{
    if(_paused) return;

    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = posRect.withMargin(2.0);
    LvlSceneP::s->queryItems(posRectC, &bodies);

    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*body=*it;
        if(body==this) continue;
        if(body->_paused) continue;

        solveCollision(body);
    }
}

void PGE_Phys_Object::solveCollision(PGE_Phys_Object *)
{}



bool PGE_Phys_Object::isWall(QVector<LVL_Block*> &blocks)
{
    if(blocks.isEmpty())
        return false;
    float higher=blocks.first()->posRect.top();
    float lower=blocks.first()->posRect.bottom();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->posRect.bottom()>lower)
            lower=blocks[i]->posRect.bottom();
        if(blocks[i]->posRect.top()<higher)
            higher=blocks[i]->posRect.top();
    }
    if(posRect.top() >= lower) return false;
    if(posRect.bottom() <= higher) return false;
    return true;
}

bool PGE_Phys_Object::isFloor(QVector<LVL_Block*> &blocks)
{
    if(blocks.isEmpty())
        return false;
    float lefter=blocks.first()->posRect.left();
    float righter=blocks.first()->posRect.right();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->posRect.right()>righter)
            righter=blocks[i]->posRect.right();
        if(blocks[i]->posRect.left()<lefter)
            lefter=blocks[i]->posRect.left();
    }
    if(posRect.left() >= righter) return false;
    if(posRect.right() <= lefter) return false;
    return true;
}

LVL_Block *PGE_Phys_Object::nearestBlock(QVector<LVL_Block*> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    LVL_Block*nearest=NULL;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            if( fabs(blocks[i]->posRect.center().x()-posRect.center().x())<
                fabs(nearest->posRect.center().x()-posRect.center().x()) )
                nearest=blocks[i];
        }
    }
    return nearest;
}

LVL_Block *PGE_Phys_Object::nearestBlockY(QVector<LVL_Block *> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    LVL_Block*nearest=NULL;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            if( fabs(blocks[i]->posRect.center().y()-posRect.center().y())<
                fabs(nearest->posRect.center().y()-posRect.center().y()) )
                nearest=blocks[i];
        }
    }
    return nearest;
}




void PGE_Phys_Object::setParentSection(LVL_Section *sct)
{
    if(_parentSection)
    {
        _parentSection->unregisterElement(this);
    }
    _parentSection=sct;
}

LVL_Section *PGE_Phys_Object::sct()
{
    return _parentSection;
}

GLdouble PGE_Phys_Object::zIndex()
{
    return z_index;
}

void PGE_Phys_Object::update() { _syncPosition(); }

void PGE_Phys_Object::update(float)
{
    _syncPosition();
}

void PGE_Phys_Object::render(double x, double y) {Q_UNUSED(x); Q_UNUSED(y);}

bool PGE_Phys_Object::isPaused()
{
    return _paused;
}

void PGE_Phys_Object::setPaused(bool p)
{
    _paused=p;
}


bool operator<(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index>rhs.z_index; }


bool operator>(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index<rhs.z_index; }


PGE_Phys_Object_Phys::PGE_Phys_Object_Phys()
{
    min_vel_x=0.f;
    min_vel_y=0.f;
    max_vel_x=0.f;
    max_vel_y=0.f;
    grd_dec_x=0.f;
    decelerate_x=0.f;
    decelerate_y=0.f;
    gravityScale=1.0f;
    gravityAccel=26.0f;
}
