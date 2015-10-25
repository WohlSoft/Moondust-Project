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

#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include <common_features/rectf.h>
#include <common_features/pointf.h>
#include <common_features/pge_texture.h>

#include <QVector>
#ifdef __APPLE__
#include <tgmath.h>
#endif

struct PGE_Phys_Object_Phys
{
    float min_vel_x;//!< Min allowed X velocity
    float min_vel_y;//!< Min allowed Y velocity
    float max_vel_x;//!< Max allowed X velocity
    float max_vel_y;//!< Max allowed Y velocity
    float decelerate_x;//!< Deceleration of X velocity in each second
    float grd_dec_x;   //!< Soft deceleration if max X speed limit exited
    float decelerate_y;//!< Deceleration of Y velocity in each second
    float gravityScale;//!< Item specific gravity scaling
    float gravityAccel;//!< Item gravity acceleration
    PGE_Phys_Object_Phys();
};

class LVL_Section;
class LVL_Block;
///
/// \brief The PGE_Phys_Object class
///
/// Is a base of any physical objects
class PGE_Phys_Object
{
public:
    PGE_Phys_Object();
    virtual ~PGE_Phys_Object();
    virtual double posX(); //!< Position X
    virtual double posY(); //!< Position Y
    double posCenterX(); //!< Centered Position X
    double posCenterY(); //!< Centered Position Y
    double width();
    double height();

    double top();
    void setTop(double tp);
    double bottom();
    void setBottom(double btm);
    double left();
    void setLeft(double lf);
    double right();
    void setRight(double rt);

    void setSize(float w, float h);
    void setWidth(float w);
    void setHeight(float h);

    virtual void setPos(double x, double y);
    void setPosX(double x);
    void setPosY(double y);
    void setCenterPos(double x, double y);
    void setCenterX(double x);
    void setCenterY(double y);

    double speedX();
    double speedY();
    double speedXsum();
    double speedYsum();
    void setSpeed(double x, double y);
    void setSpeedX(double x);
    void setSpeedY(double y);
    void setDecelX(double x);
    void applyAccel(double x, double y);

    double gravityScale();
    void setGravityScale(double scl);
    float gravityAccel();
    void setGravityAccel(float acl);

    void _syncPosition();
    void _syncPositionAndSize();
    void _syncSection();
    void renderDebug(float _camX, float _camY);

    void iterateStep(float ticks);
    virtual void updateCollisions();
    virtual void solveCollision(PGE_Phys_Object *collided);
    double colliding_xSpeed;
    double colliding_ySpeed;
    /*
    PGE_PointF c1 = posRect.center();
    PGE_RectF &r1 = posRect;
    PGE_PointF cc = collided->posRect.center();
    PGE_RectF  rc = collided->posRect;
    */
    inline bool isCollideFloorToponly(PGE_Phys_Object *collided)
    {
        float summSpeedY=(_velocityY+_velocityY_add)-(collided->_velocityY+collided->_velocityY_add);
        float summSpeedYprv=colliding_ySpeed-collided->colliding_ySpeed;
        return  (
                 (
                    (summSpeedY >= 0.0)
                    &&
                    ( (posRect.bottom()-(summSpeedYprv>=0 ? 1 : 0 )) < collided->posRect.top()+summSpeedYprv)
                    &&
                    (
                         (posRect.left()<collided->posRect.right()-1 ) &&
                         (posRect.right()>collided->posRect.left()+1 )
                     )
                 )
                ||
                (posRect.bottom() <= collided->posRect.top())
                );
    }

    inline bool isCollideFloor(PGE_Phys_Object *collided)
    {
        return (
         (
            (_velocityY+_velocityY_add >= 0.0)
            &&
            (floor(posRect.bottom())+collided->colliding_ySpeed < collided->posRect.top()+
                                       fabs(_velocityY+_velocityX_add)
                                       +2.0+colliding_ySpeed)
            &&( !( (posRect.left()>=collided->posRect.right()-0.2) || (posRect.right() <= collided->posRect.left()+0.2) ) )
         ) || (posRect.bottom() <= collided->posRect.top())
        );
    }

    inline bool isCollideCelling(PGE_Phys_Object *collided, float _heightDelta=0.0f, bool forceCollideCenter=false)
    {
        return ( (
                     (  ((!forceCollideCenter)&&(_velocityY+_velocityY_add<0.0))||
                        (forceCollideCenter&&(_velocityY+_velocityY_add<=0.0)) )
                     &&
                     (posRect.top()+collided->colliding_ySpeed > collided->posRect.bottom()
                      +colliding_ySpeed-2.0+_heightDelta)
                     &&( !( (posRect.left()>=collided->posRect.right()-0.5 ) ||
                            (posRect.right() <= collided->posRect.left()+0.5 ) ) )
                  )
                 );
    }

    inline bool isCollideLeft(PGE_Phys_Object *collided)
    {
        return (_velocityX+_velocityX_add<0.0) && (posRect.center().x() > collided->posRect.center().x()) &&
                (posRect.left()+collided->colliding_xSpeed >= collided->posRect.right()+colliding_xSpeed-collided->_width_half)
                                         && ( (posRect.top()<collided->posRect.bottom())&&
                                              (posRect.bottom()>collided->posRect.top()) );
    }

