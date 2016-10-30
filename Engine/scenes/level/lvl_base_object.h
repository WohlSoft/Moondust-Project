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

#include "base/lvl_base_physics.h"

#include <QHash>
#include <QVector>
#ifdef __APPLE__
#include <tgmath.h>
#endif

class LVL_Section;
class LevelScene;
class LVL_Block;
class PGE_LevelCamera;
///
/// \brief The PGE_Phys_Object class
///
/// Is a base of any physical objects
class PGE_Phys_Object: public PGE_physBody
{
        friend class PGE_LevelCamera;
        friend class LevelScene;
        //! Tells, does this object was catched by camera since recent render action
        bool         _vizible_on_screen;
        //! Tells, does this object stored into the render list
        bool         _render_list;
    public:
        inline bool isInRenderList()
        {
            return _render_list;
        }
    public:
        //! Pointer of the parent scene
        LevelScene  *_scene;
    protected:
        //! Is this object registered in the R-Tree?
        bool             _is_registered;
    public:
        PGE_Phys_Object(LevelScene *_parent = NULL);
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

        void setSize(double w, double h);
        void setWidth(double w);
        void setHeight(double h);

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

        inline double  gravityScale()
        {
            return phys_setup.gravityScale;
        }
        inline void    setGravityScale(double scl)
        {
            phys_setup.gravityScale = scl;
        }
        inline double  gravityAccel()
        {
            return phys_setup.gravityAccel;
        }
        inline void    setGravityAccel(double acl)
        {
            phys_setup.gravityAccel = fabs(acl);
        }

        inline double   maxVelX()
        {
            return phys_setup.max_vel_x;
        }
        inline void    setMaxVelX(double mv)
        {
            phys_setup.max_vel_x = mv;
        }
        inline double   maxVelY()
        {
            return phys_setup.max_vel_y;
        }
        inline void    setMaxVelY(double mv)
        {
            phys_setup.max_vel_y = mv;
        }
        inline double   minVelX()
        {
            return phys_setup.min_vel_x;
        }
        inline void    setMinVelX(double mv)
        {
            phys_setup.min_vel_x = mv;
        }
        inline double   minVelY()
        {
            return phys_setup.min_vel_y;
        }
        inline void    setMinVelY(double mv)
        {
            phys_setup.min_vel_y = mv;
        }

        void _syncPosition();
        void _syncPositionAndSize();
        void _syncSection(bool sync_position = true);
        void renderDebug(double _camX, double _camY);

        void iterateStep(double ticks, bool force = false);
        void iterateStepPostCollide(float ticks);
        virtual void processContacts() {}
        virtual void preCollision() {}
        virtual void postCollision() {}
        virtual void collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
        {
            Q_UNUSED(blocksHit);
        }
        /**
         * @brief Function called before collision resolving process. Allows filter some collisions
         * @param body Physical body
         * @return true if collision must be skipped, false - if need to check it
         */
        virtual bool preCollisionCheck(PGE_Phys_Object *body)
        {
            Q_UNUSED(body);
            return false;
        }

        void updateCollisions();
        virtual bool onGround()
        {
            return m_stand;
        }

        static const double m_smbxTickTime;
        static double SMBXTicksToTime(double ticks);

        PGE_Phys_Object_Phys phys_setup;//!< Settings of physics
        double m_accelX; //!<Delta of X velocity in a second
        double m_accelY; //!<Delta of Y velocity in a second

        double m_posX_registered; //!< Synchronized with R-Tree position
        double m_posY_registered; //!< Synchronized with R-Tree position

        double m_width_registered;  //!< Synchronized with R-Tree Width
        double m_height_registered; //!< Synchronized with R-Tree Height
        double m_width_half;//!< Half of width
        double m_height_half;//!< Half of height

        double m_width_toRegister;  //!< Width prepared to synchronize with R-Tree
        double m_height_toRegister; //!< Height prepared to synchronize with R-Tree

        void setParentSection(LVL_Section *sct);
        LVL_Section *sct();
        LVL_Section *_parentSection;

        int type;

        //! FILTERS
        bool m_slippery_surface;

        PGE_Texture texture;
        GLuint texId;
        virtual long double zIndex();
        long double z_index;

        /**************************Layer member****************************/
        virtual void show();
        virtual void hide();
        virtual void setVisible(bool vizible);
        virtual bool isVisible();
        bool m_is_visible;
        /******************************************************************/

    public:

        enum types
        {
            LVLUnknown = 0,
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
        virtual void update(double ticks);
        virtual void render(double x, double y)
        {
            Q_UNUSED(x);
            Q_UNUSED(y);
        }

        inline bool isPaused()
        {
            return m_paused;
        }
        inline void setPaused(bool p)
        {
            m_paused = p;
        }
    private:
        bool m_paused;

};



bool operator< (const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs);
bool operator> (const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs);

#endif // BASE_OBJECT_H
