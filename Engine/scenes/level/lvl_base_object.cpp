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

#include "collision_checks.h"

const float PGE_Phys_Object::m_smbxTickTime = 15.285f;
//1000.f/65.f; Thanks to Rednaxela for hint, 15.6 is a true frame time in SMBX Engine!
//BUT, Experimentally was found that in real is approximately is 15.285

float PGE_Phys_Object::SMBXTicksToTime(float ticks)
{
    return ticks*m_smbxTickTime;
}

PGE_Phys_Object::PGE_Phys_Object(LevelScene *_parent) :
    _vizible_on_screen(false),
    _render_list(false),
    _scene(_parent),
    _is_registered(false),
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
    m_width_half = 0.0f;
    m_height_half = 0.0f;

    z_index = 0.0L;
    m_isRectangle = true;

    m_slippery_surface = false;
    collide_player = COLLISION_ANY;
    collide_npc = COLLISION_ANY;

    collided_slope=false;
    collided_slope_angle_ratio=0.0f;
    collided_slope_celling=false;
    collided_slope_angle_ratio_celling=0.0f;

    _parentSection=NULL;
    m_width_registered = 0.0;
    m_height_registered = 0.0;
    m_width_toRegister=0.0;
    m_height_toRegister=0.0;
    m_posX_registered=0.0;
    m_posY_registered=0.0;
    m_velocityX=0.0;
    m_velocityY=0.0;
    m_velocityX_prev=0.0;
    m_velocityY_prev=0.0;
    m_velocityX_add=0.0;
    m_velocityY_add=0.0;

    colliding_xSpeed=0.0;
    colliding_ySpeed=0.0;

    m_paused=false;

    m_is_visible=true;

    m_accelX=0;
    m_accelY=0;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if(_is_registered) _scene->unregisterElement(this);
}

