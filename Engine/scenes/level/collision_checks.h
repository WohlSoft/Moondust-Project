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

#ifndef COLLISION_CHECKS_H
#define COLLISION_CHECKS_H

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
    float summSpeedY=(body._velocityY+body._velocityY_add)-(collided->_velocityY+collided->_velocityY_add);
    float summSpeedYprv=body.colliding_ySpeed-collided->colliding_ySpeed;
    return  (
             (
                (summSpeedY >= 0.0)
                &&
                ( (body.posRect.bottom()-(summSpeedYprv>=0 ? 1 : 0 )) < collided->posRect.top()+summSpeedYprv)
                &&
                (
                     (body.posRect.left()<collided->posRect.right()-1 ) &&
                     (body.posRect.right()>collided->posRect.left()+1 )
                 )
             )
            ||
            (body.posRect.bottom() <= collided->posRect.top())
            );
}

inline bool isCollideFloor(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (
     (
        (body._velocityY+body._velocityY_add >= 0.0)
        &&
        (floor(body.posRect.bottom())+collided->colliding_ySpeed < collided->posRect.top()+
                                   fabs(body._velocityY+body._velocityX_add)
                                   +2.0+body.colliding_ySpeed)
        &&( !( (body.posRect.left()>=collided->posRect.right()-0.2) || (body.posRect.right() <= collided->posRect.left()+0.2) ) )
     ) || (body.posRect.bottom() <= collided->posRect.top())
    );
}

inline bool isCollideCelling(PGE_Phys_Object &body, PGE_Phys_Object *collided, float _heightDelta=0.0f, bool forceCollideCenter=false)
{
    return ( (
                 (  ((!forceCollideCenter)&&(body._velocityY+body._velocityY_add<0.0))||
                    (forceCollideCenter&&(body._velocityY+body._velocityY_add<=0.0)) )
                 &&
                 (body.posRect.top()+collided->colliding_ySpeed > collided->posRect.bottom()
                  +body.colliding_ySpeed-2.0+_heightDelta)
                 &&( !( (body.posRect.left()>=collided->posRect.right()-0.5 ) ||
                        (body.posRect.right() <= collided->posRect.left()+0.5 ) ) )
              )
             );
}

inline bool isCollideLeft(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (body._velocityX+body._velocityX_add<0.0) && (body.posRect.center().x() > collided->posRect.center().x()) &&
            (body.posRect.left()+collided->colliding_xSpeed >= collided->posRect.right()+body.colliding_xSpeed-collided->_width_half)
                                     && ( (body.posRect.top()<collided->posRect.bottom())&&
                                          (body.posRect.bottom()>collided->posRect.top()) );
}

inline bool isCollideRight(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (body._velocityX+body._velocityX_add>0.0) && (body.posRect.center().x() < collided->posRect.center().x()) &&
            ( body.posRect.right()+collided->colliding_xSpeed <= collided->posRect.left()+body.colliding_xSpeed+collided->_width_half)
                                    && ( (body.posRect.top() < collided->posRect.bottom())&&
                                         (body.posRect.bottom() > collided->posRect.top()) );
}

inline double SL_HeightTopRight(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (fabs(collided->posRect.left()-body.posRect.right()))
            *(collided->posRect.height()/collided->posRect.width());
}

inline double SL_HeightTopLeft(PGE_Phys_Object &body, PGE_Phys_Object *collided)
{
    return (fabs(collided->posRect.right()-body.posRect.left()))
                    *(collided->posRect.height()/collided->posRect.width());
}

inline bool isCollideSlopeFloor(PGE_Phys_Object &body, PGE_Phys_Object *collided, int direction=-1)
{
    double floorH;
    if(direction<0)
        floorH = SL_HeightTopLeft(body, collided);
    else
        floorH = SL_HeightTopRight(body, collided);
    double slopeTop = collided->posRect.bottom()-floorH;
    if(slopeTop<collided->top()) slopeTop=collided->posRect.top();
    else if(slopeTop>collided->bottom()) slopeTop=collided->posRect.bottom();
    return (body.posRect.bottom()>=slopeTop)&&
           (body.posRect.bottom()<=collided->posRect.bottom())&&
   (!( (body.posRect.left()+collided->colliding_xSpeed>=collided->posRect.right()+body.colliding_xSpeed) ||
       (body.posRect.right()+collided->colliding_xSpeed<=collided->posRect.left()+body.colliding_xSpeed) ) )
            ;
}

inline bool isCollideSlopeCelling(PGE_Phys_Object &body, PGE_Phys_Object *collided, int direction=-1)
{
    double cellingH;
    if(direction>0)
        cellingH = SL_HeightTopLeft(body, collided);
    else
        cellingH = SL_HeightTopRight(body, collided);
    double slopeBottom = collided->posRect.top()+cellingH;
    if(slopeBottom >collided->bottom()) slopeBottom =collided->posRect.bottom();
    else if(slopeBottom <collided->top()) slopeBottom = collided->posRect.top();
    return (body.posRect.top()<=slopeBottom )&&
           (body.posRect.top()>=collided->posRect.top())&&
    (!( (body.posRect.left()+collided->colliding_xSpeed>=collided->posRect.right()+body.colliding_xSpeed) ||
        (body.posRect.right()+collided->colliding_xSpeed<=collided->posRect.left()+body.colliding_xSpeed) ) );
}

#endif // COLLISION_CHECKS_H

