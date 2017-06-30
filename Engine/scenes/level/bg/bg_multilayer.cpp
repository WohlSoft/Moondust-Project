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
#include <data_configs/config_manager.h>
#include <common_features/logger.h>
#include <graphics/gl_renderer.h>
#include <Utils/maths.h>

#include <algorithm>

void MultilayerBackground::init(const obj_BG &bg)
{
    if(!bg.setup.multi_layered || bg.setup.layers.empty())
        return;

    m_layers_back.clear();
    m_layers_front.clear();

    uint32_t layerIdCounter = 0;
    for(const BgSetup::BgLayer &layer : bg.setup.layers)
    {
        Layer l;
        l.setup = layer;
        l.autoscroll_x = layer.auto_scrolling_x;
        l.autoscroll_y = layer.auto_scrolling_y;
        l.layerId = layerIdCounter++;

        int tID = ConfigManager::getBGLayerTexture(bg.setup.id, l.layerId);
        if(tID >= 0)
        {
            l.texture = ConfigManager::level_textures[tID];
            l.tID = bg.texturePerLayer[l.layerId];
        }
        else
        {
            pLogWarning("Fail to load texture for background layer: %s", l.setup.image.c_str());
            break;
        }

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

    // TODO: Implement support for all INI values for multi-layer backgrounds:
    /*
     * padding - whitespace between every repeating layer
     * offset  - position offset of referrence point
     * auto-parallax - automatically calculate parallax coefficients per every layer in dependence on Z-Value
     * auto-scrolling - increase/decreaze offset value together with a time
     */

    for(const LayerPtr &layer_ptr : layers)
    {
        Layer   &layer = *layer_ptr;
        int     sHeightI  = Maths::iRound(box.height());
        double  sHeight   = box.height();
        double  sWidth    = box.width();
        const double fWidth    = static_cast<double>(layer.texture.frame_w) + layer.setup.padding_x;
        const double fHeight   = static_cast<double>(layer.texture.frame_h) + layer.setup.padding_y;
        double  imgPos_X  = 0;
        double  imgPos_Y  = 0;

        switch(layer.setup.parallax_mode_x)
        {
        // Proportionally move sprite with camera's position inside section
        case BgSetup::BgLayer::P_MODE_FIT:
            if(fWidth < w)//If image width less than screen
                imgPos_X = 0.0;
            else if(sWidth > fWidth)
                imgPos_X = (box.left() - x) / ((sWidth - w) / (fWidth - w));
            else
                imgPos_X = box.left() - x;
            break;

        // Scroll backround with divided offset at reference point edge
        case BgSetup::BgLayer::P_MODE_SCROLL:
            imgPos_X = std::fmod((box.left() - x) / layer.setup.parallax_coefficient_x, fWidth);
            break;

        // Fixed position
        case BgSetup::BgLayer::P_MODE_FIXED:
            imgPos_X = 0.0;
            break;
        }

        switch(layer.setup.parallax_mode_y)
        {
        // Proportionally move sprite with camera's position inside section
        case BgSetup::BgLayer::P_MODE_FIT:
            if(fHeight < h) //If image height less than screen
                imgPos_Y = (layer.setup.reference_point_y == BgSetup::BgLayer::R_TOP) ? 0.0 : (h - fHeight);
            else if(sHeight > fHeight)
                imgPos_Y = (box.top() - y) / ((sHeight - h) / (layer.texture.frame_h - h));
            else if(sHeightI == layer.texture.frame_h)
                imgPos_Y = box.top() - y;
            else
                imgPos_Y = (layer.setup.reference_point_y == BgSetup::BgLayer::R_TOP) ?
                           (box.top() - y) :
                           (box.bottom() - y - fHeight);
            break;

        // Scroll backround with divided offset at reference point edge
        case BgSetup::BgLayer::P_MODE_SCROLL:
            imgPos_Y = (layer.setup.reference_point_y == BgSetup::BgLayer::R_TOP) ?
                       std::fmod((box.top() - y) / 2.0, fHeight) :
                       std::fmod((box.bottom() + h - y) / 2.0, fHeight);
            break;

        // Fixed position
        case BgSetup::BgLayer::P_MODE_FIXED:
            imgPos_Y = (layer.setup.reference_point_y == BgSetup::BgLayer::R_TOP) ?
                       (box.top() - y) :
                       (box.bottom() - y - fHeight);
            break;
        }

        if(layer.setup.parallax_coefficient_x > 0.0)
            imgPos_X = std::fmod((box.left() - x) / layer.setup.parallax_coefficient_x, fWidth);
        else
        {
            if(fWidth < w) //If image width less than screen
                imgPos_X = 0.0;
            else if(sWidth > fWidth)
                imgPos_X = (box.left() - x) / ((sWidth - w) / (fWidth - w));
            else
                imgPos_X = box.left() - x;
        }


        AniPos ani_x(0, 1);
        if(layer.setup.animated) //Get current animated frame
            ani_x = ConfigManager::Animator_BG[static_cast<int>(layer.tID.animatorId)].image();
        double lenght = 0;
        double lenght_v = 0;
        //for vertical repeats
        int verticalRepeats = 1;

        if(layer.setup.repeat_y)
        {
            verticalRepeats = 0;
            lenght_v -= fHeight;
            imgPos_Y -= fHeight * ((layer.setup.reference_point_y == BgSetup::BgLayer::R_BOTTOM) ? -1.0 : 1.0);
            while(lenght_v <= h * 2.0  || (lenght_v <= fHeight * 2))
            {
                verticalRepeats++;
                lenght_v += fHeight + layer.setup.padding_y;
            }
        }

        double draw_x = imgPos_X;
        PGE_RectF m_backgrndG;

        while(verticalRepeats > 0)
        {
            draw_x = imgPos_X;
            lenght = 0;

            while((lenght <= w * 2) || (lenght <= layer.texture.frame_w * 2))
            {
                m_backgrndG.setRect(draw_x, imgPos_Y, layer.texture.frame_w, layer.texture.frame_h);
                double d_top    = ani_x.first;
                double d_bottom = ani_x.second;

                GlRenderer::renderTexture(&layer.texture,
                                          static_cast<float>(m_backgrndG.left()),
                                          static_cast<float>(m_backgrndG.top()),
                                          static_cast<float>(m_backgrndG.width()),
                                          static_cast<float>(m_backgrndG.height()),
                                          static_cast<float>(d_top),
                                          static_cast<float>(d_bottom));

                lenght += fWidth;
                draw_x += fWidth;
            }

            verticalRepeats--;
            if(verticalRepeats > 0)
                imgPos_Y += fHeight * ((layer.setup.reference_point_y == BgSetup::BgLayer::R_BOTTOM) ? -1 : 1);
        }
    }
}
