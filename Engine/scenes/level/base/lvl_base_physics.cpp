/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "lvl_base_physics.h"

#include "../lvl_player.h"
#include "../lvl_bgo.h"
#include "../lvl_block.h"
#include "../lvl_npc.h"
#include "../lvl_physenv.h"
#include "../lvl_warp.h"

#include "../../scene_level.h"

#include "../lvl_base_object.h"
#include <scenes/scene_level.h>
#include <graphics/gl_renderer.h>
#include <Utils/maths.h>

#include <audio/pge_audio.h>
#include <vector>

    /*****Physical engine locals*******/
//    m_shape(SL_Rect),
//    m_momentum(0.0, 0.0),
//    m_touchLeftWall(false),
//    m_touchRightWall(false),
//    m_blockedAtLeft(false),
//    m_blockedAtRight(false),
//    m_stand(false),
//    m_standOnYMovable(false),
//    m_crushed(false),
//    m_crushedOld(false),
//    m_crushedHard(false),
//    m_crushedHardDelay(0),
//    m_cliff(false),
//    m_onSlippery(false),
//    m_contactPadding(0.0),
//    m_moveLeft(false),
//    m_moveRight(false),
//    m_onSlopeYAdd(0.0),
//    m_bodytype(Body_STATIC),
//    m_allowHoleRuning(false),
//    m_onSlopeFloorTopAlign(false),
//    m_blocked{Block_NONE, Block_ALL, Block_ALL},
//    m_danger{Block_NONE, Block_NONE, Block_NONE},
//    m_filterID(0)
    /*****Physical engine locals*END***/


//PGE_physBody::PGE_physBody(const PGE_physBody &o) :
//    m_shape(o.m_shape),
//    m_momentum(o.m_momentum),
//    m_touchLeftWall(o.m_touchLeftWall),
//    m_touchRightWall(o.m_touchRightWall),
//    m_blockedAtLeft(o.m_blockedAtLeft),
//    m_blockedAtRight(o.m_blockedAtRight),
//    m_stand(o.m_stand),
//    m_standOnYMovable(o.m_standOnYMovable),
//    m_crushed(o.m_crushed),
//    m_crushedOld(o.m_crushedOld),
//    m_crushedHard(o.m_crushedHard),
//    m_crushedHardDelay(o.m_crushedHardDelay),
//    m_cliff(o.m_cliff),
//    m_onSlippery(o.m_onSlippery),
//    m_contactPadding(o.m_contactPadding),
//    m_moveLeft(o.m_moveLeft),
//    m_moveRight(o.m_moveRight),
//    m_slopeFloor(o.m_slopeFloor),
//    m_slopeCeiling(o.m_slopeCeiling),
//    m_onSlopeYAdd(o.m_onSlopeYAdd),
//    m_bodytype(o.m_bodytype),
//    m_allowHoleRuning(o.m_allowHoleRuning),
//    m_onSlopeFloorTopAlign(o.m_onSlopeFloorTopAlign),
//    m_blocked{o.m_blocked[0], o.m_blocked[1]},
//    m_filterID(o.m_filterID)
//{}

static inline void processCharacterSwitchBlock(LVL_Player *player, LVL_Block *nearest)
{
    //Do transformation if needed
    if(nearest->setup->setup.plSwitch_Button && (player->characterID != nearest->setup->setup.plSwitch_Button_id))
    {
        size_t target_id = static_cast<size_t>(nearest->setup->setup.plSwitch_Button_id - 1);
        std::vector<saveCharState> &states = player->m_scene->getGameState()->m_gameSave.characterStates;
        if(target_id >= states.size())
        {
            PlayerState x = player->m_scene->getGameState()->getPlayerState(player->playerID);
            x.characterID    = static_cast<unsigned int>(target_id + 1);
            x.stateID        = 1;
            x._chsetup.state = 1;
            player->m_scene->getGameState()->setPlayerState(player->playerID, x);
        }

        saveCharState &st = states[target_id];
        player->setCharacterSafe(nearest->setup->setup.plSwitch_Button_id, st.state);
    }
}

void PGE_Phys_Object::iterateStep(double ticks, bool force)
{
    if(m_paused && !force)
        return;
//FIXME: Fix the missing in-area detector's trap position
    if(m_parent && (m_bodytype == Body_DYNAMIC))
    {
        m_momentum.velX     = m_momentum_relative.velX;
        m_momentum.velXsrc  = m_momentum_relative.velXsrc;
        m_momentum.velY     = m_momentum_relative.velY;
    }

    double G = phys_setup.gravityScale * m_scene->m_globalGravity;
    double accelCof = ticks / 1000.0;
    double Xmod = 0.0;

    // Iterate accelerators
    if(m_accelX != 0.0)
    {
        Xmod += m_accelX * accelCof;
        m_accelX = 0.0;
    }
    else if(phys_setup.decelerate_x != 0.0)
    {
        double decX = phys_setup.decelerate_x * accelCof;

        if(m_momentum.velXsrc > 0.0)
        {
            if(m_momentum.velXsrc - decX > 0.0)
                m_momentum.velXsrc -= decX;
            else
                m_momentum.velXsrc = 0;
        }
        else if(m_momentum.velXsrc < 0.0)
        {
            if(m_momentum.velXsrc + decX < 0.0)
                m_momentum.velXsrc += decX;
            else
                m_momentum.velXsrc = 0;
        }
    }

    m_momentum.velX    += Xmod;
    m_momentum.velXsrc += Xmod;

    if(((m_accelY != 0.0) && !m_stand) ||
       ((m_accelY <  0.0) &&  m_stand))
    {
        m_momentum.velY += m_accelY * accelCof /* * ( (G==0.0f) ? 1.0f : G )*/;
        m_accelY = 0.0;
    }

    //    if( ((phys_setup.gravityAccel != 0.0f) && !onGround()) ||
    //        ((phys_setup.gravityAccel < 0.0f)  &&  onGround()) )
    /*
    if( (m_momentum.velY < 8) &&
        (!m_stand || m_standOnYMovable ||
        (!m_allowHoleRuning && m_cliff && (m_momentum.velXsrc != 0.0)) )
       )
    */
    //Affect gravity only to dynamic objects!
    if(m_bodytype == Body_DYNAMIC)
    {
        if(
            (G * phys_setup.gravityAccel < 0.0) ||
            (
                (G * phys_setup.gravityAccel != 0.0) &&
                (!m_stand || m_standOnYMovable ||
                 (!m_allowHoleRuning && m_cliff && (m_momentum.velXsrc != 0.0)))
            )
        )
            m_momentum.velY += (G * phys_setup.gravityAccel) * accelCof;
    }

    if((m_accelY == 0.0) && (phys_setup.decelerate_y != 0.0))
    {
        double decY = phys_setup.decelerate_y * accelCof;

        if(m_momentum.velY > 0)
        {
            if(m_momentum.velY - decY > 0.0)
                m_momentum.velY -= decY;
            else
                m_momentum.velY = 0;
        }
        else if(m_momentum.velY < 0)
        {
            if(m_momentum.velY + decY < 0.0)
                m_momentum.velY += decY;
            else
                m_momentum.velY = 0;
        }
    }

    /*
    if( (m_momentum.velY < phys_setup.max_vel_y) &&
        (!onGround() || m_standOnYMovable ||
        (!m_allowHoleRuning && m_cliff && (m_momentum.velXsrc != 0.0)) )
       )
    */

    if((phys_setup.max_vel_x != 0.0) && (m_momentum.velXsrc > phys_setup.max_vel_x))
        m_momentum.velXsrc -= phys_setup.grd_dec_x * accelCof;

    if((phys_setup.min_vel_x != 0.0) && (m_momentum.velXsrc < phys_setup.min_vel_x))
        m_momentum.velXsrc += phys_setup.grd_dec_x * accelCof;

    if((phys_setup.max_vel_y != 0.0) && (m_momentum.velY > phys_setup.max_vel_y))
        m_momentum.velY = phys_setup.max_vel_y;

    if((phys_setup.min_vel_y != 0.0) && (m_momentum.velY < phys_setup.min_vel_y))
        m_momentum.velY = phys_setup.min_vel_y;

    if(!m_stand)
        m_momentum.velX = m_momentum.velXsrc;

    double moveStepRatio = ticks / (1000.0 / 65);

    double iterateX = m_momentum.velX * moveStepRatio;
    double iterateY = m_momentum.velY * moveStepRatio;

    if(m_slopeFloor.has)
        iterateY += m_onSlopeYAdd * moveStepRatio;

    // Iterate movement
    m_momentum.saveOld();
    m_momentum.x += iterateX;
    m_momentum.y += iterateY;

    Maths::clearPrecision(m_momentum.x);
    Maths::clearPrecision(m_momentum.y);

//FIXME: Fix the missing in-area detector's trap position
    if(m_parent)
    {
        if(m_bodytype == Body_DYNAMIC)
        {
            m_momentum_relative.velX    = m_momentum.velX;
            m_momentum_relative.velXsrc = m_momentum.velXsrc;
            m_momentum_relative.velY    = m_momentum.velY;
        }
        // Iterate relative movement if needed
        m_momentum_relative.saveOld();
        m_momentum_relative.x += iterateX;
        m_momentum_relative.y += iterateY;

        Maths::clearPrecision(m_momentum_relative.x);
        Maths::clearPrecision(m_momentum_relative.y);
    }
    m_treemap.updatePos();
}

