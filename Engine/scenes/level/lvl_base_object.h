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
#include <common_features/pge_texture.h>

struct PGE_Phys_Object_Phys
{
    PGE_Phys_Object_Phys();
    float min_vel_x;
    float min_vel_y;
    float max_vel_x;
    float max_vel_y;
    float decelerate_x;
    float grd_dec_x;
    float decelerate_y;
    float gravityScale;
};

class LVL_Section;
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
    double posCenterX(); //!< Position X
    double posCenterY(); //!< Position Y

    double top();
    double bottom();
    double left();
    double right();

    void setSize(float w, float h);

    virtual void setPos(double x, double y);
    void setPosX(double x);
    void setPosY(double y);
    double speedX();
    double speedY();
    void setSpeed(double x, double y);
    void setSpeedX(double x);
    void setSpeedY(double y);
    void setDecelX(double x);
    void applyAccel(double x, double y);

    double gravityScale();
    void setGravityScale(double scl);

    void _syncPosition();
    void _syncPositionAndSize();
    void _syncSection();
    void renderDebug(float _camX, float _camY);

    void iterateStep(float ticks);
    virtual void updateCollisions();
    virtual void solveCollision(PGE_Phys_Object *collided);

    static const float _smbxTickTime;
    static float SMBXTicksToTime(float ticks);

    PGE_Phys_Object_Phys phys_setup;
    PGE_RectF posRect;
    double _accelX;
    double _accelY;

    double _velocityX;
    double _velocityY;

    double _velocityX_prev;
    double _velocityY_prev;

    double _posX;
    double _posY;

    double width;  //!< Width
    double height; //!< Height

    double _realWidth;  //!< need to sync with tree
    double _realHeight; //!< need to sync with tree

    double posX_coefficient;
    double posY_coefficient;
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

    int collide;

    bool slippery_surface;
    bool isRectangle;

    PGE_Texture texture;
    GLuint texId;
    virtual GLdouble zIndex();
    GLdouble z_index;

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
