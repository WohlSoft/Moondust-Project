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

#ifndef BG_MULTILAYER_H
#define BG_MULTILAYER_H

#include "bg_base.h"

class MultilayerBackground : public LevelBackgroundBase
{
public:
    MultilayerBackground() = default;
    MultilayerBackground(const MultilayerBackground &) = default;
    ~MultilayerBackground() = default;
    void init(const obj_BG &bg);
    void process(double tickDelay);
    void renderBackground(const PGE_RectF &box, double x, double y, double w, double h);
    void renderForeground(const PGE_RectF &box, double x, double y, double w, double h);
private:

    bool m_isInitialized = false;

    struct Layer
    {
        BgSetup::BgLayer    setup;
        PGE_Texture         texture;
        bool    autoscroll_x = false;
        double  autoscroll_x_offset = 0.0;
        bool    autoscroll_y = false;
        double  autoscroll_y_offset = 0.0;

        inline bool operator < (const Layer& layer) const
        {
            return (setup.z_index < layer.setup.z_index);
        }

        inline bool operator > (const Layer& layer) const
        {
            return (setup.z_index > layer.setup.z_index);
        }
    };

    struct Scroller
    {
        Layer *layer = nullptr;
        double speed_x = 0.0;
        double speed_y = 0.0;
    };

    typedef std::vector<Layer> LayersList;
    //! Back layers, must be sorted by Z index
    LayersList m_layers_back;
    //! Front layers, must be sorted by Z index
    LayersList m_layers_front;

    typedef std::vector<Scroller> ScrollersList;
    //! Active auto-scrollers (one auto-scroller per layer)
    ScrollersList m_scrollers;

    void initScrollers(LayersList &ls);

    void renderLayersList(const LayersList &layers, const PGE_RectF &box, double x, double y, double w, double h);
};

#endif // BG_MULTILAYER_H
