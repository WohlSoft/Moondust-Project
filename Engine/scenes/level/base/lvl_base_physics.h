#ifndef LVL_BASE_PHYSICS_H
#define LVL_BASE_PHYSICS_H

#include <cmath>
#include <vector>
#include <unordered_set>
#include <common_features/rectf.h>

class PGE_Phys_Object;
typedef std::vector<PGE_Phys_Object *> PGE_RenderList;

#define BLOCK_FILTER_COUNT 3

struct PGE_Phys_Object_Phys
{
    //! Min allowed X velocity
    double min_vel_x = 0.0;
    //! Min allowed Y velocity
    double min_vel_y = 0.0;
    //! Max allowed X velocity
    double max_vel_x = 0.0;
    //! Max allowed Y velocity
    double max_vel_y = 0.0;
    //! Deceleration of X velocity in each second
    double decelerate_x = 0.0;
    //! Soft deceleration if max X speed limit exited
    double grd_dec_x = 0.0;
    //! Deceleration of Y velocity in each second
    double decelerate_y = 0.0;
    //! Item specific gravity scaling
    double gravityScale = 1.0;
    //! Item gravity acceleration
    double gravityAccel = 26.0;
};

class PGE_physBody
{
public:
    PGE_physBody();
    PGE_physBody(const PGE_physBody &o) = default;

    void processCollisions(PGE_RenderList &objs);

    /***********************Physical engine locals*****************************/
    enum ContactAt
    {
        Contact_None = 0,
        Contact_Top,
        Contact_Bottom,
        Contact_Left,
        Contact_Right,
        Contact_Skipped
    };

    enum ShapeType
    {
        SL_Rect = 0,
        SL_RightBottom,
        SL_LeftBottom,
        SL_LeftTop,
        SL_RightTop
    };

    enum BlockedSides
    {
        Block_NONE      = 0,
        Block_LEFT      = 0x1,
        Block_TOP       = 0x2,
        Block_RIGHT     = 0x4,
        Block_BOTTOM    = 0x8,
        Block_ALL       = 0xF,
    };

    enum BodyType
    {
        Body_STATIC = 0,
        Body_DYNAMIC = 1
    };

    enum CollisionCheckPolicy
    {
        CollisionCheckPolicy_EVERYTHING             = 0,
        CollisionCheckPolicy_CENTER_CONTACTS_ONLY   = 0x1,
        CollisionCheckPolicy_DISABLE_ALIGN          = 0x4,
    };

    struct objRect
    {
        objRect() {}
        objRect(double _x, double _y, double _w, double _h):
            x(_x), y(_y), w(_w), h(_h)
        {}
        double x = 0.0;
        double y = 0.0;
        double w = 0.0;
        double h = 0.0;
        inline double left()
        {
            return x;
        }
        inline double right()
        {
            return x + w;
        }
        inline double top()
        {
            return y;
        }
        inline double bottom()
        {
            return y + h;
        }
    };

    struct Momentum
    {
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
        double x = 0.0;
        //! Position Y
        double y = 0.0;
        //! Width
        double w = 32.0;
        //! Height
        double h = 32.0;
        //! Previous position X
        double oldx = 0.0;
        //! Previous position Y
        double oldy = 0.0;
        //! Previous width
        double oldw = 32.0;
        //! Previous height
        double oldh = 32.0;
        //! Real (sum of the own and floor speeds) horizontal speed (velocity)
        double velX = 0.0;
        //! Real Vertical speed (velocity)
        double velY = 0.0;
        //! Source (own) horizontal speed (velocity).
        double velXsrc = 0.0;

        inline objRect   rect()
        {
            return objRect(x, y, w, h);
        }
        inline objRect   rectOld()
        {
            return objRect(oldx, oldy, oldw, oldh);
        }
        inline PGE_RectF rectF()
        {
            return PGE_RectF(x, y, w, h);
        }
        inline PGE_RectF rectOldF()
        {
            return PGE_RectF(oldx, oldy, oldw, oldh);
        }

        /**
         * @brief Copy all current values to the "old"
         */
        inline void saveOld()
        {
            oldx = x;
            oldy = y;
            oldw = w;
            oldh = h;
        }

        inline void setXatLeft(double newx)
        {
            x = newx;
        }
        inline void setXatRight(double newx)
        {
            x = newx - w;
        }
        inline void setYatTop(double newy)
        {
            y = newy;
        }
        inline void setYatBottom(double newy)
        {
            y = newy - h;
        }
        inline void setCenterX(double newx)
        {
            x = newx - w / 2.0;
        }
        inline void setCenterY(double newy)
        {
            y = newy - h / 2.0;
        }
        inline void setCenterPos(double X, double Y)
        {
            x = X - (w / 2.0);
            y = Y - (h / 2.0);
        }
        inline void setLeft(double newx)
        {
            w = std::fabs(newx - (x + w));
            x = newx;
        }
        inline void setRight(double newx)
        {
            w = std::fabs(x - newx);
        }
        inline void setTop(double newy)
        {
            h = std::fabs(newy - (y + h));
            y = newy;
        }
        inline void setBottom(double newy)
        {
            h = std::fabs(y - newy);
        }

