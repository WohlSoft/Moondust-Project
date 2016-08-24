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

#ifndef COLLISION_CHECKS_H
#define COLLISION_CHECKS_H

#ifdef OLD_COLLIDERS
#include "lvl_base_object.h"
#include <tgmath.h>
/*
PGE_PointF c1 = posRect.center();
PGE_RectF &r1 = posRect;
PGE_PointF cc = collided->posRect.center();
PGE_RectF  rc = collided->posRect;
*/
inline bool isCollideFloorToponly(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    float summSpeedY =
            ( body.LEGACY_m_velocityY + body.LEGACY_m_velocityY_add )
            -( collided->LEGACY_m_velocityY + collided->LEGACY_m_velocityY_add );
    float summSpeedYprv =
            body.colliding_ySpeed - collided->colliding_ySpeed;

    return  (
             (
                ( summSpeedY >= 0.0 )
                &&
                ( ( body.m_posRect.bottom() - (summSpeedYprv>=0 ? 1 : 0 ) ) < collided->m_posRect.top() + summSpeedYprv )
                &&
                (
                     ( body.m_posRect.left() < collided->m_posRect.right()-1 ) &&
                     ( body.m_posRect.right() > collided->m_posRect.left()+1 )
                 )
             )
            ||
            ( body.m_posRect.bottom() <= collided->m_posRect.top() )
            );
}

inline bool isCollideFloor(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (
     (
        ( ( body.LEGACY_m_velocityY + body.LEGACY_m_velocityY_add >= 0.0 )
          ||( (collided->LEGACY_m_velocityY + collided->LEGACY_m_velocityY_add) <= 0.0 ) )
        &&
        (
            (floor(body.m_posRect.bottom()) + collided->colliding_ySpeed)
                        <
             ( collided->m_posRect.top()
               +fabs(body.LEGACY_m_velocityY
               +body.LEGACY_m_velocityX_add)
               +2.0+body.colliding_ySpeed )
          )
        &&( !( ( body.m_posRect.left() >= (collided->m_posRect.right() - 0.2) )
               ||( body.m_posRect.right() <= (collided->m_posRect.left() + 0.2) ) )
           )
     ) || ( body.m_posRect.bottom() <= collided->m_posRect.top() )
    );
}

inline bool isCollideCelling(PGE_Phys_Object &body,
                             PGE_Phys_Object *collided,
                             float  _heightDelta=0.0f,
                             bool   forceCollideCenter=false)
{
    return ( (
                 (
                     ( (!forceCollideCenter) && ( body.LEGACY_m_velocityY+body.LEGACY_m_velocityY_add<0.0 ) )
                     ||( forceCollideCenter && ( (body.LEGACY_m_velocityY + body.LEGACY_m_velocityY_add) <= 0.0 ) )
                  )
                 &&
                 ( (body.m_posRect.top()+collided->colliding_ySpeed)
                   >
                   ( collided->m_posRect.bottom() + body.colliding_ySpeed - 2.0 + _heightDelta )
                  )
                 &&( !( (body.m_posRect.left() >= collided->m_posRect.right()-0.5 ) ||
                        (body.m_posRect.right() <= collided->m_posRect.left()+0.5 ) ) )
              )
            );
}

inline bool isCollideLeft(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return ( (body.LEGACY_m_velocityX + body.LEGACY_m_velocityX_add) < 0.0)
            &&( body.m_posRect.center().x() > collided->m_posRect.center().x())
            &&( (body.m_posRect.left() + collided->colliding_xSpeed)
                    >=
                 collided->m_posRect.right()+body.colliding_xSpeed-collided->m_width_half
               )
            &&( ( body.m_posRect.top() < collided->m_posRect.bottom() )
              &&( body.m_posRect.bottom() > collided->m_posRect.top() ) );
}

inline bool isCollideRight(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return ( (body.LEGACY_m_velocityX + body.LEGACY_m_velocityX_add) > 0.0)
            &&( body.m_posRect.center().x() < collided->m_posRect.center().x())
            &&( (body.m_posRect.right() + collided->colliding_xSpeed)
                <=
                (collided->m_posRect.left() + body.colliding_xSpeed + collided->m_width_half)
               )
            && ( ( body.m_posRect.top() < collided->m_posRect.bottom() )
               &&( body.m_posRect.bottom() > collided->m_posRect.top() ) );
}

inline double SL_HeightTopRight(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return ( fabs(collided->m_posRect.left() - body.m_posRect.right()) )
             *( collided->m_posRect.height() / collided->m_posRect.width() );
}

inline double SL_HeightTopLeft(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return ( fabs(collided->m_posRect.right() - body.m_posRect.left()) )
                    *( collided->m_posRect.height() / collided->m_posRect.width() );
}

inline bool isCollideSlopeFloor(PGE_Phys_Object &body,
                                PGE_Phys_Object *collided,
                                int direction=-1)
{
    double floorH;
    if(direction<0)
        floorH = SL_HeightTopLeft(body, collided);
    else
        floorH = SL_HeightTopRight(body, collided);
    double slopeTop = collided->m_posRect.bottom()-floorH;
    if( slopeTop < collided->top() ) slopeTop=collided->m_posRect.top();
    else if( slopeTop > collided->bottom() ) slopeTop=collided->m_posRect.bottom();
    return ( body.m_posRect.bottom() >= (slopeTop - 1.0) )
            &&( body.m_posRect.bottom() <= collided->m_posRect.bottom() )
            &&(!( (body.m_posRect.left() + collided->colliding_xSpeed >= collided->m_posRect.right() + body.colliding_xSpeed)
                ||(body.m_posRect.right() + collided->colliding_xSpeed <= collided->m_posRect.left() + body.colliding_xSpeed) ) );
}

inline bool isCollideSlopeCelling(PGE_Phys_Object &body,
                                  PGE_Phys_Object *collided,
                                  int direction=-1)
{
    double cellingH;
    if(direction>0)
        cellingH = SL_HeightTopLeft(body, collided);
    else
        cellingH = SL_HeightTopRight(body, collided);
    double slopeBottom = collided->m_posRect.top() + cellingH;
    if(slopeBottom > collided->bottom()) slopeBottom = collided->m_posRect.bottom();
    else if(slopeBottom < collided->top()) slopeBottom = collided->m_posRect.top();
    return (body.m_posRect.top() <= slopeBottom )
           &&(body.m_posRect.top()>=collided->m_posRect.top())
            &&(!( ( body.m_posRect.left() + collided->colliding_xSpeed >= collided->m_posRect.right() + body.colliding_xSpeed)
                ||( body.m_posRect.right() + collided->colliding_xSpeed <= collided->m_posRect.left() + body.colliding_xSpeed) ) );
}
#endif

#endif // COLLISION_CHECKS_H

