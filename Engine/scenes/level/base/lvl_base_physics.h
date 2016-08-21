#ifndef LVL_BASE_PHYSICS_H
#define LVL_BASE_PHYSICS_H

#include <QHash>
#include <common_features/rectf.h>

class PGE_Phys_Object;
typedef QVector<PGE_Phys_Object *> PGE_RenderList;

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

class PGE_physBody
{
public:
    PGE_physBody();
    PGE_physBody(const PGE_physBody &o);

    void processCollisions(PGE_RenderList &objs);

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
            shape(SL_Rect)
        {}
        bool    has;
        bool    hasOld;
        objRect rect;
        int     shape;
    };
    typedef QHash<intptr_t, PGE_Phys_Object*> ObjectColliders;
    typedef QHash<intptr_t, PGE_Phys_Object*>::iterator ObjectCollidersIt;
    ObjectColliders l_contactAny;
    ObjectColliders l_contactL;
    ObjectColliders l_contactR;
    ObjectColliders l_contactT;
    ObjectColliders l_contactB;
    inline void l_pushAny(PGE_Phys_Object*ob) { l_contactAny[intptr_t(ob)] = ob; }
    inline void l_pushL(PGE_Phys_Object*ob) { l_contactL[intptr_t(ob)] = ob; l_pushAny(ob); }
    inline void l_pushR(PGE_Phys_Object*ob) { l_contactR[intptr_t(ob)] = ob; l_pushAny(ob); }
    inline void l_pushT(PGE_Phys_Object*ob) { l_contactT[intptr_t(ob)] = ob; l_pushAny(ob); }
    inline void l_pushB(PGE_Phys_Object*ob) { l_contactB[intptr_t(ob)] = ob; l_pushAny(ob); }

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

        l_contactAny.clear();
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
    //! Blocking filters (1 - playable characters, 2 - NPCs)
    int     m_blocked[3];
    //! Danger surface (1 - playable characters, 2 - NPCs)
    int     m_danger[3];
    //! Type of self (1 - playable characters, 2 - NPCs)
    int     m_filterID;
    /***********************Physical engine locals***END***********************/

};

#endif // LVL_BASE_PHYSICS_H
