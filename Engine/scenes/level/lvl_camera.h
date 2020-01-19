/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_CAMERA_H
#define LVL_CAMERA_H

#include "lvl_base_object.h"
#include <graphics/graphics.h>
#include <common_features/fader.h>
#include <common_features/rectf.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

#include <vector>
#include <PGE_File_Formats/file_formats.h>
#include <queue>

class   PGE_Phys_Object;
typedef std::vector<PGE_Phys_Object *>  PGE_RenderList;
typedef size_t                          PGE_SizeT;

class LVL_Background;
class LVL_Section;

class PGE_LevelCamera
{
        friend class LVL_Background;
        friend class LVL_Section;
        friend class LevelScene;

        //! List of NPC's which needs an activation
        std::queue<PGE_Phys_Object *> npcs_to_activate;
        /*!
         * \brief QuadTree search callback function which accepts found elements into cache
         * \param item found element
         * \param arg pointer to this camera
         * \return always true
         */
        static bool _TreeSearchCallback(PGE_Phys_Object *item, void *arg);
        /*!
         * \brief Collects elements from vizible area
         * \param zone Area range where look for elements
         */
        void queryItems(PGE_RectF &zone);

        //! Pointer of the parent scene
        LevelScene  *_scene;
    public:
        //! Time of one tick in the SMBX Engine (is 1/65)
        static const double _smbxTickTime;
        /*!
         * \brief Constructor
         */
        PGE_LevelCamera(LevelScene *_parent);
        /*!
         * \brief Copy constructor
         * \param cam another camera object
         */
        PGE_LevelCamera(const PGE_LevelCamera &cam);
        /*!
         * \brief Destructor
         */
        ~PGE_LevelCamera();
        /*!
         * \brief Returns width of vizible camera area
         * \return width of vizible camera area
         */
        int w();
        /*!
         * \brief Returns height of vizible camera area
         * \return height of vizible camera area
         */
        int h();
        /*!
         * \brief Returns X position of camera vizible area
         * \return X position of camera vizible area
         */
        double posX();
        /*!
         * \brief Returns Y position of camera vizible area
         * \return Y position of camera vizible area
         */
        double posY();

        double renderX();

        double renderY();

        double centerX();

        double centerY();

        void init(double x, double y, double w, double h);

        void setPos(double x, double y);
        void setRenderPos(double x, double y);
        void setCenterPos(double x, double y);
        void setSize(int w, int h);
        void setOffset(int x, int y);
        void updatePre(double frameDelay);
        void updatePost(double frameDelay);
        void drawBackground();
        void drawForeground();

        void changeSection(LVL_Section *sct, bool isInit = false);
        void changeSectionBorders(long left, long top, long right, long bottom);
        void resetLimits();

        //PGE_RenderList &renderObjects();
        PGE_Phys_Object **renderObjects_arr();
        int              renderObjects_max();
        int              renderObjects_count();
        void             setRenderObjects_count(int count);
        void             setRenderObjectsCacheEnabled(bool enabled);

        LevelSection *section;
        LVL_Section *cur_section;

        PGE_RectF posRect;

        //! Recent center X position gotten from playable character
        double focus_x;
        //! Recent center Y position gotten from playable character
        double focus_y;

        double render_x;
        double render_y;

        double offset_x;
        double offset_y;

        //! ID of player who owns this camera
        int playerID;

        /**************Fader**************/
        PGE_Fader fader;
        /**************Fader**************/

        /**************Autoscrool**************/
        struct AutoScrooler
        {
            AutoScrooler() :
                camera(nullptr),
                enabled(false),
                velXmax(0.0),
                velYmax(0.0),
                velX(0.0),
                velY(0.0)
            {}
            void resetAutoscroll();
            void processAutoscroll(double tickTime);
            PGE_LevelCamera *camera;
            bool  enabled;
            double velXmax;
            double velYmax;
            double velX;
            double velY;
        } m_autoScrool;
        PGE_RectF limitBox;
        /**************Autoscrool**************/

        /**************LUA Binding*************/
        static luabind::scope bindToLua();
        /**************************************/
        /************Screen shaking************/
        /*!
         * \brief Enables screen shaking
         * \param force Max pixels delay at center position to shake
         * \param dec_step Force deceleration step per millisecond
         */
        void shakeScreen(double forceX, double forceY, double dec_step_x, double dec_step_y);
        void shakeScreenX(double forceX, double dec_step_x);
        void shakeScreenY(double forceY, double dec_step_y);
        bool shake_enabled_x;
        bool shake_enabled_y;
        double shake_force_decelerate_x;
        double shake_force_decelerate_y;
        double shake_force_x;
        double shake_force_y;
        /**************************************/

    private:
        void _applyLimits();
        void sortElements();
        //PGE_RenderList     objects_to_render;
        PGE_Phys_Object **_objects_to_render;
        int               _objects_to_render_max;
        int               _objects_to_render_stored;
        int               _objects_to_render_recent;
        bool              _disable_cache_mode;
};

#endif // LVL_CAMERA_H