void PGE_Phys_Object::iterateStepPostCollide(float ticks)
{
    (void)(ticks);
#if 0
    bool updateSpeedAdding = true;
    float G = phys_setup.gravityScale * _scene->globalGravity;
    float accelCof = ticks / 1000.0f;

    if(m_accelX != 0.0f)
    {
        m_velocityX += m_accelX * accelCof;
        updateSpeedAdding = true;
        m_accelX = 0;
    }
    else if(phys_setup.decelerate_x != 0.0f)
    {
        float decX = phys_setup.decelerate_x * accelCof;

        if(m_velocityX > 0)
        {
            if(m_velocityX - decX > 0.0)
                m_velocityX -= decX;
            else
                m_velocityX = 0;
        }
        else if(m_velocityX < 0)
        {
            if(m_velocityX + decX < 0.0)
                m_velocityX += decX;
            else
                m_velocityX = 0;
        }

        updateSpeedAdding = true;
    }

    if(((m_accelY != 0.0f) && !onGround()) ||
       ((m_accelY <  0.0f) &&  onGround()))
    {
        m_velocityY += m_accelY * accelCof * ((G == 0.0f) ? 1.0f : G);
        updateSpeedAdding = true;
        m_accelY = 0.0f;
    }

    if(phys_setup.decelerate_y != 0.0f)
    {
        float decY = phys_setup.decelerate_y * accelCof;

        if(m_velocityY > 0)
        {
            if(m_velocityY - decY > 0.0)
                m_velocityY -= decY;
            else
                m_velocityY = 0;
        }
        else if(m_velocityY < 0)
        {
            if(m_velocityY + decY < 0.0)
                m_velocityY += decY;
            else
                m_velocityY = 0;
        }

        updateSpeedAdding = true;
    }

    if(((phys_setup.gravityAccel != 0.0f) && !onGround()) ||
       ((phys_setup.gravityAccel < 0.0f)  &&  onGround()))
    {
        m_velocityY += (G * phys_setup.gravityAccel) * accelCof;
        updateSpeedAdding = true;
    }

    if((phys_setup.max_vel_x != 0.0f) && (m_velocityX > phys_setup.max_vel_x))
    {
        m_velocityX -= phys_setup.grd_dec_x * accelCof;
        updateSpeedAdding = true;
    }

    if((phys_setup.min_vel_x != 0.0f) && (m_velocityX < phys_setup.min_vel_x))
    {
        m_velocityX += phys_setup.grd_dec_x * accelCof;
        updateSpeedAdding = true;
    }

    if((phys_setup.max_vel_y != 0.0f) && (m_velocityY > phys_setup.max_vel_y))
    {
        m_velocityY = phys_setup.max_vel_y;
        updateSpeedAdding = true;
    }

    if((phys_setup.min_vel_y != 0.0f) && (m_velocityY < phys_setup.min_vel_y))
    {
        m_velocityY = phys_setup.min_vel_y;
        updateSpeedAdding = true;
    }

    if(updateSpeedAdding)
        updateSpeedAddingStack();

#endif
}

typedef PGE_physBody PhysObject;

template <class TArray> void findHorizontalBoundaries(TArray &array, double &lefter, double &righter,
        PhysObject **leftest = nullptr, PhysObject **rightest = nullptr)
{
    if(array.empty())
        return;

    for(unsigned int i = 0; i < array.size(); i++)
    {
        PhysObject *x = array[i];

        if(x->m_momentum.left() < lefter)
        {
            lefter = x->m_momentum.left();

            if(leftest)
                *leftest = x;
        }

        if(x->m_momentum.right() > righter)
        {
            righter = x->m_momentum.right();

            if(rightest)
                *rightest = x;
        }
    }
}

template <class TArray> void findVerticalBoundaries(TArray &array, double &higher, double &lower,
        PhysObject **highest = nullptr, PhysObject **lowerest = nullptr)
{
    if(array.empty())
        return;

    for(int i = 0; i < array.size(); i++)
    {
        PhysObject *x = array[i];

        if(x->m_momentum.y < higher)
        {
            higher = x->m_momentum.y;

            if(highest)
                *highest = x;
        }

        if(x->m_momentum.y + x->m_momentum.h > lower)
        {
            lower = x->m_momentum.y + x->m_momentum.h;

            if(lowerest)
                *lowerest = x;
        }
    }
}

static inline bool pt(double x1, double y1, double w1, double h1,
                      double x2, double y2, double w2, double h2)
{
    if((y1 + h1 > y2) && (y2 + h2 > y1))
        return ((x1 + w1 > x2) && (x2 + w2 > x1));

    return false;
}

static inline bool recttouch(double X,  double Y,   double w,   double h,
                             double x2, double y2,  double w2,  double h2)
{
    return ((X + w > x2) && (x2 + w2 > X) && (Y + h > y2) && (y2 + h2 > Y));
}

static inline bool figureTouch(PhysObject::Momentum &pl, PhysObject::Momentum &block, double marginV = 0.0, double marginH = 0.0)
{
    return recttouch(pl.x + marginH, pl.y + marginV, pl.w - (marginH * 2.0), pl.h - (marginV * 2.0),
                     block.x,      block.y,      block.w,            block.h);
}

inline bool isBlockFloor(int shape)
{
    return (shape == PhysObject::SL_Rect) ||
           (shape == PhysObject::SL_LeftTop) ||
           (shape == PhysObject::SL_RightTop);
}

inline bool isBlockCeiling(int shape)
{
    return (shape == PhysObject::SL_Rect) ||
           (shape == PhysObject::SL_LeftBottom) ||
           (shape == PhysObject::SL_RightBottom);
}

inline bool isBlockLeftWall(int shape)
{
    return (shape == PhysObject::SL_Rect) ||
           (shape == PhysObject::SL_LeftBottom) ||
           (shape == PhysObject::SL_LeftTop);
}

inline bool isBlockRightWall(int shape)
{
    return (shape == PhysObject::SL_Rect) ||
           (shape == PhysObject::SL_RightBottom) ||
           (shape == PhysObject::SL_RightTop);
}

inline bool isBlockFloor(PhysObject *block)
{
    return (block->m_shape == PhysObject::SL_Rect) ||
           (block->m_shape == PhysObject::SL_LeftTop) ||
           (block->m_shape == PhysObject::SL_RightTop);
}

inline bool isBlockCeiling(PhysObject *block)
{
    return (block->m_shape == PhysObject::SL_Rect) ||
           (block->m_shape == PhysObject::SL_LeftBottom) ||
           (block->m_shape == PhysObject::SL_RightBottom);
}

inline bool isBlockLeftWall(PhysObject *block)
{
    return (block->m_shape == PhysObject::SL_Rect) ||
           (block->m_shape == PhysObject::SL_LeftBottom) ||
           (block->m_shape == PhysObject::SL_LeftTop);
}

inline bool isBlockRightWall(PhysObject *block)
{
    return (block->m_shape == PhysObject::SL_Rect) ||
           (block->m_shape == PhysObject::SL_RightBottom) ||
           (block->m_shape == PhysObject::SL_RightTop);
}

/**
 * @brief Finds minimal available height between slope floor and slope ceiling (and between horizontal block and one of slopes)
 * @param idF shape of floor
 * @param sF rectangle of floor
 * @param vF velocity of floor
 * @param idC shape of ceiling
 * @param sC rectangle of ceilig
 * @param player Pointer to the player
 * @return true if slope floor<->ceiling collision has been detected and resolved. False if found nothing
 */
