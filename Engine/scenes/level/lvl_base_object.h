/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <vector>
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
        struct metaCamera{
            //! Tells, does this object was catched by camera since recent render action
            bool         isVizibleOnScreen = false;
            //! Tells, does this object stored into the render list
            bool         isInIenderList = false;
        } m_camera_meta;

        //! Tells, does this object was catched by camera since recent render action
        bool         m_isVizibleOnScreen;
        //! Tells, does this object stored into the render list
        bool         m_isInIenderList;
    public:
        inline bool isInRenderList()
        {
            return m_camera_meta.isInIenderList;
        }
    public:
        //! Pointer of the parent scene
        LevelScene  *m_scene;
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
        virtual double posXrelative();
        /*!
         * \brief Position Y at left-top corner
         * \return Position Y at left-top corner
         */
        virtual double posY();
        virtual double posYrelative();
        /*!
         * \brief Position X at center
         * \return Position X at center
         */
        double posCenterX();
        double posCenterXrelative();
        /*!
         * \brief Position Y at center
         * \return Position Y at center
         */
        double posCenterY();
        double posCenterYrelative();
        /*!
         * \brief width of body
         * \return width of body
         */
        double width();
        double widthRelative();
        /*!
         * \brief height of body
         * \return height of body
         */
        double height();
        double heightRelative();

        double top();
        double topRelative();
        void setTop(double tp);
        void setTopRelative(double tp);
        double bottom();
        double bottomRelative();
        void setBottom(double btm);
        void setBottomRelative(double btm);
        double left();
        double leftRelative();
        void setLeft(double lf);
        void setLeftRelative(double lf);
        double right();
        double rightRelative();
        void setRight(double rt);
        void setRightRelative(double rt);

        void setSize(double w, double h);
        void setWidth(double w);
        void setHeight(double h);

        double  luaPosX();
        double  luaPosY();
        double  luaWidth();
        double  luaHeight();
        double  luaPosCenterX();
        double  luaPosCenterY();
        double  luaLeft();
        double  luaTop();
        double  luaRight();
        double  luaBottom();

        void    luaSetPosX(double x);
        void    luaSetPosY(double y);
        void    luaSetWidth(double w);
        void    luaSetHeight(double h);
        void    luaSetPosCenterX(double x);
        void    luaSetPosCenterY(double y);
        void    luaSetPos(double x, double y);
        void    luaSetCenterPos(double x, double y);
        void    luaSetCenterX(double x);
        void    luaSetCenterY(double y);
        void    luaSetSize(double w, double h);
        void    luaSetLeft(double l);
        void    luaSetTop(double t);
        void    luaSetRight(double r);
        void    luaSetBottom(double b);

        virtual void setPos(double x, double y);
        virtual void setRelativePos(double x, double y);
        void setPosX(double x);
        void setPosXrelative(double x);
        void setPosY(double y);
        void setPosYrelative(double y);
        void setCenterPos(double x, double y);
        void setRelativeCenterPos(double x, double y);
        void setCenterX(double x);
        void setCenterXrelative(double x);
        void setCenterY(double y);
        void setCenterYrelative(double y);

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

        //void _syncPosition();
        //void _syncPositionAndSize();
        void _syncSection(bool sync_position = true);
        void renderDebug(double _camX, double _camY);

        virtual void iterateStep(double ticks, bool force = false);
        void iterateStepPostCollide(float ticks);
        virtual void processContacts() {}
        virtual void preCollision() {}
        virtual void postCollision() {}
        virtual void collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
        {
            (void)(blocksHit);
        }
        /**
         * @brief Function called before collision resolving process. Allows filter some collisions
         * @param body Physical body
         * @return true if collision must be skipped, false - if need to check it
         */
        virtual bool preCollisionCheck(PGE_Phys_Object *body)
        {
            (void)(body);
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

        void setParentSection(LVL_Section *sct);
        LVL_Section *sct();
        LVL_Section *m_parentSection;

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
        bool                m_is_visible;
        struct TreeMapMember
        {
            TreeMapMember(PGE_Phys_Object *self) : m_self(self) {}
            PGE_Phys_Object *m_self = nullptr;
            void addToScene(bool keepSamePos = true);
            void updatePos();
            void updatePosAndSize();
            void updateSize();
            void delFromScene();
            bool   m_is_registered = false;
            double m_posX_registered = 0.0; //!< Synchronized with R-Tree position
            double m_posY_registered = 0.0; //!< Synchronized with R-Tree position
            double m_width_registered = 1.0;  //!< Synchronized with R-Tree Width
            double m_height_registered = 1.0; //!< Synchronized with R-Tree Height
        } m_treemap;
        Momentum            m_momentum_relative;//Momentum, relative to parent layer's position
        PGE_Phys_Object     *m_parent = nullptr;
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
            LVLPhysEnv,
            LVLSubTree
        };

        virtual void update();
        virtual void update(double ticks);
        virtual void render(double x, double y)
        {
            (void)(x);
            (void)(y);
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
