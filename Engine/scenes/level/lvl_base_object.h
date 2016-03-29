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
class LevelScene;
class LVL_Block;
class PGE_LevelCamera;
///
/// \brief The PGE_Phys_Object class
///
/// Is a base of any physical objects
class PGE_Phys_Object
{
    friend class PGE_LevelCamera;
    friend class LevelScene;
    //! Tells, does this object was catched by camera since recent render action
    bool         _vizible_on_screen;
    //! Tells, does this object stored into the render list
    bool         _render_list;
public:
    inline bool isInRenderList() {return _render_list;}
public:
    //! Pointer of the parent scene
    LevelScene*  _scene;
protected:
    //! Is this object registered in the R-Tree?
    bool             _is_registered;
public:
    PGE_Phys_Object(LevelScene *_parent=NULL);
    virtual ~PGE_Phys_Object();
    void registerInTree();
    void unregisterFromTree();
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
    /*!
     * \brief Update Speed-adding stack if needed
     */
    virtual void updateSpeedAddingStack() {}
    /*!
     * \brief Apply position correction to members of speed-adding stack (to don't cause through-fall when rider will touch floor)
     * param offsetX Apply offset on X
     * param offsetY Apply offset on Y
     */
    virtual void applyCorrectionToSA_stack(double, double) {}

    double gravityScale();
    void setGravityScale(double scl);
    float gravityAccel();
    void setGravityAccel(float acl);

    void _syncPosition();
    void _syncPositionAndSize();
    void _syncSection(bool sync_position=true);
    void renderDebug(float _camX, float _camY);

    void iterateStep(float ticks);
    virtual void updateCollisions();
    virtual void detectCollisions(PGE_Phys_Object *collided);
    double colliding_xSpeed;
    double colliding_ySpeed;

    bool   collided_slope;
    float  collided_slope_angle_ratio;
    bool   collided_slope_celling;
    float  collided_slope_angle_ratio_celling;

    enum Slopes{
        SLOPE_LEFT=-1,
        SLOPE_RIGHT=1
    };

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
    virtual long double zIndex();
    long double z_index;

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