inline bool findMinimalHeight(int idF, PhysObject::objRect sF, double vF,
                              int idC, PhysObject::objRect sC, PhysObject *player)
{
    bool slT  =     player->m_slopeCeiling.has;
    bool slB  =     player->m_slopeFloor.has;
    bool &wallL =   player->m_blockedAtLeft;
    bool &wallR =   player->m_blockedAtRight;
    double &w =     player->m_momentum.w;
    double &h =     player->m_momentum.h;
    double &posX =  player->m_momentum.x;
    double &posY =  player->m_momentum.y;
    double &velX =  player->m_momentum.velXsrc;
    double k1   =   sF.h / sF.w;
    double k2   =   sC.h / sC.w;
    bool k1_NE_k2 = !Maths::equals(k1, k2);

    /***************************Ceiling and floor slopes*******************************/
    if(slT && slB && (idF == PhysObject::SL_LeftBottom) && (idC == PhysObject::SL_LeftTop))
    {
        //LeftBottom
        double Y1 = sF.y + ((posX - sF.x) * k1) - h;
        //LeftTop
        double Y2 = sC.bottom() - ((posX - sC.x) * k2);

        while((Y1 < Y2) && (posX <= sC.right()))
        {
            posX += 1.0;
            Y1 = sF.y + ((posX - sF.x) * k1) - h;
            Y2 = sC.bottom() - ((posX - sC.x) * k2);
        }

        posY = Y1;
        velX = std::max(velX, vF);
        wallL = true;
        return true;
    }
    else if(slT && slB && (idF == PhysObject::SL_RightBottom) && (idC == PhysObject::SL_RightTop))
    {
        //RightBottom
        double Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
        //RightTop
        double Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);

        while((Y1 < Y2) && ((posX + w) >= sC.left()))
        {
            posX -= 1.0;
            Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
            Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);
        }

        posY = Y1;
        velX = std::min(velX, vF);
        wallR = true;
        return true;
    }
    else if(slT && slB && (idF == PhysObject::SL_RightBottom) && (idC == PhysObject::SL_LeftTop) && k1_NE_k2)
    {
        //RightBottom
        double Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
        //LeftTop
        double Y2 = sC.bottom() - ((posX - sC.x) * k2);

        while((Y1 < Y2) /*&& ( (posX + w) >= sC.left())*/)
        {
            posX += (k1 > k2) ? -1.0 : 1.0;
            Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
            Y2 = sC.bottom() - ((posX - sC.x) * k2);
        }

        posY = Y2;
        velX = (k1 > k2) ? std::min(velX, vF) : std::max(velX, vF);
        wallL = (k1 <= k2);
        wallR = (k1 > k2);
        return true;
    }
    else if(slT && slB && (idF == PhysObject::SL_LeftBottom) && (idC == PhysObject::SL_RightTop) && k1_NE_k2)
    {
        //LeftBottom
        double Y1 = sF.y + ((posX - sF.x) * k1) - h;
        //RightTop
        double Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);

        while((Y1 < Y2) /*&& ( (posX + w) >= sC.left())*/)
        {
            posX += (k1 < k2) ? -1.0 : 1.0;
            Y1 = sF.y + ((posX - sF.x) * k1) - h;
            Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);
        }

        posY = Y2;
        velX = (k1 < k2) ? std::min(velX, vF) : std::max(velX, vF);
        wallL = (k1 >= k2);
        wallR = (k1 < k2);
        return true;
    }
    /***************************Ceiling slope and horizontal floor*******************************/
    else if(slT && isBlockFloor(idF) && (idC == PhysObject::SL_LeftTop))
    {
        double Y1 = sF.y - h;
        double Y2 = sC.bottom() - ((posX - sC.x) * k2);

        while((Y1 < Y2) && (posX <= sC.right()))
        {
            posX += 1.0;
            Y2 = sC.bottom() - ((posX - sC.x) * k2);
        }

        posY = Y1;
        velX = std::max(velX, vF);
        wallL = true;
        return true;
    }
    else if(slT && isBlockFloor(idF) && (idC == PhysObject::SL_RightTop))
    {
        double Y1 = sF.y - h;
        double Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);

        while((Y1 < Y2) && ((posX + w) >= sC.left()))
        {
            posX -= 1.0;
            Y2 = sC.bottom() - ((sC.right() - posX - w) * k2);
        }

        posY = Y1;
        velX = std::min(velX, vF);
        wallR = true;
        return true;
    }
    /***************************Floor slope and horizontal ceiling*******************************/
    else if(slB && (idF == PhysObject::SL_LeftBottom) && isBlockCeiling(idC))
    {
        double Y1 = sF.y + ((posX - sF.x) * k1) - h;
        double Y2 = sC.bottom();

        while((Y1 < Y2) && (posX < sC.right()))
        {
            posX += 1.0;
            Y1 = sF.y + ((posX - sF.x) * k1) - h;
        }

        posY = Y2;
        velX = std::max(velX, vF);
        wallL = true;
        return true;
    }
    else if(slB && (idF == PhysObject::SL_RightBottom) && isBlockCeiling(idC))
    {
        double Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
        double Y2 = sC.bottom();

        while((Y1 < Y2) && ((posX + w) > sC.left()))
        {
            posX -= 1.0;
            Y1 = sF.y + ((sF.right() - posX - w) * k1) - h;
        }

        posY = Y2;
        velX = std::min(velX, vF);
        wallR = true;
        return true;
    }

    return false;
}