void PGE_Phys_Object::registerInTree()
{
    if(!_is_registered) _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::unregisterFromTree()
{
    if(_is_registered) _scene->unregisterElement(this);
    _is_registered=false;
}

double PGE_Phys_Object::posX()
{
    return m_posRect.x();
}

double PGE_Phys_Object::posY()
{
    return m_posRect.y();
}

double PGE_Phys_Object::posCenterX()
{
    return m_posRect.center().x();
}

double PGE_Phys_Object::posCenterY()
{
    return m_posRect.center().y();
}

void PGE_Phys_Object::setCenterX(double x)
{
    setPosX(x-m_width_half);
}

void PGE_Phys_Object::setCenterY(double y)
{
    setPosY(y-m_height_half);
}

double PGE_Phys_Object::width()
{
    return m_posRect.width();
}

double PGE_Phys_Object::height()
{
    return m_posRect.height();
}

double PGE_Phys_Object::top()
{
    return posY();
}

void PGE_Phys_Object::setTop(double tp)
{
    m_posRect.setTop(tp);
    m_height_toRegister=m_posRect.height();
    m_height_half = m_height_toRegister/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::bottom()
{
    return posY()+m_height_registered;
}

void PGE_Phys_Object::setBottom(double btm)
{
    m_posRect.setBottom(btm);
    m_height_toRegister=m_posRect.height();
    m_height_half = m_height_toRegister/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::left()
{
    return posX();
}

void PGE_Phys_Object::setLeft(double lf)
{
    m_posRect.setLeft(lf);
    m_width_toRegister=m_posRect.width();
    m_height_half = m_width_toRegister/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::right()
{
    return posX()+m_width_registered;
}

void PGE_Phys_Object::setRight(double rt)
{
    m_posRect.setRight(rt);
    m_width_toRegister=m_posRect.width();
    m_height_half = m_width_toRegister/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setSize(float w, float h)
{
    m_posRect.setSize(w, h);
    m_width_toRegister=w;
    m_height_toRegister=h;
    m_width_half = m_width_toRegister/2.0f;
    m_height_half = m_height_toRegister/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setWidth(float w)
{
    m_posRect.setWidth(w);
    m_width_toRegister=w;
    m_width_half = m_width_toRegister/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setHeight(float h)
{
    m_posRect.setHeight(h);
    m_height_toRegister=h;
    m_height_half = m_height_toRegister/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setPos(double x, double y)
{
    m_posRect.setPos(x, y);
    _syncPosition();
}

void PGE_Phys_Object::setPosX(double x)
{
    m_posRect.setX(x);
    _syncPosition();
}

void PGE_Phys_Object::setPosY(double y)
{
    m_posRect.setY(y);
    _syncPosition();
}

void PGE_Phys_Object::setCenterPos(double x, double y)
{
    setPos(x-m_width_half, y-m_height_half);
}

double PGE_Phys_Object::speedX()
{
    return m_velocityX;
}

double PGE_Phys_Object::speedY()
{
    return m_velocityY;
}

double PGE_Phys_Object::speedXsum()
{
    return m_velocityX+m_velocityX_add;
}

double PGE_Phys_Object::speedYsum()
{
    return m_velocityY+m_velocityY_add;
}

void PGE_Phys_Object::setSpeed(double x, double y)
{
    m_velocityX=x;
    m_velocityY=y;
    updateSpeedAddingStack();
    m_velocityX_prev=m_velocityX;
    m_velocityY_prev=m_velocityY;
}

void PGE_Phys_Object::setSpeedX(double x)
{
    m_velocityX=x;
    updateSpeedAddingStack();
    m_velocityX_prev=m_velocityX;
}

void PGE_Phys_Object::setSpeedY(double y)
{
    m_velocityY=y;
    updateSpeedAddingStack();
    m_velocityY_prev=m_velocityY;
}

void PGE_Phys_Object::setDecelX(double x)
{
    phys_setup.decelerate_x = float(x);
}

void PGE_Phys_Object::applyAccel(double x, double y)
{
    m_accelX=x;
    m_accelY=y;
}

void PGE_Phys_Object::iterateSpeedAddingStack(double ticks)
{
    for(int i=0; i<m_speedAddingTopElements.size(); i++)
    {
        PGE_Phys_Object* &topEl = m_speedAddingTopElements[i];
        PGE_RectF &posR = topEl->m_posRect;
        QList<PGE_Phys_Object*> &be = topEl->m_speedAddingBottomElements;
        int beI=0;
        double middleX = 0.0;
        double middleX_num = 0.0;
        double middleY = 0.0;
        //topEl->_velocityX_add = 0.0;
        //topEl->_velocityY_add = 0.0;
        for(int j=0; j<be.size(); j++)
        {
            //Find middle value
            middleX     += be[j]->speedXsum();
            middleX_num += 1.0;
            //Don't fall through floor!!!
            if( j == 0 )
                middleY = be[j]->speedYsum();
            else if(middleY > be[j]->speedYsum())
                middleY = be[j]->speedYsum();
            if(be[j] == this)
            {
                beI = j;
            }
        }
        if(middleX_num != 0.0)  middleX /= middleX_num;
        topEl->m_velocityX_add = middleX;
        topEl->m_velocityY_add = middleY;
        if(middleX_num != 0.0)  middleX /= middleX_num;
        double iterateX = (middleX) * (ticks/m_smbxTickTime);
        double iterateY = (middleY) * (ticks/m_smbxTickTime);
        topEl->setPos(posR.x()+iterateX, posR.y()+iterateY);
        //topEl->iterateSpeedAddingStack(ticks);
        if( !m_posRect.collideRectDeep( posR, -2.0 )
        /* || (posRect.top()+speedYsum() < posR.bottom()-topEl->speedYsum() )*/
           ||  (m_posRect.top() < posR.bottom()-topEl->colliding_ySpeed) )
        {
            topEl->m_velocityX_add = 0.0;
            topEl->m_velocityY_add = 0.0;
            be.removeAt(beI);
            m_speedAddingTopElements.removeAt(i); i--;
            //continue;
        }
    }
}

void PGE_Phys_Object::removeSpeedAddingPointers()
{
    for(int i=0; i<m_speedAddingBottomElements.size(); i++)
    {
        QList<PGE_Phys_Object*>  &te = m_speedAddingBottomElements[i]->m_speedAddingTopElements;
        for(int j=0; j<te.size(); j++)
        {
            if(te[j]==this)
            {
                te.removeAt(j);
                j--;
            }
        }
    }
    for(int i=0; i<m_speedAddingTopElements.size(); i++)
    {
        QList<PGE_Phys_Object*>  &be = m_speedAddingTopElements[i]->m_speedAddingBottomElements;
        for(int j=0; j<be.size(); j++)
        {
            if(be[j]==this)
            {
                be.removeAt(j);
                j--;
            }
        }
    }
}

void PGE_Phys_Object::_syncPosition()
{
    if(_is_registered) _scene->unregisterElement(this);
    m_posX_registered= m_posRect.x();
    m_posY_registered= m_posRect.y();
    _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::_syncPositionAndSize()
{
    if(_is_registered) _scene->unregisterElement(this);
    m_posX_registered= m_posRect.x();
    m_posY_registered= m_posRect.y();
    m_width_registered=m_width_toRegister;
    m_height_registered=m_height_toRegister;
    _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::_syncSection(bool sync_position)
{
    int sID = _scene->findNearestSection(long(posX()), long(posY()));
    LVL_Section *sct = _scene->getSection(sID);
    if(sct)
    {
        setParentSection(sct);
    }
    if(sync_position) _syncPosition();
}

void PGE_Phys_Object::renderDebug(float _camX, float _camY)
{
    switch(type)
    {
        case LVLUnknown:    GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false); break;
        case LVLBlock:      GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, false); break;
        case LVLBGO:        GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, false); break;
        case LVLNPC:        GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, false); break;
        case LVLPlayer:     GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 0.5f, 0.5f, 1.0f, false); break;
        case LVLEffect:     GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 0.5f, 0.5f, 0.5f, 1.0f, false); break;
        case LVLWarp:       GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 0.0f, 0.0f, 0.5f, true);  break;
        case LVLSpecial:    GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, true);  break;
        case LVLPhysEnv:    GlRenderer::renderRect(float(m_posRect.x())-_camX, float(m_posRect.y())-_camY, float(m_posRect.width())-1.0f, float(m_posRect.height())-1.0f, 1.0f, 1.0f, 0.0f, 0.5f, true);  break;
    }
}


void PGE_Phys_Object::iterateStep(float ticks)
{
    if(m_paused) return;

    double iterateX = (m_velocityX) * (ticks/m_smbxTickTime);
    double iterateY = (m_velocityY) * (ticks/m_smbxTickTime);
    if(collided_slope)
    {
        iterateY = (m_velocityY + (m_velocityX*collided_slope_angle_ratio))* (ticks/m_smbxTickTime);
        //posRect.setY(posRect.y() + _velocityY_prev * (ticks/_smbxTickTime));
    }

    m_posRect.setX(m_posRect.x() + iterateX);
    m_velocityX_prev = m_velocityX;
    m_posRect.setY(m_posRect.y() + iterateY);
    m_velocityY_prev = m_velocityY;

    iterateSpeedAddingStack(ticks);
//    if(collided_slope)
//    {
//        _velocityY_prev = (_velocityY+_velocityY_add+(_velocityX*collided_slope_angle_ratio));
//        posRect.setY(posRect.y() + _velocityY_prev * (ticks/_smbxTickTime));
//    }
//    else
//    {
//        posRect.setY(posRect.y() + iterateY);
//        _velocityY_prev = _velocityY;
//    }

    colliding_xSpeed = Maths::max(fabs(m_velocityX+m_velocityX_add), fabs(m_velocityX_prev+m_velocityX_add))
            * Maths::sgn(speedX()+m_velocityX_add)*(ticks/m_smbxTickTime);
    colliding_ySpeed = Maths::max(fabs(m_velocityY+m_velocityY_add), fabs(m_velocityY_prev+m_velocityY_add))
            * Maths::sgn(speedY()+m_velocityY_add)*(ticks/m_smbxTickTime);
}

void PGE_Phys_Object::iterateStepPostCollide(float ticks)
{
    bool updateSpeedAdding=true;

    float G = phys_setup.gravityScale * _scene->globalGravity;
    float accelCof=ticks/1000.0f;

    if(m_accelX != 0.0f)
    {
        m_velocityX+= m_accelX*accelCof;
        updateSpeedAdding=true;
        m_accelX=0;
    }
    else
    if(phys_setup.decelerate_x != 0.0f)
    {
        float decX=phys_setup.decelerate_x*accelCof;
        if(m_velocityX>0)
        {
            if(m_velocityX-decX>0.0)
                m_velocityX-=decX;
            else
                m_velocityX=0;
        } else if(m_velocityX<0) {
            if(m_velocityX+decX<0.0)
                m_velocityX+=decX;
            else
                m_velocityX=0;
        }
        updateSpeedAdding=true;
    }

    if( ((m_accelY != 0.0f) && !onGround())||
        ((m_accelY <  0.0f) &&  onGround()) )
    {
        m_velocityY+= m_accelY*accelCof*( (G==0.0f)?1.0f:G );
        updateSpeedAdding=true;
        m_accelY = 0.0f;
    }

    if(phys_setup.decelerate_y != 0.0f)
    {
        float decY=phys_setup.decelerate_y*accelCof;
        if(m_velocityY>0)
        {
            if(m_velocityY-decY>0.0)
                m_velocityY-=decY;
            else
                m_velocityY=0;
        } else if(m_velocityY<0) {
            if(m_velocityY+decY<0.0)
                m_velocityY+=decY;
            else
                m_velocityY=0;
        }
        updateSpeedAdding=true;
    }

    if( ((phys_setup.gravityAccel != 0.0f) && !onGround()) ||
        ((phys_setup.gravityAccel < 0.0f)  &&  onGround()) )
    {
        m_velocityY+= (G*phys_setup.gravityAccel)*accelCof;
        updateSpeedAdding=true;
    }

    if((phys_setup.max_vel_x != 0.0f)&&(m_velocityX>phys_setup.max_vel_x))
        { m_velocityX-=phys_setup.grd_dec_x*accelCof; updateSpeedAdding=true;}
    if((phys_setup.min_vel_x != 0.0f)&&(m_velocityX<phys_setup.min_vel_x))
        { m_velocityX+=phys_setup.grd_dec_x*accelCof; updateSpeedAdding=true;}
    if((phys_setup.max_vel_y != 0.0f)&&(m_velocityY>phys_setup.max_vel_y))
        {m_velocityY=phys_setup.max_vel_y; updateSpeedAdding=true;}
    if((phys_setup.min_vel_y != 0.0f)&&(m_velocityY<phys_setup.min_vel_y))
        {m_velocityY=phys_setup.min_vel_y; updateSpeedAdding=true;}

    if(updateSpeedAdding)
        updateSpeedAddingStack();
}

void PGE_Phys_Object::updateCollisions()
{
    if(m_paused) return;

    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = m_posRect.withMargin(2.0);
    _scene->queryItems(posRectC, &bodies);

    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*body=*it;
        if(body==this) continue;
        if(body->m_paused) continue;
        if(!body->m_is_visible) continue;

        detectCollisions(body);
    }
}

void PGE_Phys_Object::detectCollisions(PGE_Phys_Object *)
{}



bool PGE_Phys_Object::isWall(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.isEmpty())
        return false;
    double higher = blocks.first()->m_posRect.top();
    double lower  = blocks.first()->m_posRect.bottom();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->m_posRect.bottom()>lower)
            lower = blocks[i]->m_posRect.bottom();
        if(blocks[i]->m_posRect.top()<higher)
            higher = blocks[i]->m_posRect.top();
    }
    if(m_posRect.top() >= lower) return false;
    if(m_posRect.bottom() <= higher) return false;
    return true;
}

