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

#ifndef BG_STANDARD_H
#define BG_STANDARD_H

#include "bg_base.h"

class StandardBackground : public LevelBackgroundBase
{
public:
    StandardBackground() = default;
    StandardBackground(const StandardBackground &) = default;
    ~StandardBackground() = default;
    void init(const obj_BG &bg);
    void setScene(LevelScene *scene);
    void process(double tickDelay);
    void renderBackground(const PGE_RectF &box, double x, double y, double w, double h);
    void renderInScene(const PGE_RectF &box, double x, double y, double w, double h);
    void renderForeground(const PGE_RectF &box, double x, double y, double w, double h);

private:
    BgSetup::BgType         m_bgType            = BgSetup::BG_TYPE_SingleRow;
    double                  m_repeat_h          = 2.0;
    double                  m_second_repeat_h   = 4.0;
    uint32_t                m_repeat_v          = 0;
    BgSetup::BgAttach       m_attached          = BgSetup::BG_ATTACH_TO_BOTTOM;
    BgSetup::BgAttachSecond m_second_attached   = BgSetup::BG_ATTACH_2_TO_TOP_OF_FIRST;

    PGE_Texture m_txData1;
    PGE_Texture m_txData2;

    bool m_isAnimated   = false;
    long m_animator_ID  = -1;

    /*!
     * \brief Magic background strip value
     */
    struct LVL_Background_strip
    {
        double      repeat_h;
        double      top;
        double      bottom;
        uint32_t    height;
    };

    bool m_isSegmented = false;
    std::vector<LVL_Background_strip> m_strips;

    PGEColor m_color;

    //! A buffer used in draw process
    PGE_RectF m_backgrndG;
};

#endif // BG_STANDARD_H