void PGE_Phys_Object::updateCollisions()
{
    if(m_paused)
        return;

    std::vector<PGE_Phys_Object *> objs;
    objs.reserve(25);
    bool hasSlopes = false;
    PGE_RectF posRectC = m_momentum.rectF().withMargin(m_momentum.w / 2.0);

    if(m_slopeFloor.has || m_slopeFloor.hasOld)
    {
        posRectC.setRight(posRectC.right() + m_slopeFloor.rect.w);
        posRectC.setLeft(posRectC.left() - m_slopeFloor.rect.w);
        posRectC.setBottom(posRectC.bottom() + m_slopeFloor.rect.h * 1.5);
    }

    std::function<bool(PGE_Phys_Object*)> itemValidator = [this, &hasSlopes](PGE_Phys_Object *CUR)->bool
    {
        bool isValid = (CUR) &&
                        (dynamic_cast<PGE_Phys_Object*>(CUR) != dynamic_cast<PGE_Phys_Object*>(this)) &&
                        (!CUR->m_paused) &&
                        (CUR->m_is_visible);
        if(isValid)
            hasSlopes |= (CUR->m_shape != PhysObject::SL_Rect);
        return isValid;
    };

    m_scene->queryItems(posRectC, &objs, &itemValidator);

    // Put slopes into the top of order
    // This will avoid nearest rectangular blocks to confuse alignment of object at the slope top
    if(hasSlopes) // Order the array for only case when slope blocks are found
    {
        bool moveLeft = m_momentum.velX < 0.0;

        std::sort( objs.begin( ), objs.end( ), [moveLeft](PGE_Phys_Object * &lhs, PGE_Phys_Object * &rhs)->bool
        {
            int l = 0, r = 0;
            if(moveLeft)
            {
                if(lhs->m_shape == PhysObject::SL_Rect)
                    l = 2;
                else if(lhs->m_shape == PhysObject::SL_RightBottom || lhs->m_shape == PhysObject::SL_RightTop || lhs->m_shape == PhysObject::SL_LeftTop)
                    l = 1;

                if(rhs->m_shape == PhysObject::SL_Rect)
                    r = 2;
                else if(rhs->m_shape == PhysObject::SL_RightBottom || rhs->m_shape == PhysObject::SL_RightTop || rhs->m_shape == PhysObject::SL_LeftTop)
                    r = 1;
            }
            else
            {
                if(lhs->m_shape == PhysObject::SL_Rect)
                    l = 2;
                else if(lhs->m_shape == PhysObject::SL_LeftBottom || lhs->m_shape == PhysObject::SL_LeftTop || lhs->m_shape == PhysObject::SL_RightTop)
                    l = 1;

                if(rhs->m_shape == PhysObject::SL_Rect)
                    r = 2;
                else if(rhs->m_shape == PhysObject::SL_LeftBottom || rhs->m_shape == PhysObject::SL_LeftTop || rhs->m_shape == PhysObject::SL_RightTop)
                    r = 1;
            }
            return l < r;
        });
    }

    double k = 0;
    int tm = -1, td = 0;
    PhysObject::ContactAt contactAt = PhysObject::Contact_None;
    bool colH = false, colV = false;
    bool doHit = false;
    bool doCliffCheck = false;
    bool xSpeedWasReversed = false;
    std::vector<PGE_Phys_Object *> l_clifCheck;
    std::vector<PGE_Phys_Object *> l_toBump;
    PhysObject *collideAtTop  = nullptr;
    PhysObject *collideAtBottom = nullptr;
    PhysObject *collideAtLeft  = nullptr;
    PhysObject *collideAtRight = nullptr;
    bool    policy_CenterContactsOnly = (m_collisionCheckPolicy & CollisionCheckPolicy_CENTER_CONTACTS_ONLY) != 0;
    bool    doSpeedStack = true;
    double  speedNum = 0.0;
    double  speedSum = 0.0;
    bool blockSkip = false;
    PGE_SizeT   blockSkipStartFrom = 0;
    PGE_SizeT   blockSkipI = 0;
    double oldSpeedX = m_momentum.velX;
    double oldSpeedY = m_momentum.velY;
    //resetEvents();
    preCollision();
    PGE_Phys_Object *CUR = nullptr;
    //PGE_Phys_Object* CURO=nullptr;
    PGE_SizeT i = 0;

    for(i = 0; i < objs.size(); i++)
    {
        if(blockSkip && (blockSkipI == i))
        {
            blockSkip = false;
            continue;
        }

        CUR = objs[i];
        //if((!CUR) || (dynamic_cast<PGE_Phys_Object*>(CUR) == dynamic_cast<PGE_Phys_Object*>(this)) || (CUR->m_paused) || (!CUR->m_is_visible))
        //{
        //    objs.erase(objs.begin() + static_cast<std::vector<PGE_Phys_Object *>::difference_type>(i));
        //    --i;
        //    continue;
        //}
        if(policy_CenterContactsOnly || preCollisionCheck(CUR) || (CUR->m_blocked[m_filterID] == Block_NONE))
        {
            if(
                (CUR->m_shape == PhysObject::SL_Rect) &&
                figureTouch(m_momentum, CUR->m_momentum, CUR->m_contactPadding, CUR->m_contactPadding)
            )
                l_pushAny(CUR);
            continue;
        }

#ifdef IS_MINIPHYSICS_DEMO_PROGRAM
        CUR->m_bumped = false;
#endif
        contactAt = PhysObject::Contact_None;

        /* ********************Collect blocks to hit************************ */
        if(figureTouch(m_momentum, objs[i]->m_momentum, -1.0, 0.0))
        {
            if(m_momentum.centerY() < CUR->m_momentum.centerY())
            {
                if(((CUR->m_blocked[m_filterID] & Block_TOP) != 0) &&
                   (CUR->m_momentum.top() >= m_momentum.bottom())
                  )
                    l_clifCheck.push_back(CUR);
            }
            else
            {
                if((CUR->m_blocked[m_filterID] & Block_BOTTOM) != 0)
                    l_toBump.push_back(CUR);

                if((CUR->m_bodytype == Body_DYNAMIC) &&
                   (CUR->m_stand) &&
                   (CUR->bottom() <= top()) && (CUR->bottom() >= top() - 1.0))
                    l_pushT(CUR);
            }
        }

        /* ****Collect extra candidates for a cliff detection on the slope******** */
        if(((CUR->m_blocked[m_filterID]&Block_TOP) != 0) &&
           (m_slopeFloor.has || m_slopeFloor.hasOld)
          )
        {
            PhysObject::Momentum &mPlr = m_momentum;
            PhysObject::Momentum &mBlk = CUR->m_momentum;
            PhysObject::objRect &r1 = m_slopeFloor.rect;
            PhysObject::objRect  r2 = CUR->m_momentum.rect();

            if((m_slopeFloor.shape == PhysObject::SL_LeftBottom) && (mPlr.velX >= 0.0))
            {
                if(recttouch(mPlr.x + (mPlr.w / 2.0), mPlr.centerY(), mPlr.w, (r2.h + r1.h * 1.5),
                             mBlk.x, mBlk.y, mBlk.w, mBlk.h)
                   &&
                   //is touching corners
                   (((r1.x + r1.w) >= (r2.x - 1.0)) &&
                    ((r1.x + r1.w) <= (r2.x + r2.w)) &&
                    ((r1.y + r1.h) >= (r2.y - 1.0)) &&
                    ((r1.y + r1.h) <= (r2.y + 1.0))) &&
                   (mBlk.top() > mPlr.bottom()))
                    l_clifCheck.push_back(CUR);
            }
            else if((m_slopeFloor.shape == PhysObject::SL_RightBottom) && (mPlr.velX <= 0.0))
            {
                if(recttouch(mPlr.x - (mPlr.w / 2.0), mPlr.centerY(), mPlr.w, (r2.h + r1.h * 1.5),
                             mBlk.x, mBlk.y, mBlk.w, mBlk.h)
                   &&
                   //is touching corners
                   (((r1.x) >= (r2.x)) &&
                    ((r1.x) <= (r2.x + r2.w + 1.0)) &&
                    ((r1.y + r1.h) >= (r2.y - 1.0)) &&
                    ((r1.y + r1.h) <= (r2.y + 1.0))))
                    l_clifCheck.push_back(CUR);
            }
        }

        /* ************************************************************************* */

        if((m_momentum.x + m_momentum.w > CUR->m_momentum.x) && (CUR->m_momentum.x + CUR->m_momentum.w > m_momentum.x))
        {
            if(Maths::equals(m_momentum.y + m_momentum.h, CUR->m_momentum.y))
                goto tipRectV;
        }

tipRectShape://Recheck rectangular collision

        if(pt(m_momentum.x, m_momentum.y, m_momentum.w, m_momentum.h,
              CUR->m_momentum.x, CUR->m_momentum.y, CUR->m_momentum.w, CUR->m_momentum.h))
        {
            colH = pt(m_momentum.x,     m_momentum.oldy,  m_momentum.w, m_momentum.oldh,
                    CUR->m_momentum.x,    CUR->m_momentum.oldy, CUR->m_momentum.w, CUR->m_momentum.oldh);
            colV = pt(m_momentum.oldx,  m_momentum.y,     m_momentum.oldw, m_momentum.h,
                    CUR->m_momentum.oldx, CUR->m_momentum.y,    CUR->m_momentum.oldw, CUR->m_momentum.h);

            if(colH ^ colV)
            {
                if(!colH)
                {
tipRectV://Check vertical sides collisions

                    if(m_momentum.centerY() < CUR->m_momentum.centerY())
                    {
                        //'top
                        if(isBlockFloor(CUR))
                        {
tipRectT://Impacted at top of block (bottom of player)

                            if((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) == 0)
                                goto tipRectT_Skip;

                            m_momentum.y = CUR->m_momentum.y - m_momentum.h;

                            if(CUR->m_momentum.velY <= m_momentum.velY)
                                m_momentum.velY   = CUR->m_momentum.velY;

                            m_stand  = true;
                            m_standOnYMovable = (CUR->m_momentum.velY != 0.0);

                            if(doSpeedStack)
                            {
                                m_momentum.velX   = m_momentum.velXsrc + CUR->m_momentum.velX;
                                speedSum += CUR->m_momentum.velX;

                                if(CUR->m_momentum.velX != 0.0)
                                    speedNum += 1.0;
                            }

                            contactAt = PhysObject::Contact_Top;
                            doCliffCheck = true;
                            l_clifCheck.push_back(CUR);
                            l_pushB(CUR);
                            collideAtBottom = CUR;

                            //CUR->m_momentum.touch = contactAt;
                            if(m_slopeCeiling.has)
                            {
                                if(findMinimalHeight(CUR->m_shape, CUR->m_momentum.rect(), CUR->m_momentum.velX,
                                                     m_slopeCeiling.shape, m_slopeCeiling.rect, this))
                                {
                                    m_momentum.velX = m_momentum.velXsrc;
                                    doSpeedStack = false;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }
                        }

tipRectT_Skip:
                        ;
                    }
                    else
                    {
                        //'bottom
                        if(isBlockCeiling(CUR))
                        {
tipRectB://Impacted at bottom of block (top of player)

                            if((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) == 0)
                                goto tipRectB_Skip;

                            if(CUR->m_bodytype != Body_STATIC)
                            {
                                l_pushT(CUR);
                                goto tipRectB_Skip;
                            }

                            /* ************************************************************
                             * Aligned contact check to allow catching hole on the ceiling
                             * by thrown up NPCs. This makes inability to catch hole on the
                             * wall while flying up. But still be able catch hole on the wall
                             * while falling down.
                             * Ignore this code part when gravitation is directed to up
                             **************************************************************/
                            if(round(CUR->m_momentum.right()) - 1.0 < round(m_momentum.left()))
                            {
                                m_momentum.x = round(m_momentum.x);
                                goto tipRectB_Skip;
                            }

                            if(round(CUR->m_momentum.left()) + 1.0 > round(m_momentum.right()))
                            {
                                m_momentum.x = round(m_momentum.x);
                                goto tipRectB_Skip;
                            }

                            /* *************************************************************/
                            m_momentum.y = CUR->m_momentum.y + CUR->m_momentum.h;
                            m_momentum.velY = CUR->m_momentum.velY;
                            contactAt = PhysObject::Contact_Bottom;
                            doHit = true;
                            collideAtTop = CUR;
                            l_pushT(CUR);

                            if(m_slopeFloor.has)
                            {
                                if(findMinimalHeight(m_slopeFloor.shape, m_slopeFloor.rect, 0.0,
                                                     CUR->m_shape, CUR->m_momentum.rect(), this))
                                {
                                    doSpeedStack = false;
                                    m_momentum.velX = m_momentum.velXsrc;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }

tipRectB_Skip:
                            ;
                        }
                    }
                }
                else
                {
tipRectH://Check horizontal sides collision

                    if(m_momentum.centerX() < CUR->m_momentum.centerX())
                    {
                        //'left
                        if(isBlockLeftWall(CUR))
                        {
                            if(m_slopeCeiling.hasOld || m_slopeFloor.hasOld)
                            {
                                PhysObject::objRect &rF = m_slopeFloor.rect;
                                PhysObject::objRect &rC = m_slopeCeiling.rect;

                                if(Maths::equals(CUR->m_momentum.top(), rF.top())  &&
                                   (CUR->m_momentum.right() <= (rF.left() + 1.0)) &&
                                   (m_slopeFloor.shape == PhysObject::SL_LeftBottom))
                                    goto tipRectL_Skip;

                                if(Maths::equals(CUR->m_momentum.bottom(), rC.bottom()) &&
                                   (CUR->m_momentum.right() <= (rF.left() + 1.0)) &&
                                   (m_slopeCeiling.shape == PhysObject::SL_LeftTop))
                                    goto tipRectL_Skip;
                            }

tipRectL://Impacted at left (right of player)
                            {
                                if(m_allowHoleRuning)
                                {
                                    double add = 2.0 + ((CUR->m_momentum.velY < 0.0) ?
                                                        fabs(CUR->m_momentum.velY) : 0.0);

                                    if((m_momentum.bottom() < (CUR->m_momentum.top() + add)) &&
                                       (m_momentum.velY > CUR->m_momentum.velY) && (m_momentum.velX > 0.0) &&
                                       (std::fabs(m_momentum.velX) > std::fabs(m_momentum.velY)))
                                        goto tipRectT;
                                }

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) == 0)
                                    goto tipRectL_Skip;

                                if(CUR->m_bodytype == Body_DYNAMIC)
                                {
                                    CUR->m_blockedAtLeft = true;
                                    /*blockSkipI = i;
                                    i = blockSkipStartFrom;
                                    CUR = objs[i];
                                    blockSkipStartFrom = blockSkipI;
                                    blockSkip = true;*/
                                }
                                else
                                {
                                    m_momentum.x = CUR->m_momentum.x - m_momentum.w;
                                    double &splr = m_momentum.velX;
                                    double &sbox = CUR->m_momentum.velX;
                                    xSpeedWasReversed = splr <= sbox;
                                    splr = std::min(splr, sbox);
                                    m_momentum.velXsrc = splr;
                                }

                                doSpeedStack = false;
                                speedSum = 0.0;
                                speedNum = 0.0;
                                contactAt = PhysObject::Contact_Left;
                                collideAtRight = CUR;
                                l_pushR(CUR);
                                m_blockedAtRight = true;
                            }
tipRectL_Skip:
                            ;
                        }
                    }
                    else
                    {
                        //'right
                        if(isBlockRightWall(CUR))
                        {
                            if(m_slopeCeiling.hasOld || m_slopeFloor.hasOld)
                            {
                                objRect &rF = m_slopeFloor.rect;
                                objRect &rC = m_slopeCeiling.rect;

                                if(Maths::equals(CUR->m_momentum.top(), rF.top()) &&
                                   (CUR->m_momentum.left() >= (rF.right() - 1.0)) &&
                                   (m_slopeFloor.shape == PhysObject::SL_RightBottom))
                                    goto tipRectR_Skip;

                                if(Maths::equals(CUR->m_momentum.bottom(), rC.bottom()) &&
                                   (CUR->m_momentum.left() >= (rF.right() - 1.0)) &&
                                   (m_slopeCeiling.shape == PhysObject::SL_RightTop))
                                    goto tipRectR_Skip;
                            }

tipRectR://Impacted at right (left of player)
                            {
                                if(m_allowHoleRuning)
                                {
                                    double add = 2.0 + ((CUR->m_momentum.velY < 0.0) ?
                                                        fabs(CUR->m_momentum.velY) : 0.0);

                                    if((m_momentum.bottom() < (CUR->m_momentum.top() + add)) &&
                                       (m_momentum.velY > CUR->m_momentum.velY) && (m_momentum.velX < 0.0) &&
                                       (std::fabs(m_momentum.velX) > std::fabs(m_momentum.velY)))
                                        goto tipRectT;
                                }

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) == 0)
                                    goto tipRectR_Skip;

                                if(CUR->m_bodytype == Body_DYNAMIC)
                                {
                                    CUR->m_blockedAtRight = true;
                                    /*blockSkipI = i;
                                    i = blockSkipStartFrom;
                                    CUR = objs[i];
                                    blockSkipStartFrom = blockSkipI;
                                    blockSkip = true;*/
                                }
                                else
                                {
                                    m_momentum.x = CUR->m_momentum.x + CUR->m_momentum.w;
                                    double &splr = m_momentum.velX;
                                    double &sbox = CUR->m_momentum.velX;
                                    xSpeedWasReversed = splr >= sbox;
                                    splr = std::max(splr, sbox);
                                    m_momentum.velXsrc = splr;
                                }

                                doSpeedStack = false;
                                speedSum = 0.0;
                                speedNum = 0.0;
                                contactAt = PhysObject::Contact_Right;
                                collideAtLeft = CUR;
                                l_pushL(CUR);
                                m_blockedAtLeft = true;
                            }
tipRectR_Skip:
                            ;
                        }
                    }
                }

                if((m_stand) || (m_momentum.velXsrc == 0.0) || xSpeedWasReversed)
                    tm = -2;

tipTriangleShape://Check triangular collision

                if(contactAt == PhysObject::Contact_None)
                {
                    //Avoid infinite loop for case of skipped collision resolving
                    contactAt = PhysObject::Contact_Skipped;
                    k = CUR->m_momentum.h / CUR->m_momentum.w;
                    Maths::clearPrecision(k);

                    switch(CUR->m_shape)
                    {
                    case PhysObject::SL_LeftBottom:

                        /* *************** Resolve collision with corner on the top ********************************* */
                        if((m_momentum.left() <= CUR->m_momentum.left()) && (m_onSlopeFloorTopAlign || (m_momentum.velY >= 0.0)))
                        {
                            if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                               (m_momentum.bottom() > CUR->m_momentum.bottom()))
                                goto tipRectB;

                            if(CUR->m_blocked[m_filterID] == PhysObject::Block_ALL)
                            {
                                if((m_momentum.bottom() >= CUR->m_momentum.top()) &&
                                   ((m_momentum.left() < CUR->m_momentum.left()) || (m_momentum.velX <= 0.0)))
                                    goto tipRectT;
                            }
                            else
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                                   (m_momentum.bottom() >= CUR->m_momentum.top()) && (m_momentum.bottomOld() <= CUR->m_momentum.topOld()) &&
                                   ((m_momentum.left() < CUR->m_momentum.left()) || (m_momentum.velX <= 0.0)))
                                    goto tipRectT;
                            }
                        }
                        /* *************** Resolve collision with footer corner on right bottom side **************** */
                        else if((m_momentum.bottom() > CUR->m_momentum.bottom()) &&
                                (m_momentum.bottomOld() > CUR->m_momentum.bottomOld()) &&
                                (
                                    (!m_slopeFloor.hasOld) ||
                                    ((m_slopeFloor.shape != CUR->m_shape) && (m_slopeFloor.shape >= 0))
                                ))
                        {
                            if(!colH)
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                                   CUR->m_momentum.betweenH(m_momentum.left(), m_momentum.right()) && (m_momentum.velY >= 0.0))
                                    goto tipRectB;
                            }
                            else
                            {
                                if(m_momentum.centerX() < CUR->m_momentum.centerX())
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                                       (m_momentum.velX >= 0.0))
                                        goto tipRectL;
                                }
                                else
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                                       (m_momentum.velX <= 0.0))
                                        goto tipRectR;
                                }
                            }
                        }
                        /* ********************* Resolve collision with top slope surface *************************** */
                        else if(m_momentum.bottom() > CUR->m_momentum.y + ((m_momentum.left() - CUR->m_momentum.left()) * k) - 1.0)
                        {
                            if((CUR->m_blocked[m_filterID] & PhysObject::Block_TOP) == 0)
                                goto skipTriangleResolving;

                            if((CUR->m_blocked[m_filterID] != PhysObject::Block_ALL) && (!m_slopeFloor.hasOld))
                            {
                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) == 0)
                                    goto skipTriangleResolving;

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) == 0)
                                {
                                    if(m_momentum.velY < CUR->m_momentum.velY)
                                        goto skipTriangleResolving;

                                    if((m_momentum.bottomOld() > CUR->m_momentum.oldy + ((m_momentum.leftOld() - CUR->m_momentum.leftOld()) * k) - 0))
                                        goto skipTriangleResolving;
                                }
                            }

                            m_momentum.y = CUR->m_momentum.y + ((m_momentum.left() - CUR->m_momentum.left()) * k) - m_momentum.h;

                            if(CUR->m_momentum.velY <= m_momentum.velY)
                                m_momentum.velY = CUR->m_momentum.velY;

                            m_slopeFloor.has = true;
                            m_slopeFloor.shape = CUR->m_shape;
                            m_slopeFloor.rect  = CUR->m_momentum.rect();

                            if((m_momentum.velX > 0.0) || !m_onSlopeFloorTopAlign)
                            {
                                if(CUR->m_momentum.velY <= m_momentum.velY)
                                    m_momentum.velY = CUR->m_momentum.velY + (m_momentum.velX * k);

                                m_onSlopeYAdd = 0.0;
                            }
                            else
                            {
                                m_onSlopeYAdd = (m_momentum.velX * k);

                                if((m_onSlopeYAdd < 0.0) && (m_momentum.bottom() + m_onSlopeYAdd < CUR->m_momentum.y))
                                    m_onSlopeYAdd = -std::fabs(m_momentum.bottom() - CUR->m_momentum.y);
                            }

                            m_stand = true;
                            m_standOnYMovable = (CUR->m_momentum.velY != 0.0);

                            if(doSpeedStack)
                            {
                                m_momentum.velX = m_momentum.velXsrc + CUR->m_momentum.velX;
                                speedSum += CUR->m_momentum.velX;

                                if(CUR->m_momentum.velX != 0.0)
                                    speedNum += 1.0;
                            }

                            contactAt = PhysObject::Contact_Top;
                            doCliffCheck = true;
                            l_clifCheck.push_back(CUR);
                            l_pushB(CUR);

                            if(m_slopeCeiling.has)
                            {
                                if(findMinimalHeight(CUR->m_shape, CUR->m_momentum.rect(), CUR->m_momentum.velX,
                                                     m_slopeCeiling.shape, m_slopeCeiling.rect, this))
                                {
                                    doSpeedStack = false;
                                    m_momentum.velX = m_momentum.velXsrc;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }
                        }

                        break;

                    case PhysObject::SL_RightBottom:

                        /* *************** Resolve collision with corner on the right top ************************* */
                        if(m_momentum.right() >= CUR->m_momentum.right() && (m_onSlopeFloorTopAlign || (m_momentum.velY >= 0.0)))
                        {
                            if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                               (m_momentum.bottom() > CUR->m_momentum.bottom()))
                                goto tipRectB;

                            if(CUR->m_blocked[m_filterID] == PhysObject::Block_ALL)
                            {
                                if((m_momentum.bottom() >= CUR->m_momentum.top()) &&
                                   ((m_momentum.right() > CUR->m_momentum.right()) || (m_momentum.velX >= 0.0)))
                                    goto tipRectT;
                            }
                            else
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                                   (m_momentum.bottom() >= CUR->m_momentum.top()) && (m_momentum.bottomOld() <= CUR->m_momentum.topOld()) &&
                                   ((m_momentum.right() > CUR->m_momentum.right()) || (m_momentum.velX >= 0.0)))
                                    goto tipRectT;
                            }
                        }
                        /* ************** Resolve collision with footer corner on left bottom side **************** */
                        else if((m_momentum.bottom() > CUR->m_momentum.bottom()) &&
                                (m_momentum.bottomOld() > CUR->m_momentum.bottomOld()) &&
                                (
                                    (!m_slopeFloor.hasOld) ||
                                    ((m_slopeFloor.shape != CUR->m_shape) && (m_slopeFloor.shape >= 0))
                                ))
                        {
                            if(!colH)
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                                   CUR->m_momentum.betweenH(m_momentum.left(), m_momentum.right()) && (m_momentum.velY >= 0.0))
                                    goto tipRectB;
                            }
                            else
                            {
                                if(m_momentum.centerX() < CUR->m_momentum.centerX())
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                                       (m_momentum.velX >= 0.0))
                                        goto tipRectL;
                                }
                                else
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                                       (m_momentum.velX <= 0.0))
                                        goto tipRectR;
                                }
                            }
                        }
                        /* ********************* Resolve collision with top slope surface *************************** */
                        else if(m_momentum.bottom() > CUR->m_momentum.y + ((CUR->m_momentum.right() - m_momentum.right()) * k) - 1.0)
                        {
                            if((CUR->m_blocked[m_filterID] & PhysObject::Block_TOP) == 0)
                                goto skipTriangleResolving;

                            if((CUR->m_blocked[m_filterID] != PhysObject::Block_ALL) && (!m_slopeFloor.hasOld))
                            {
                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) == 0)
                                    goto skipTriangleResolving;

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) == 0)
                                {
                                    if(m_momentum.velY < CUR->m_momentum.velY)
                                        goto skipTriangleResolving;

                                    if((m_momentum.bottomOld() > CUR->m_momentum.oldy + ((CUR->m_momentum.rightOld() - m_momentum.rightOld()) * k) - 0))
                                        goto skipTriangleResolving;
                                }
                            }

                            m_momentum.y = CUR->m_momentum.y + ((CUR->m_momentum.right() - m_momentum.right()) * k) - m_momentum.h;

                            if(CUR->m_momentum.velY <= m_momentum.velY)
                                m_momentum.velY = CUR->m_momentum.velY;

                            m_slopeFloor.has = true;
                            m_slopeFloor.shape = CUR->m_shape;
                            m_slopeFloor.rect  = CUR->m_momentum.rect();

                            if((m_momentum.velX < 0.0) || !m_onSlopeFloorTopAlign)
                            {
                                if(CUR->m_momentum.velY <= m_momentum.velY)
                                    m_momentum.velY = CUR->m_momentum.velY - (m_momentum.velX * k);

                                m_onSlopeYAdd = 0.0;
                            }
                            else
                            {
                                m_onSlopeYAdd = -(m_momentum.velX * k);

                                if((m_onSlopeYAdd < 0.0) && (m_momentum.bottom() + m_onSlopeYAdd < CUR->m_momentum.y))
                                    m_onSlopeYAdd = -std::fabs(m_momentum.bottom() - CUR->m_momentum.y);
                            }

                            m_stand = true;
                            m_standOnYMovable = (CUR->m_momentum.velY != 0.0);

                            if(doSpeedStack)
                            {
                                m_momentum.velX = m_momentum.velXsrc + CUR->m_momentum.velX;
                                speedSum += CUR->m_momentum.velX;

                                if(CUR->m_momentum.velX != 0.0)
                                    speedNum += 1.0;
                            }

                            contactAt = PhysObject::Contact_Top;
                            doCliffCheck = true;
                            l_clifCheck.push_back(CUR);
                            l_pushB(CUR);

                            if(m_slopeCeiling.has)
                            {
                                if(findMinimalHeight(CUR->m_shape, CUR->m_momentum.rect(), CUR->m_momentum.velX,
                                                     m_slopeCeiling.shape, m_slopeCeiling.rect, this))
                                {
                                    //m_velX_source = 0.0;
                                    doSpeedStack = false;
                                    m_momentum.velX = m_momentum.velXsrc;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }
                        }

                        break;

                    case PhysObject::SL_LeftTop:

                        /* *************** Resolve collision with corner on the left bottom ************************* */
                        if(m_momentum.left() <= CUR->m_momentum.left())
                        {
                            if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                               (m_momentum.top() < CUR->m_momentum.top()))
                                goto tipRectT;

                            if(CUR->m_blocked[m_filterID] == PhysObject::Block_ALL)
                            {
                                if((m_momentum.top() <= CUR->m_momentum.bottom()) &&
                                   ((m_momentum.left() < CUR->m_momentum.left()) || (m_momentum.velX <= 0.0)))
                                    goto tipRectB;
                            }
                            else
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                                   (m_momentum.top() < CUR->m_momentum.bottom()) && (m_momentum.topOld() >= CUR->m_momentum.bottomOld()) &&
                                   ((m_momentum.left() < CUR->m_momentum.left()) || (m_momentum.velX <= 0.0)))
                                    goto tipRectB;
                            }
                        }
                        /* ****************** Resolve collision with upper corner on left top side ****************** */
                        else if((m_momentum.top() < CUR->m_momentum.top())  &&
                                ((!m_slopeCeiling.hasOld && (m_momentum.topOld() < CUR->m_momentum.topOld())) ||
                                 (m_slopeCeiling.shape != CUR->m_shape)))
                        {
                            if(!colH)
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                                   CUR->m_momentum.betweenH(m_momentum.left(), m_momentum.right()) && (m_momentum.velY <= 0.0))
                                    goto tipRectT;
                            }
                            else
                            {
                                if(m_momentum.centerX() < CUR->m_momentum.centerX())
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                                       (m_momentum.velX >= 0.0))
                                        goto tipRectL;
                                }
                                else
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                                       (m_momentum.velX <= 0.0))
                                        goto tipRectR;
                                }
                            }
                        }
                        /* ********************* Resolve collision with bottom slope surface *************************** */
                        else if(m_momentum.y < CUR->m_momentum.bottom() - ((m_momentum.x - CUR->m_momentum.x) * k))
                        {
                            if((CUR->m_blocked[m_filterID] != PhysObject::Block_ALL) && (!m_slopeCeiling.hasOld))
                            {
                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) == 0)
                                    goto skipTriangleResolving;

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) == 0)
                                {
                                    if(m_momentum.velY > CUR->m_momentum.velY)
                                        goto skipTriangleResolving;

                                    if(m_momentum.topOld() < CUR->m_momentum.bottomOld() - ((m_momentum.oldx - CUR->m_momentum.oldx) * k))
                                        goto skipTriangleResolving;
                                }
                            }

                            m_momentum.y    = CUR->m_momentum.bottom() - ((m_momentum.x - CUR->m_momentum.x) * k);

                            if(m_momentum.velX < 0.0)
                                m_momentum.velY = CUR->m_momentum.velY - m_momentum.velX * k;
                            else
                                m_momentum.velY = CUR->m_momentum.velY;

                            m_slopeCeiling.has = true;
                            m_slopeCeiling.shape = CUR->m_shape;
                            m_slopeCeiling.rect  = CUR->m_momentum.rect();
                            contactAt = PhysObject::Contact_Bottom;
                            doHit = true;
                            l_pushT(CUR);

                            if(m_slopeFloor.has)
                            {
                                if(findMinimalHeight(m_slopeFloor.shape, m_slopeFloor.rect, 0.0,
                                                     CUR->m_shape, CUR->m_momentum.rect(), this))
                                {
                                    //m_velX_source = 0.0;
                                    doSpeedStack = false;
                                    m_momentum.velX = m_momentum.velXsrc;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }
                        }

                        break;

                    case PhysObject::SL_RightTop:

                        /* *************** Resolve collision with corner on the right bottom ************************* */
                        if(m_momentum.right() >= CUR->m_momentum.right())
                        {
                            if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                               (m_momentum.top() < CUR->m_momentum.top()))
                                goto tipRectT;

                            if(CUR->m_blocked[m_filterID] == PhysObject::Block_ALL)
                            {
                                if((m_momentum.y < CUR->m_momentum.bottom()) &&
                                   ((m_momentum.right() > CUR->m_momentum.right()) || (m_momentum.velX >= 0.0)))
                                    goto tipRectB;
                            }
                            else
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                                   (m_momentum.top() < CUR->m_momentum.bottom()) && (m_momentum.topOld() >= CUR->m_momentum.bottomOld()) &&
                                   ((m_momentum.right() > CUR->m_momentum.right()) || (m_momentum.velX >= 0.0)))
                                    goto tipRectB;
                            }
                        }
                        /* ****************** Resolve collision with upper corner on right top side ****************** */
                        else if((m_momentum.top() < CUR->m_momentum.top())  &&
                                ((!m_slopeCeiling.hasOld && (m_momentum.topOld() < CUR->m_momentum.topOld())) ||
                                 (m_slopeCeiling.shape != CUR->m_shape)))
                        {
                            if(!colH)
                            {
                                if(((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0) &&
                                   CUR->m_momentum.betweenH(m_momentum.left(), m_momentum.right()) && (m_momentum.velY <= 0.0))
                                    goto tipRectT;
                            }
                            else
                            {
                                if(m_momentum.centerX() < CUR->m_momentum.centerX())
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                                       (m_momentum.velX >= 0.0))
                                        goto tipRectL;
                                }
                                else
                                {
                                    if(((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                                       (m_momentum.velX <= 0.0))
                                        goto tipRectR;
                                }
                            }
                        }
                        /* ********************* Resolve collision with bottom slope surface *************************** */
                        else if(m_momentum.y < CUR->m_momentum.bottom() - ((CUR->m_momentum.right() - m_momentum.x - m_momentum.w) * k))
                        {
                            if((CUR->m_blocked[m_filterID] != PhysObject::Block_ALL) && (!m_slopeCeiling.hasOld))
                            {
                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) == 0)
                                    goto skipTriangleResolving;

                                if((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) == 0)
                                {
                                    if(m_momentum.velY > CUR->m_momentum.velY)
                                        goto skipTriangleResolving;

                                    if(m_momentum.oldy < CUR->m_momentum.bottomOld() - ((CUR->m_momentum.rightOld() - m_momentum.oldx - m_momentum.oldw) * k))
                                        goto skipTriangleResolving;
                                }
                            }

                            m_momentum.y    = CUR->m_momentum.bottom() - ((CUR->m_momentum.right() - m_momentum.x - m_momentum.w) * k);

                            if(m_momentum.velX > 0.0)
                                m_momentum.velY = CUR->m_momentum.velY + m_momentum.velX * k ;
                            else
                                m_momentum.velY = CUR->m_momentum.velY;

                            m_slopeCeiling.has = true;
                            m_slopeCeiling.shape = CUR->m_shape;
                            m_slopeCeiling.rect  = CUR->m_momentum.rect();
                            contactAt = PhysObject::Contact_Bottom;
                            doHit = true;
                            l_pushT(CUR);

                            if(m_slopeFloor.has)
                            {
                                if(findMinimalHeight(m_slopeFloor.shape, m_slopeFloor.rect, 0.0,
                                                     CUR->m_shape, CUR->m_momentum.rect(), this))
                                {
                                    doSpeedStack = false;
                                    m_momentum.velX = m_momentum.velXsrc;
                                    speedSum = 0.0;
                                    speedNum = 0.0;
                                }
                            }
                        }

                        break;

                    default:
                        break;
                    }

                    /* ***** If slope block being detected, restart loop from begin and then skip this element *******
                     * this is VERY important, because just resolved ceiling or floor collision will be missed
                     * after resolving of the slope collision which makes glitch.
                     * To have accurate result need re-check collisions with all previous elements
                     * after resolving slope collision
                     * ***********************************************************************************************/
                    if((contactAt != PhysObject::Contact_None) && (contactAt != PhysObject::Contact_Skipped))
                    {
                        blockSkipI = i;
                        i = blockSkipStartFrom;
                        CUR = objs[i];
                        blockSkipStartFrom = blockSkipI;
                        blockSkip = true;
                    }
                }

