/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GRAPHICS_LVL_BACKGRND_H
#define GRAPHICS_LVL_BACKGRND_H

#include <memory>

#include <data_configs/obj_bg.h>

#include <common_features/pge_texture.h>
#include <common_features/rectf.h>

#include "lvl_camera.h"
#include "bg/bg_base.h"

class PGE_LevelCamera;
class LevelScene;

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
     * \param scene Pointer to actual level scene (needed to be able draw in-scene backgrounds)
     */
    void setBg(obj_BG &bg, LevelScene *scene = nullptr);

    /*!
     * \brief Set blank black background
     */
    void setBlank();

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
    void drawBack(double x, double y, double w, double h);

    /*!
     * \brief Draw in-scene background layers on the screen.
     * \param x X in-scene position of the camera
     * \param y Y in-scene position of the camera
     * \param w Width of the camera
     * \param h Height of the camera
     *
     * Instead of directly draw, this function puts prepared to draw image pieces to the special queue.
     * Don't call it in render() function or you have a risk to overflow memory!
     */
    void drawInScene(double x, double y, double w, double h);

    /*!
     * \brief Draw foreground on the screen
     * \param x X in-scene position of the camera
     * \param y Y in-scene position of the camera
     * \param w Width of the camera
     * \param h Height of the camera
     */
    void drawFront(double x, double y, double w, double h);

    /*!
     * \brief Process internal algorithms of each background type if available
     * \param frameDelay Frame delay in milliseconds
     */
    void process(double frameDelay);

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
    uint64_t    m_bgId = 0;
    obj_BG      *m_setup = nullptr;
    bool        m_blankBackground = true;
    PGEColor    m_color;
    PGE_RectF   m_box;
    bool        m_isInitialized = false;
    std::unique_ptr<LevelBackgroundBase> m_bg_base;

    void construct();
};


#endif // GRAPHICS_LVL_BACKGRND_H