bool PGE_Phys_Object::isFloor(QVector<PGE_Phys_Object*> &blocks, bool *isCliff)
{
    if(isCliff)
        *isCliff=false;

    if(blocks.isEmpty())
        return false;
    double lefter  = blocks.first()->m_posRect.left();
    double righter = blocks.first()->m_posRect.right();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->m_posRect.right()>righter)
            righter=blocks[i]->m_posRect.right();
        if(blocks[i]->m_posRect.left()<lefter)
            lefter=blocks[i]->m_posRect.left();
    }

    if(m_posRect.left() >= righter) return false;
    if(m_posRect.right() <= lefter) return false;

    if(isCliff)
    {
        if((speedX() < 0.0) && ( lefter > m_posRect.center().x()) )
            *isCliff=true;
        else
        if((speedX() > 0.0) && (righter < m_posRect.center().x()) )
            *isCliff=true;
    }
    return true;
}

PGE_Phys_Object *PGE_Phys_Object::nearestBlock(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    PGE_Phys_Object*nearest=NULL;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            if( fabs(blocks[i]->m_posRect.center().x()-m_posRect.center().x())<
                fabs(nearest->m_posRect.center().x()-m_posRect.center().x()) )
                nearest=blocks[i];
        }
    }
    return nearest;
}