skipTriangleResolving:
                ;
            }
            else
            {
                //If shape is not rectangle - check triangular collision
                if(CUR->m_shape != PhysObject::SL_Rect)
                    goto tipTriangleShape;

                //Catching 90-degree angle impacts of corners with rectangles
                if(!colH && !colV)
                {
                    if(tm == signed(i))
                    {
                        if(std::fabs(m_momentum.velY) > std::fabs(m_momentum.velX))
                            goto tipRectV;
                        else
                            goto tipRectH;
                    }
                    else
                    {
                        if(tm != -2)
                            tm = static_cast<int>(i);
                    }
                }
            }
        }

        /* ********************Find intere*********************************** */

        if(
            (CUR->m_shape == PhysObject::SL_Rect) &&
            figureTouch(m_momentum, CUR->m_momentum, 0.0, 0.0)
        )
            l_pushAny(CUR);

        /* ********************Find wall touch blocks************************ */
        if(figureTouch(m_momentum, CUR->m_momentum, 0.0, -1.0) &&
           !figureTouch(m_momentum, CUR->m_momentum, 0.0, 0.0))
        {
            if(m_momentum.betweenV(CUR->m_momentum.centerY()))
            {
                if((m_momentum.right() <= CUR->m_momentum.left()) &&
                   isBlockLeftWall(CUR->m_shape) &&
                   ((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                   (oldSpeedX >= CUR->m_momentum.velX))
                {
                    //CUR->m_touch = obj::Contact_Left;
                    m_touchRightWall = true;

                    if(!collideAtRight)
                        collideAtRight = CUR;
                }
                else if((m_momentum.left() >= CUR->m_momentum.right()) &&
                        isBlockRightWall(CUR->m_shape) &&
                        ((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                        (oldSpeedX <= CUR->m_momentum.velX))
                {
                    //CUR->m_touch = obj::Contact_Right;
                    m_touchLeftWall = true;

                    if(!collideAtLeft)
                        collideAtLeft = CUR;
                }
            }

            if(((contactAt == PhysObject::Contact_None) || (contactAt == PhysObject::Contact_Skipped)) && //Don't push duplicates
               m_momentum.betweenV(m_momentum.top(), m_momentum.bottom()))
            {
                if((m_momentum.right() <= CUR->m_momentum.left()) &&
                   isBlockLeftWall(CUR->m_shape) &&
                   ((CUR->m_blocked[m_filterID]&PhysObject::Block_LEFT) != 0) &&
                   (oldSpeedX >= CUR->m_momentum.velX) &&
                   m_moveRight)
                    l_pushR(CUR);
                else if((m_momentum.left() >= CUR->m_momentum.right()) &&
                        isBlockRightWall(CUR->m_shape) &&
                        ((CUR->m_blocked[m_filterID]&PhysObject::Block_RIGHT) != 0) &&
                        (oldSpeedX <= CUR->m_momentum.velX) &&
                        m_moveLeft)
                    l_pushL(CUR);
            }
        }

        /* ***********Find touching blocks (needed to process danger surfaces)*********** */
        if(((contactAt == PhysObject::Contact_None) || (contactAt == PhysObject::Contact_Skipped)) && //Don't push duplicates
           figureTouch(m_momentum, CUR->m_momentum, -1.0, 0.0) &&
           !figureTouch(m_momentum, CUR->m_momentum, 0.0, 0.0))
        {
            if(m_momentum.betweenH(m_momentum.left(), m_momentum.right()))
            {
                if((m_momentum.bottom() <= CUR->m_momentum.top()) &&
                   isBlockFloor(CUR->m_shape) &&
                   ((CUR->m_blocked[m_filterID]&PhysObject::Block_TOP) != 0))
                    l_pushB(CUR);
                else if((m_momentum.top() >= CUR->m_momentum.bottom()) &&
                        (isBlockCeiling(CUR->m_shape) /*||
                      ( (CUR->m_id == obj::SL_RightTop) && betweenH(CUR->left()) )||
                      ( (CUR->m_id == obj::SL_LeftTop) && betweenH(CUR->right()) )*/
                        ) &&
                        ((CUR->m_blocked[m_filterID]&PhysObject::Block_BOTTOM) != 0) &&
                        (oldSpeedY < CUR->m_momentum.velY))
                    l_pushT(CUR);
            }
        }

        if(td == 1)
        {
            tm = -1;
            break;
        }

        if((CUR->m_shape == PhysObject::SL_Rect) &&
           (CUR->m_blocked[m_filterID] == PhysObject::Block_ALL) &&
           recttouch(m_momentum.oldx,      m_momentum.oldy,        m_momentum.oldw,      m_momentum.oldh,
                     CUR->m_momentum.oldx, CUR->m_momentum.oldy,   CUR->m_momentum.oldw, CUR->m_momentum.oldh))
        {
            //l_possibleCrushers.push_back(CUR);
            m_crushed = true;
        }
    }//for(..)

    if(tm >= 0)
    {
        i = static_cast<PGE_SizeT>(tm);
        CUR = objs[i];
        if(dynamic_cast<PGE_Phys_Object*>(CUR) == dynamic_cast<PGE_Phys_Object*>(this))
            PGE_MsgBox::info("AHA! I ate you!");
        td = 1;
        goto tipRectShape;
    }

    /*
    if(m_crushed && m_crushedOld )
    {
        if(m_stand)
        {
            //HELP ME TO AVOID THIS CRAP!!!!
            doSpeedStack = false;
            m_momentum.velXsrc = 0.0;
            m_momentum.velX = 0.0;
            m_momentum.x += 8.0;
        }
    }*/

    /* ***********************Detect a cliff********************************** */
    if(doCliffCheck && !l_clifCheck.empty())
    {
        PhysObject *candidate = l_clifCheck[0];
        double lefter  = candidate->m_momentum.left();
        double righter = candidate->m_momentum.right();
        findHorizontalBoundaries(l_clifCheck, lefter, righter);

        if((m_momentum.velXsrc <= 0.0) && (lefter >= m_momentum.centerX()))
            m_cliff = true;

        if((m_momentum.velXsrc >= 0.0) && (righter <= m_momentum.centerX()))
            m_cliff = true;
    }
    else
    {
        if(!m_stand)
            l_clifCheck.clear();
    }

    /* *********************** Check all collided sides ***************************** */
    {
        auto it = l_contactL.begin();
        while(it != l_contactL.end())
        {
            PhysObject *cEL = *it;
            if(!cEL->m_momentum.betweenV(m_momentum.top() + 1.0, m_momentum.bottom() - 1.0))
                it = l_contactL.erase(it);
            else
                ++it;
        }
    }
    {
        auto it = l_contactR.begin();
        while(it != l_contactR.end())
        {
            PhysObject *cEL = *it;
            if(!cEL->m_momentum.betweenV(m_momentum.top() + 1.0, m_momentum.bottom() - 1.0))
                it = l_contactR.erase(it);
            else
                ++it;
        }
    }
    {
        auto it = l_contactT.begin();
        while(it != l_contactT.end())
        {
            PhysObject *cEL = *it;
            if(!cEL->m_momentum.betweenH(m_momentum.left() + 1.0, m_momentum.right() - 1.0))
                it = l_contactT.erase(it);
            else
                ++it;
        }
    }
    {
        auto it = l_contactB.begin();
        while(it != l_contactB.end())
        {
            PhysObject *cEL = *it;
            if(!cEL->m_momentum.betweenH(m_momentum.left() + 1.0, m_momentum.right() - 1.0))
                it = l_contactB.erase(it);
            else
                ++it;
        }
    }

    /* ****************************Detection of the crush****************************** */
    if(collideAtBottom && collideAtTop)
    {
        //If character got crushed between moving layers
        if((collideAtTop->m_bodytype == Body_STATIC) &&
           (collideAtBottom->m_momentum.velY < collideAtTop->m_momentum.velY))
        {
            m_stand = false;
            m_momentum.y = collideAtTop->m_momentum.bottom();
            m_momentum.velY = collideAtTop->m_momentum.velY;
            doSpeedStack = false;
            speedNum = 0.0;
            speedSum = 0.0;
#ifdef STOP_LOOP_ON_CRUSH
            alive = false;
#endif

            if((collideAtTop->m_shape == PhysObject::SL_RightTop) &&
               (round(m_momentum.right()) == round(collideAtTop->m_momentum.right())))
                m_momentum.x = collideAtTop->m_momentum.right() - m_momentum.w - 1.0;
            else if((collideAtTop->m_shape == PhysObject::SL_LeftTop) &&
                    (round(m_momentum.left()) == round(collideAtTop->m_momentum.left())))
                m_momentum.x = collideAtTop->m_momentum.left() + 1.0;
            else if((collideAtBottom->m_shape == PhysObject::SL_RightBottom) &&
                    (round(m_momentum.right()) == round(collideAtBottom->m_momentum.right())))
                m_momentum.x = collideAtBottom->m_momentum.right() - m_momentum.w - 1.0;
            else if((collideAtBottom->m_shape == PhysObject::SL_LeftBottom) &&
                    (round(m_momentum.left()) == round(collideAtBottom->m_momentum.left())))
                m_momentum.x = collideAtBottom->m_momentum.left() + 1.0;
            else if((collideAtTop->m_blocked[m_filterID] == PhysObject::Block_ALL) &&
                    (collideAtBottom->m_blocked[m_filterID] == PhysObject::Block_ALL) &&
                    (m_momentum.h > std::fabs(collideAtTop->m_momentum.bottom() - collideAtBottom->m_momentum.top())))
            {
                //m_crushedHardDelay = 30;
                m_crushedHard = true;
            }
        }
    }

    if(collideAtLeft && collideAtRight)
    {
        //If character got crushed between moving layers
        if((collideAtLeft->m_bodytype == Body_STATIC) &&
           (collideAtRight->m_bodytype == Body_STATIC) &&
           (collideAtRight->m_momentum.velX < collideAtLeft->m_momentum.velX))
        {
#ifdef STOP_LOOP_ON_CRUSH
            alive = false;
#endif

            if((collideAtLeft->m_blocked[m_filterID] == PhysObject::Block_ALL) &&
               (collideAtRight->m_blocked[m_filterID] == PhysObject::Block_ALL) &&
               (m_momentum.w > std::fabs(collideAtLeft->m_momentum.right() - collideAtRight->m_momentum.left())))
            {
                //m_crushedHardDelay = 30;
                m_crushedHard = true;
            }
        }
    }

    /* ***********************Hit a block********************************** */
    if(doHit && !l_toBump.empty())
        collisionHitBlockTop(l_toBump);

    /* ****************************************************************************** */
    if(m_bodytype != Body_STATIC) // Ignore speed stack if this is a static body
    {
        if(doSpeedStack && (speedNum > 1.0) && (speedSum != 0.0))
            m_momentum.velX = m_momentum.velXsrc + (speedSum / speedNum);
        m_treemap.updatePos();
    }

    Maths::clearPrecision(m_momentum.x);
    Maths::clearPrecision(m_momentum.y);
    Maths::clearPrecision(m_momentum.velX);
    Maths::clearPrecision(m_momentum.velY);
    Maths::clearPrecision(m_momentum.velXsrc);

    processContacts();
    postCollision();
    l_clifCheck.clear();
    l_toBump.clear();
}
