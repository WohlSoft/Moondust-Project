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
    double min_vel_x;//!< Min allowed X velocity
    double min_vel_y;//!< Min allowed Y velocity
    double max_vel_x;//!< Max allowed X velocity
    double max_vel_y;//!< Max allowed Y velocity
    double decelerate_x;//!< Deceleration of X velocity in each second
    double grd_dec_x;   //!< Soft deceleration if max X speed limit exited
    double decelerate_y;//!< Deceleration of Y velocity in each second
    double gravityScale;//!< Item specific gravity scaling
    double gravityAccel;//!< Item gravity acceleration
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

    /***********************Physical engine locals*****************************/
    enum ContactAt{
        Contact_None = 0,
        Contact_Top,
        Contact_Bottom,
        Contact_Left,
        Contact_Right,
        Contact_Skipped
    };

    enum ShapeType {
        SL_Rect = 0,
        SL_RightBottom,
        SL_LeftBottom,
        SL_LeftTop,
        SL_RightTop
    };

    enum BlockedSides{
        Block_NONE      = 0,
        Block_LEFT      = 0x1,
        Block_TOP       = 0x2,
        Block_RIGHT     = 0x4,
        Block_BOTTOM    = 0x8,
        Block_ALL       = 0xF,
    };

    struct objRect
    {
        double x;
        double y;
        double w;
        double h;
        inline double left() {return x;}
        inline double right() {return x+w;}
        inline double top() {return y;}
        inline double bottom() {return y+h;}
    };

    struct Momentum {
        Momentum() :
            x(0.0),     y(0.0),     w(32.0),    h(32.0),
            oldx(0.0),  oldy(0.0),  oldw(32.0), oldh(32.0),
            velX(0.0),  velY(0.0),  velXsrc(0.0)
        {}
        Momentum(double _x, double _y) :
            x(_x),     y(_y),     w(32.0),    h(32.0),
            oldx(_x),  oldy(_y),  oldw(32.0), oldh(32.0),
            velX(0.0),  velY(0.0),  velXsrc(0.0)
        {}

        //! Position X
        double x;
        //! Position Y
        double y;
        //! Width
        double w;
        //! Height
        double h;
        //! Previous position X
        double oldx;
        //! Previous position Y
        double oldy;
        //! Previous width
        double oldw;
        //! Previous height
        double oldh;
        //! Real (sum of the own and floor speeds) horizontal speed (velocity)
        double velX;
        //! Real Vertical speed (velocity)
        double velY;
        //! Source (own) horizontal speed (velocity).
        double velXsrc;

        inline objRect   rect() {return {x, y, w, h}; }
        inline objRect   rectOld() {return {oldx, oldy, oldw, oldh}; }
        inline PGE_RectF rectF() {return PGE_RectF(x, y, w, h); }
        inline PGE_RectF rectOldF() {return PGE_RectF(oldx, oldy, oldw, oldh); }

        /**
         * @brief Copy all current values to the "old"
         */
        inline void saveOld() { oldx = x; oldy=y; oldw=w; oldh=h; }

        inline void setXatLeft(double newx) { x = newx; }
        inline void setXatRight(double newx) { x = newx-w; }
        inline void setYatTop(double newy) { y = newy; }
        inline void setYatBottom(double newy) { y = newy-h; }
        inline void setCenterX(double newx) { x = newx - w/2.0; }
        inline void setCenterY(double newy) { y = newy - h/2.0; }
        inline void setCenterPos(double X, double Y) { x=X-(w/2.0); y=Y-(h/2.0); }
        inline void setLeft(double newx) { w = fabs(newx-(x+w)); x = newx; }
        inline void setRight(double newx) { w = fabs(x-newx); }
        inline void setTop(double newy) { h = fabs(newy-(y+h)); y = newy; }
        inline void setBottom(double newy) { h = fabs(y-newy); }

        inline double left(){return x;}
        inline double top(){return y;}
        inline double right(){return x+w;}
        inline double bottom(){return y+h;}
        inline double centerX(){return x+(w/2.0);}
        inline double centerY(){return y+(h/2.0);}

        inline double leftOld(){return oldx;}
        inline double topOld(){return oldy;}
        inline double rightOld(){return oldx+oldw;}
        inline double bottomOld(){return oldy+oldh;}
        inline double centerXold(){return oldx+(oldw/2.0);}
        inline double centerYold(){return oldy+(oldh/2.0);}

        inline  bool   betweenH(double left, double right) { if(right < x) return false; if(left > x+w) return false; return true; }
        inline  bool   betweenH(double X) { return (X >= x) && (X <= x+w); }
        inline  bool   betweenV(double top, double bottom) { if(bottom < y) return false; if(top > y+h) return false; return true; }
        inline  bool   betweenV(double Y) { return (Y >= y) && (Y <= y+h); }
    };

    struct SlopeState {
        SlopeState():
            has(false),
            hasOld(false),
            shape(PGE_Phys_Object::SL_Rect)
        {}
        bool    has;
        bool    hasOld;
        objRect rect;
        int     shape;
    };

    QVector<PGE_Phys_Object*> l_contactL;
    QVector<PGE_Phys_Object*> l_contactR;
    QVector<PGE_Phys_Object*> l_contactT;
    QVector<PGE_Phys_Object*> l_contactB;

    int         m_shape;
    Momentum    m_momentum;
    /***************Events****************/
    inline void    resetEvents()
    {
        m_cliff      = false;
        m_touchLeftWall  = false;
        m_touchRightWall = false;
        m_blockedAtLeft = false;
        m_blockedAtRight = false;
        m_crushedOld = m_crushed;
        m_crushed    = false;
        m_stand      = false;
        m_onSlippery = false;
        m_standOnYMovable = false;
        #ifdef IS_MINIPHYSICS_DEMO_PROGRAM
        if(m_crushedHardDelay > 0)
            m_crushedHardDelay -= 1;
        else
        #endif
            m_crushedHard = false;

        l_contactL.clear();
        l_contactR.clear();
        l_contactT.clear();
        l_contactB.clear();

        m_slopeFloor.hasOld = m_slopeFloor.has;
        m_slopeFloor.has = false;
        m_slopeCeiling.hasOld = m_slopeCeiling.has;
        m_slopeCeiling.has = false;
        if(!m_slopeFloor.hasOld)
            m_slopeFloor.shape = -1;
        if(!m_slopeCeiling.hasOld)
            m_slopeCeiling.shape = -1;

    }
    bool    m_touchLeftWall;
    bool    m_touchRightWall;
    bool    m_blockedAtLeft;
    bool    m_blockedAtRight;
    bool    m_stand;
    bool    m_standOnYMovable;
    bool    m_crushed;
    bool    m_crushedOld;
    bool    m_crushedHard;
    int     m_crushedHardDelay;
    bool    m_cliff;
    bool    m_onSlippery;
    /*************************************/
    bool    m_moveLeft;
    bool    m_moveRight;
    SlopeState m_slopeFloor;
    SlopeState m_slopeCeiling;
    //! Y-speed add while standing on the slope
    double  m_onSlopeYAdd;

    //! Allow running over floor holes
    bool    m_allowHoleRuning;
    //! Enable automatical aligning of position while staying on top corner of slope
    bool    m_onSlopeFloorTopAlign;
    //! Blocking filters (0 - playable characters, 1 - NPCs)
    int     m_blocked[3];
    //! Type of self (0 - playable characters, 1 - NPCs)
    int     m_filterID;

    /***********************Physical engine locals***END***********************/

    PGE_Phys_Object(LevelScene *_parent=NULL);
    virtual ~PGE_Phys_Object();
    void registerInTree();
    void unregisterFromTree();

    /*!
     * Position X at left-top corner
     * \return Position X at left-top corner
     */
    virtual double posX();
    /*!
     * \brief Position Y at left-top corner
     * \return Position Y at left-top corner
     */
    virtual double posY();
    /*!
     * \brief Position X at center
     * \return Position X at center
     */
    double posCenterX();
    /*!
     * \brief Position Y at center
     * \return Position Y at center
     */
    double posCenterY();
    /*!
     * \brief width of body
     * \return width of body
     */
    double width();
    /*!
     * \brief height of body
     * \return height of body
     */
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

    //! Elements staying on top (or bumped from side)
    QList<PGE_Phys_Object*> m_speedAddingTopElements;
    QList<PGE_Phys_Object*> m_speedAddingBottomElements;
    virtual void iterateSpeedAddingStack(double ticks);
    virtual void removeSpeedAddingPointers();
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

    inline double  gravityScale() { return phys_setup.gravityScale; }
    inline void    setGravityScale(double scl) { phys_setup.gravityScale = scl; }
    inline double  gravityAccel() { return phys_setup.gravityAccel; }
    inline void    setGravityAccel(double acl) { phys_setup.gravityAccel = fabs(acl); }

    inline double   maxVelX() { return phys_setup.max_vel_x; }
    inline void    setMaxVelX(double mv) { phys_setup.max_vel_x = mv; }
    inline double   maxVelY() { return phys_setup.max_vel_y; }
    inline void    setMaxVelY(double mv) { phys_setup.max_vel_y = mv; }
    inline double   minVelX() { return phys_setup.min_vel_x; }
    inline void    setMinVelX(double mv) { phys_setup.min_vel_x = mv; }
    inline double   minVelY() { return phys_setup.min_vel_y; }
    inline void    setMinVelY(double mv) { phys_setup.min_vel_y = mv; }

    void _syncPosition();
    void _syncPositionAndSize();
    void _syncSection(bool sync_position=true);
    void renderDebug(double _camX, double _camY);

    void iterateStep(double ticks);
    void iterateStepPostCollide(float ticks);
    virtual void updateCollisions();
    virtual void detectCollisions(PGE_Phys_Object *collided);
    double colliding_xSpeed;
    double colliding_ySpeed;

    virtual bool onGround() { return m_stand; }

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

    static const float m_smbxTickTime;
    static float SMBXTicksToTime(float ticks);

    PGE_Phys_Object_Phys phys_setup;//!< Settings of physics
    PGE_RectF m_posRect;//!< Real body geometry and position
    double m_accelX; //!<Delta of X velocity in a second
    double m_accelY; //!<Delta of Y velocity in a second

    double m_velocityX;//!< current X speed (pixels per 1/65 of second)
    double m_velocityY;//!< current Y speed (pixels per 1/65 of second)

    double m_velocityX_prev;//!< X speed before last itertion step (pixels per 1/65 of second)
    double m_velocityY_prev;//!< Y speed before last itertion step (pixels per 1/65 of second)

    double m_velocityX_add; //!< additional X acceleration
    double m_velocityY_add; //!< additional Y acceleration

    double m_posX_registered; //!< Synchronized with R-Tree position
    double m_posY_registered; //!< Synchronized with R-Tree position

    double m_width_registered;  //!< Synchronized with R-Tree Width
    double m_height_registered; //!< Synchronized with R-Tree Height
    double m_width_half;//!< Half of width
    double m_height_half;//!< Half of height

    double m_width_toRegister;  //!< Width prepared to synchronize with R-Tree
    double m_height_toRegister; //!< Height prepared to synchronize with R-Tree

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

    //! FILTERS
    int collide_player;
    int collide_npc;

    bool m_slippery_surface;
    bool m_isRectangle;

    PGE_Texture texture;
    GLuint texId;
    virtual long double zIndex();
    long double z_index;

    /**************************Layer member****************************/
    virtual void show();
    virtual void hide();
    virtual bool isVisible();
    bool m_is_visible;
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
    virtual void render(double x, double y) { Q_UNUSED(x); Q_UNUSED(y); }

    inline bool isPaused() { return m_paused; }
    inline void setPaused(bool p) { m_paused=p; }
private:
    bool m_paused;

};



bool operator< (const PGE_Phys_Object& lhs, const PGE_Phys_Object& rhs);
bool operator> (const PGE_Phys_Object& lhs, const PGE_Phys_Object& rhs);

#endif // BASE_OBJECT_H