    inline bool isCollideRight(PGE_Phys_Object *collided)
    {
        return (_velocityX+_velocityX_add>0.0) && (posRect.center().x() < collided->posRect.center().x()) &&
                ( posRect.right()+collided->colliding_xSpeed <= collided->posRect.left()+colliding_xSpeed+collided->_width_half)
                                        && ( (posRect.top() < collided->posRect.bottom())&&
                                             (posRect.bottom() > collided->posRect.top()) );
    }

    enum Slopes{
        SLOPE_LEFT=-1,
        SLOPE_RIGHT=1
    };

    inline double SL_HeightTopRight(PGE_Phys_Object *collided)
    {
        return (fabs(collided->posRect.left()-posRect.right()))
                *(collided->posRect.height()/collided->posRect.width());
    }

    inline double SL_HeightTopLeft(PGE_Phys_Object *collided)
    {
        return (fabs(collided->posRect.right()-posRect.left()))
                        *(collided->posRect.height()/collided->posRect.width());
    }

    inline bool isCollideSlopeFloor(PGE_Phys_Object *collided, int direction=-1)
    {
        double floorH;
        if(direction<0)
            floorH = SL_HeightTopLeft(collided);
        else
            floorH = SL_HeightTopRight(collided);
        double slopeTop = collided->posRect.bottom()-floorH;
        if(slopeTop<collided->top()) slopeTop=collided->posRect.top();
        else if(slopeTop>collided->bottom()) slopeTop=collided->posRect.bottom();
        return (posRect.bottom()>=slopeTop)&&
               (posRect.bottom()<=collided->posRect.bottom())&&
       (!( (posRect.left()+collided->colliding_xSpeed>=collided->posRect.right()+colliding_xSpeed) ||
           (posRect.right()+collided->colliding_xSpeed<=collided->posRect.left()+colliding_xSpeed) ) )
                ;
    }

    inline bool isCollideSlopeCelling(PGE_Phys_Object *collided, int direction=-1)
    {
        double cellingH;
        if(direction>0)
            cellingH = SL_HeightTopLeft(collided);
        else
            cellingH = SL_HeightTopRight(collided);
        double slopeBottom = collided->posRect.top()+cellingH;
        if(slopeBottom >collided->bottom()) slopeBottom =collided->posRect.bottom();
        else if(slopeBottom <collided->top()) slopeBottom = collided->posRect.top();
        return (posRect.top()<=slopeBottom )&&
               (posRect.top()>=collided->posRect.top())&&
        (!( (posRect.left()+collided->colliding_xSpeed>=collided->posRect.right()+colliding_xSpeed) ||
            (posRect.right()+collided->colliding_xSpeed<=collided->posRect.left()+colliding_xSpeed) ) );
    }

    PGE_Phys_Object *nearestBlock(QVector<PGE_Phys_Object *> &blocks);
    PGE_Phys_Object *nearestBlockY(QVector<PGE_Phys_Object *> &blocks);
    bool isWall(QVector<PGE_Phys_Object *> &blocks);
    bool isFloor(QVector<PGE_Phys_Object *> &blocks, bool *isCliff=0);

    static const float _smbxTickTime;
    static float SMBXTicksToTime(float ticks);

    PGE_Phys_Object_Phys phys_setup;//!< Settings of physics
    PGE_RectF posRect;//!< Real body geometry and position
    double _accelX; //!<Delta of X velocity in a second
    double _accelY; //!<Delta of Y velocity in a second

    double _velocityX;//!< current X speed (pixels per 1/65 of second)
    double _velocityY;//!< current Y speed (pixels per 1/65 of second)

    double _velocityX_prev;//!< X speed before last itertion step (pixels per 1/65 of second)
    double _velocityY_prev;//!< Y speed before last itertion step (pixels per 1/65 of second)

    double _velocityX_add; //!< additional X acceleration
    double _velocityY_add; //!< additional Y acceleration

    double _posX; //!< Synchronized with R-Tree position
    double _posY; //!< Synchronized with R-Tree position

    double _width;  //!< Synchronized with R-Tree Width
    double _height; //!< Synchronized with R-Tree Height
    double _width_half;//!< Half of width
    double _height_half;//!< Half of height

    double _realWidth;  //!< Width prepared to synchronize with R-Tree
    double _realHeight; //!< Height prepared to synchronize with R-Tree

    void setParentSection(LVL_Section* sct);
    LVL_Section* sct();
    LVL_Section *_parentSection;

    int type;

    enum CollisionType{
        COLLISION_NONE = 0,
        COLLISION_ANY = 1,
        COLLISION_TOP = 2,
        COLLISION_BOTTOM = 3
    };

    int collide_player;
    int collide_npc;

    bool slippery_surface;
    bool isRectangle;

    PGE_Texture texture;
    GLuint texId;
    virtual GLdouble zIndex();
    GLdouble z_index;

    /**************************Layer member****************************/
    virtual void show();
    virtual void hide();
    virtual bool isVisible();
    bool _is_visible;
    /******************************************************************/

public:

    enum types
    {
        LVLUnknown=0,
        LVLBlock,
        LVLBGO,
        LVLNPC,
        LVLPlayer,
        LVLEffect,
        LVLWarp,
        LVLSpecial,
        LVLPhysEnv
    };

    virtual void update();
    virtual void update(float ticks);
    virtual void render(double x, double y);

    bool isPaused();
    void setPaused(bool p);
private:
    bool _paused;

};



bool operator< (const PGE_Phys_Object& lhs, const PGE_Phys_Object& rhs);
bool operator> (const PGE_Phys_Object& lhs, const PGE_Phys_Object& rhs);

#endif // BASE_OBJECT_H
