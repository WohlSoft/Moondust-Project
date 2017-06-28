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

#ifndef GRAPHICS_LVL_BACKGRND_H
#define GRAPHICS_LVL_BACKGRND_H

#include <memory>

#include <data_configs/obj_bg.h>

#include <common_features/pge_texture.h>
#include <common_features/rectf.h>

#include "lvl_camera.h"
#include "bg/bg_base.h"

class PGE_LevelCamera;

class LVL_Background
{
    friend class PGE_LevelCamera;
public:
    LVL_Background() = default;
    LVL_Background(const LVL_Background &_bg);
    ~LVL_Background() = default;

    /*!
     * \brief Load background with attempt to load custom background settings
     * \param bg Referrence to background config
     */
    void setBg(obj_BG &bg);

    /*!
     * \brief Set blank black background
     */
    void setNone();

    /*!
     * \brief Set the section box
     * \param _box Area of the section
     */
    void setBox(const PGE_RectF &box);

    /*!
     * \brief Draw background on the screen
     * \param x X in-scene position of the camera
     * \param y Y in-scene position of the camera
     * \param w Width of the camera
     * \param h Height of the camera
     */
    void draw(double x, double y, double w, double h);

    /*!
     * \brief Is background initialized
     * \return true if background is initialized
     */
    bool isInit();

    /*!
     * \brief Get ID of currently initialized background image
     * \return ID of background image in use
     */
    unsigned long curBgId();

private:
    obj_BG      m_setup;
    bool        m_blankBackground = true;
    PGEColor    m_color;
    PGE_RectF   m_box;
    bool        m_isInitialized = false;
    std::unique_ptr<LevelBackgroundBase> m_bg_base;

    void construct();
};


#endif // GRAPHICS_LVL_BACKGRND_H
