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

#include "bg_multilayer.h"
#include <algorithm>

void MultilayerBackground::init(const obj_BG &bg)
{
    if(!bg.setup.multi_layered || bg.setup.layers.empty())
        return;

    m_layers_back.clear();
    m_layers_front.clear();

    for(const BgSetup::BgLayer &layer : bg.setup.layers)
    {
        Layer l;
        l.setup = layer;
        l.autoscroll_x = layer.auto_scrolling_x;
        l.autoscroll_y = layer.auto_scrolling_y;

        // TODO: Implement here actual loading of texture and loading properties per every layer

        if(layer.z_index <= 0)      //Background layer
            m_layers_back.push_back(LayerPtr(new Layer(l)));
        else if(layer.z_index > 0)  //Foreground layer
            m_layers_front.push_back(LayerPtr(new Layer(l)));
    }

    m_scrollers.clear();
    initScrollers(m_layers_back);
    initScrollers(m_layers_front);

    m_isInitialized = true;
}

void MultilayerBackground::initScrollers(MultilayerBackground::LayersList &ls)
{
    struct LessThanZ
    {
        inline bool operator() (const LayerPtr& struct1, const LayerPtr& struct2)
        {
            return (struct1->setup.z_index < struct2->setup.z_index);
        }
    };

    //Sort by Z-index
    std::sort(ls.begin(),  ls.end(), LessThanZ());

    //Add pointers to every layer into the scroller
    for(LayerPtr &layer_ptr : ls)
    {
        Layer &layer = *layer_ptr;
        if(layer.autoscroll_x || layer.autoscroll_y)
        {
            Scroller s;
            s.layer = layer_ptr;
            s.speed_x = layer.autoscroll_x ? layer.setup.auto_scrolling_x_speed : 0.0;
            s.speed_y = layer.autoscroll_y ? layer.setup.auto_scrolling_y_speed : 0.0;
            m_scrollers.push_back(s);
        }
    }
}

void MultilayerBackground::process(double tickDelay)
{
    for(Scroller &scroller : m_scrollers)
    {
        Layer &layer = *scroller.layer;
        //Iterate scrolling
        layer.autoscroll_x_offset += scroller.speed_x * (tickDelay / 1000.0);
        layer.autoscroll_y_offset += scroller.speed_y * (tickDelay / 1000.0);

        //Return offset to initial position with small difference when it reaches width or height of layer image
        double w = layer.texture.w        + layer.setup.padding_x;
        double h = layer.texture.frame_h  + layer.setup.padding_y;

        if(layer.autoscroll_y_offset > h)
            layer.autoscroll_y_offset -= h;
        else if(layer.autoscroll_y_offset < 0.0)
            layer.autoscroll_y_offset += h;

        if(layer.autoscroll_x_offset > w)
            layer.autoscroll_x_offset -= w;
        else if(layer.autoscroll_x_offset < 0.0)
            layer.autoscroll_x_offset += w;
        //(scroller.layer->autoscroll_y_offset > scroller.layer->texture.frame_h)
    }
}

void MultilayerBackground::renderBackground(const PGE_RectF &box, double x, double y, double w, double h)
{
    renderLayersList(m_layers_back, box, x, y, w, h);
}

void MultilayerBackground::renderForeground(const PGE_RectF &box, double x, double y, double w, double h)
{
    renderLayersList(m_layers_front, box, x, y, w, h);
}

void MultilayerBackground::renderLayersList(const MultilayerBackground::LayersList &layers, const PGE_RectF &box, double x, double y, double w, double h)
{
    if(!m_isInitialized)
        return;

    // TODO: Implement here actual rendering

}