PGE_Phys_Object *PGE_Phys_Object::nearestBlockY(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    PGE_Phys_Object*nearest=NULL;
    double nearest_blockY=0.0;
    double blockY=0.0;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            //Check for a possible slope
            if(blocks[i]->type==PGE_Phys_Object::LVLBlock)
            {
                LVL_Block* b = static_cast<LVL_Block*>(blocks[i]);
                switch(b->shape)
                {
                    case LVL_Block::shape_tr_left_top:
                        blockY=nearest->m_posRect.top()+SL_HeightTopRight(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_right_top:
                        blockY=nearest->m_posRect.top()+SL_HeightTopLeft(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_right_bottom:
                        blockY=nearest->m_posRect.bottom()-SL_HeightTopRight(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_left_bottom:
                        blockY=nearest->m_posRect.bottom()-SL_HeightTopLeft(*this, nearest);
                        break;

                    default:break;
                        blockY=blocks[i]->m_posRect.center().y();
                }
            } else blockY=blocks[i]->m_posRect.center().y();
            if( fabs(blockY-m_posRect.center().y())<
                fabs(nearest_blockY-m_posRect.center().y()) )
            {
                nearest=blocks[i];
                nearest_blockY=blockY;
            }
        }
    }
    return nearest;
}




void PGE_Phys_Object::setParentSection(LVL_Section *sct)
{
    _parentSection=sct;
}

LVL_Section *PGE_Phys_Object::sct()
{
    return _parentSection;
}

long double PGE_Phys_Object::zIndex() { return z_index; }

void PGE_Phys_Object::update() { _syncPosition(); }
void PGE_Phys_Object::update(float) { _syncPosition(); }






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

void PGE_Phys_Object::show()
{
    m_is_visible=true;
}

void PGE_Phys_Object::hide()
{
    m_is_visible=false;
}

bool PGE_Phys_Object::isVisible()
{
    return m_is_visible;
}


