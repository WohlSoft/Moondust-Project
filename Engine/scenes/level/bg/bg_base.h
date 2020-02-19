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

#ifndef BG_BASE_H
#define BG_BASE_H

#include <data_configs/obj_bg.h>
#include <common_features/rectf.h>

class LevelScene;

/*!
 * \brief Generic level background processor
 */
class LevelBackgroundBase
{
public:
    /*!
     * \brief Captain Obvious sayd: It's descructor!
     */
    virtual ~LevelBackgroundBase();

    /*!
     * \brief Initialize background with given setup
     * \param bg Background config
     */
    virtual void init(const obj_BG &bg) = 0;

    /*!
     * \brief Give the level scene pointer to the background processor (to allow in-scene rendering)
     * \param scene Pointer to level scene
     */
    virtual void setScene(LevelScene *scene) = 0;

    /*!
     * \brief Process internal animation (scrolling, etc.) if available
     * \param frameDelay (Delay of one frame in milliseconds)
     */
    virtual void process(double frameDelay) = 0;

    /*!
     * \brief Draw background on the screen
     * \param box Current section box
     * \param x X position on the screen
     * \param y Y position on the screen
     * \param w Width of screen
     * \param h Heigh of screen
     */
    virtual void renderBackground(const PGE_RectF &box, double x, double y, double w, double h) = 0;

    /*!
     * \brief Draw in-scene background on the screen
     * \param box Current section box
     * \param x X position on the screen
     * \param y Y position on the screen
     * \param w Width of screen
     * \param h Heigh of screen
     *
     * Instead of directly draw, this function puts prepared to draw image pieces to the special queue.
     * You must call it once per PHYSICAL step and don't call it in render() function or you have a risk
     * to overflow memory while game pause is turned on!
     */
    virtual void renderInScene(const PGE_RectF &box, double x, double y, double w, double h) = 0;

    /*!
     * \brief Draw foreground on the screen
     * \param box Current section box
     * \param x X position on the screen
     * \param y Y position on the screen
     * \param w Width of screen
     * \param h Heigh of screen
     */
    virtual void renderForeground(const PGE_RectF &box, double x, double y, double w, double h) = 0;
};

#endif // BG_BASE_H