        inline double left()
        {
            return x;
        }
        inline double top()
        {
            return y;
        }
        inline double right()
        {
            return x + w;
        }
        inline double bottom()
        {
            return y + h;
        }
        inline double centerX()
        {
            return x + (w / 2.0);
        }
        inline double centerY()
        {
            return y + (h / 2.0);
        }

        inline double leftOld()
        {
            return oldx;
        }
        inline double topOld()
        {
            return oldy;
        }
        inline double rightOld()
        {
            return oldx + oldw;
        }
        inline double bottomOld()
        {
            return oldy + oldh;
        }
        inline double centerXold()
        {
            return oldx + (oldw / 2.0);
        }
        inline double centerYold()
        {
            return oldy + (oldh / 2.0);
        }

        inline  bool   betweenH(double left, double right)
        {
            if(right < x) return false;
            if(left > x + w) return false;
            return true;
        }
        inline  bool   betweenH(double X)
        {
            return (X >= x) && (X <= x + w);
        }
        inline  bool   betweenV(double top, double bottom)
        {
            if(bottom < y) return false;
            if(top > y + h) return false;
            return true;
        }
        inline  bool   betweenV(double Y)
        {
            return (Y >= y) && (Y <= y + h);
        }
    };

    struct SlopeState
    {
        bool    has = false;
        bool    hasOld = false;
        objRect rect;
        int     shape = SL_Rect;
    };
    typedef std::unordered_set<PGE_Phys_Object *> ObjectColliders;
    typedef std::unordered_set<PGE_Phys_Object *>::iterator ObjectCollidersIt;
    ObjectColliders l_contactAny;
    ObjectColliders l_contactL;
    ObjectColliders l_contactR;
    ObjectColliders l_contactT;
    ObjectColliders l_contactB;
    inline void l_pushAny(PGE_Phys_Object *ob)
    {
        l_contactAny.insert(ob);
    }
    inline void l_pushL(PGE_Phys_Object *ob)
    {
        l_contactL.insert(ob);
        l_pushAny(ob);
    }
    inline void l_pushR(PGE_Phys_Object *ob)
    {
        l_contactR.insert(ob);
        l_pushAny(ob);
    }
    inline void l_pushT(PGE_Phys_Object *ob)
    {
        l_contactT.insert(ob);
        l_pushAny(ob);
    }
    inline void l_pushB(PGE_Phys_Object *ob)
    {
        l_contactB.insert(ob);
        l_pushAny(ob);
    }

    int         m_shape = SL_Rect;
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
    bool    m_touchLeftWall = false;
    bool    m_touchRightWall = false;
    bool    m_blockedAtLeft = false;
    bool    m_blockedAtRight = false;
    bool    m_stand = false;
    bool    m_standOnYMovable = false;
    bool    m_crushed = false;
    bool    m_crushedOld = false;
    bool    m_crushedHard = false;
    int     m_crushedHardDelay = 0;
    bool    m_cliff = false;
    bool    m_onSlippery = false;
    /*************************************/
    double  m_contactPadding = 0.0;
    bool    m_moveLeft = false;
    bool    m_moveRight = false;
    SlopeState m_slopeFloor;
    SlopeState m_slopeCeiling;
    //! Y-speed add while standing on the slope
    double  m_onSlopeYAdd = 0.0;
    //! Body type
    BodyType m_bodytype = Body_STATIC;
    int      m_collisionCheckPolicy = CollisionCheckPolicy_EVERYTHING;

    //! Allow running over floor holes
    bool    m_allowHoleRuning = false;
    //! Enable automatical aligning of position while staying on top corner of slope
    bool    m_onSlopeFloorTopAlign = false;
    //! Blocking filters (1 - playable characters, 2 - NPCs)
    int     m_blocked[BLOCK_FILTER_COUNT] = {Block_NONE, Block_ALL, Block_ALL};
    //! Danger surface (1 - playable characters, 2 - NPCs)
    int     m_danger[BLOCK_FILTER_COUNT] = {Block_NONE, Block_NONE, Block_NONE};
    //! Type of self (1 - playable characters, 2 - NPCs)
    int     m_filterID = 0;
    int     collidePlayer()
    {
        return m_blocked[1];
    }
    void    setCollidePlayer(int cp)
    {
        m_blocked[1] = cp;
    }
    int     collideNpc()
    {
        return m_blocked[2];
    }
    void    setCollideNpc(int cn)
    {
        m_blocked[2] = cn;
    }
    /***********************Physical engine locals***END***********************/

};

#endif // LVL_BASE_PHYSICS_H
