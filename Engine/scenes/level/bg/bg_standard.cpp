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

#include "bg_standard.h"
#include <data_configs/config_manager.h>
#include <common_features/logger.h>
#include <Utils/maths.h>
#include <graphics/gl_renderer.h>

void StandardBackground::init(const obj_BG &bg)
{
    m_bgType            = static_cast<BgSetup::BgType>(bg.setup.type);
    m_repeat_h          = bg.setup.repeat_h;
    m_second_repeat_h   = bg.setup.second_repeat_h;
    m_repeat_v          = bg.setup.repeat_v;
    m_attached          = static_cast<BgSetup::BgAttach>(bg.setup.attached);
    m_second_attached   = static_cast<BgSetup::BgAttachSecond>(bg.setup.second_attached);

    pLogDebug("BG Type %d", m_bgType);

    //Reset magic background parameters
    m_isSegmented = false;
    m_strips.clear();

    switch(m_bgType)
    {
    case BgSetup::BG_TYPE_SingleRow:
    case BgSetup::BG_TYPE_Tiled:
    {
        int tID = ConfigManager::getBGTexture(bg.setup.id);
        if(tID >= 0)
        {
            m_txData1 = ConfigManager::level_textures[tID];

            if(bg.setup.attached == BgSetup::BG_ATTACH_TO_TOP)
                m_color = bg.Color_lower;
            else
                m_color = bg.Color_upper;

            m_isAnimated = bg.setup.animated;
            m_animator_ID = bg.animator_ID;

            if(bg.setup.segmented)
            {
                for(uint32_t i = 0; i < bg.setup.segmented_strips; i++)
                {
                    LVL_Background_strip x;
                    const auto &splits = bg.setup.segmented_splits;

                    if((i > 0) && (i - 1 < splits.size()))
                    {
                        double split_y = static_cast<double>(splits[i - 1]);
                        double frame_h = static_cast<double>(m_txData1.frame_h);
                        x.top = split_y / frame_h;
                    }
                    else
                        x.top = 0.0;

                    if(i < splits.size())
                    {
                        double split_y = static_cast<double>(splits[i]);
                        double frame_h = static_cast<double>(m_txData1.frame_h);
                        x.bottom = split_y / frame_h;
                    }
                    else
                        x.bottom = 1.0;

                    uint32_t segment_cur =  (i < splits.size() ? splits[i] : m_txData1.frame_h);
                    uint32_t segment_prev = (i == 0 ? 0 : splits[i - 1]);

                    x.height = (segment_cur > segment_prev) ? segment_cur - segment_prev : 1;

                    if(i < bg.setup.segmented_speeds.size())
                        x.repeat_h = bg.setup.segmented_speeds[i];
                    else
                        x.repeat_h = bg.setup.repeat_h;

                    if(x.repeat_h <= 0)
                        x.repeat_h = 1;

                    if(m_isAnimated && bg.setup.frames > 1)
                    {
                        double div_height = static_cast<double>(bg.setup.frames);
                        x.top /= div_height;
                        x.bottom /= div_height;
                    }

                    m_strips.push_back(x);
                }
                m_isSegmented = true;
            }
        }
    }
    break;

    case BgSetup::BG_TYPE_DoubleRow:
    {
        int tID = ConfigManager::getBGTexture(bg.setup.id);
        if(tID >= 0)
        {
            m_txData1 = ConfigManager::level_textures[tID];

            if(bg.setup.attached)
                m_color = bg.Color_lower;
            else
                m_color = bg.Color_upper;

            m_isAnimated = bg.setup.animated;
            m_animator_ID = bg.animator_ID;
        }

        int tID2 = ConfigManager::getBGTexture(bg.setup.id, true);
        if(tID2 >= 0)
        {
            m_txData2 = ConfigManager::level_textures[tID2];
            if(bg.setup.second_attached)
                m_color = bg.Color_upper;
            else
                m_color = bg.second_Color_upper;
        }

        pLogDebug("DoubleRow background images %s and %s",
                  bg.setup.image_n.c_str(),
                  bg.setup.second_image_n.c_str());
    }
    break;
    }
}

void StandardBackground::setScene(LevelScene * /*scene*/)
{
    /* Unsupported for standard backgrounds */
}

void StandardBackground::process(double /*tickDelay*/)
{
    /* Unsupported for standard backgrounds */
}

void StandardBackground::renderBackground(const PGE_RectF &box, double x, double y, double w, double h)
{
    int     sHeightI = Maths::iRound(box.height());
    double  sHeight = box.height();
    double  sWidth  = box.width();
    double  fWidth  = static_cast<double>(m_txData1.frame_w);
    double  fHeight1  = static_cast<double>(m_txData1.frame_h);
    double  imgPos_X = 0;
    double  imgPos_Y = 0;

    double refPointX = 0.0;
    refPointX = box.left() - x;

    if(m_repeat_h > 0)
    {
        //If referrence point is positive (for example, changing size of section lefter than section), fix it!
        if(refPointX > 0.0)
            refPointX = -((fWidth * m_repeat_h) - std::fmod(refPointX, fWidth * m_repeat_h));
        imgPos_X = std::fmod(refPointX / m_repeat_h, fWidth);
    }
    else
    {
        if(fWidth < w) //If image height less than screen
            imgPos_X = 0.0;
        else if(sWidth > fWidth)
        {
            if(refPointX <= 0.0)
                imgPos_X = (refPointX) / ((sWidth - w) / (fWidth - w));
            else
                imgPos_X = 0.0;
        }
        else
            imgPos_X = refPointX;
    }

    switch(m_repeat_v)
    {
    case BgSetup::BG_REPEAT_V_RP_RepeatParallax: //Repeat vertical with paralax coefficient =2
        imgPos_Y = (m_attached == 1) ?
                   std::fmod((box.top() - y) / 2.0, fHeight1) :
                   std::fmod((box.bottom() + h - y) / 2.0, fHeight1);
        break;

    case BgSetup::BG_REPEAT_V_ZR_NoRepeatNoParallax: //Zero Repeat
        imgPos_Y = (m_attached == 1) ?
                   (box.top() - y) :
                   (box.bottom() - y - fHeight1);
        break;

    case BgSetup::BG_REPEAT_V_NR_NoRepeat: //Proportional repeat
    default:
        if(fHeight1 < h) //If image height less than screen
            imgPos_Y = (m_attached == 1) ? 0.0 : (h - fHeight1);
        else if(sHeight > fHeight1)
            imgPos_Y = (box.top() - y) / ((sHeight - h) / (m_txData1.frame_h - h));
        else if(sHeightI == m_txData1.frame_h)
            imgPos_Y = box.top() - y;
        else
            imgPos_Y = (m_attached == 1) ?
                       (box.top() - y) :
                       (box.bottom() - y - fHeight1);

        break;
    }

    AniPos ani_x(0, 1);

    if(m_isAnimated) //Get current animated frame
        ani_x = ConfigManager::Animator_BG[static_cast<int>(m_animator_ID)].image();

    double lenght = 0;
    double lenght_v = 0;
    //for Tiled repeats
    int verticalRepeats = 1;

    if(m_bgType == BgSetup::BG_TYPE_Tiled)
    {
        verticalRepeats = 0;
        lenght_v -= fHeight1;
        imgPos_Y -= fHeight1 * ((m_attached == BgSetup::BG_ATTACH_TO_BOTTOM) ? -1.0 : 1.0);

        while(lenght_v <= h * 2.0  || (lenght_v <= fHeight1 * 2))
        {
            verticalRepeats++;
            lenght_v += fHeight1;
        }
    }

    double draw_x = imgPos_X;

    while(verticalRepeats > 0)
    {
        draw_x = imgPos_X;
        lenght = 0;

        while((lenght <= w * 2) || (lenght <= m_txData1.frame_w * 2))
        {
            size_t segmentRepeat = 1;

            if(m_isSegmented)
                segmentRepeat = m_strips.size();
            else
                m_backgrndG.setRect(draw_x, imgPos_Y, m_txData1.frame_w, m_txData1.frame_h);

            double drawnHeight = 0;
            double d_top    = ani_x.first;
            double d_bottom = ani_x.second;

            for(size_t mg = 0; mg < segmentRepeat; mg++)
            {
                if(m_isSegmented)
                {
                    const auto &strip = m_strips[mg];
                    draw_x       = std::fmod((box.left() - x) / strip.repeat_h, fWidth);
                    draw_x      += lenght;
                    m_backgrndG.setRect(draw_x, imgPos_Y + drawnHeight, m_txData1.frame_w, strip.height);
                    drawnHeight += strip.height;
                    d_top        = ani_x.first + strip.top;
                    d_bottom     = ani_x.first + strip.bottom;
                }

                GlRenderer::renderTexture(&m_txData1,
                                          static_cast<float>(m_backgrndG.left()),
                                          static_cast<float>(m_backgrndG.top()),
                                          static_cast<float>(m_backgrndG.width()),
                                          static_cast<float>(m_backgrndG.height()),
                                          static_cast<float>(d_top),
                                          static_cast<float>(d_bottom));
            }

            lenght += m_txData1.frame_w;
            draw_x += m_txData1.frame_w;
        }

        verticalRepeats--;

        if(verticalRepeats > 0)
            imgPos_Y += m_txData1.frame_h * ((m_attached == BgSetup::BG_ATTACH_TO_BOTTOM) ? -1 : 1);
    }

    if(m_bgType == BgSetup::BG_TYPE_DoubleRow)
    {
        ani_x = AniPos(0, 1);
        double fWidth2  = static_cast<double>(m_txData2.frame_w);
        double fHeight2 = static_cast<double>(m_txData2.frame_h);

        switch(m_second_attached)
        {
        case BgSetup::BG_ATTACH_2_TO_TOP_OF_FIRST:
            imgPos_Y = box.bottom() - y - fHeight1 - fHeight2;
            break;
        case BgSetup::BG_ATTACH_2_TO_BOTTOM:
            imgPos_Y = box.bottom() - y - fHeight2;
            break;
        case BgSetup::BG_ATTACH_2_TO_TOP:
            imgPos_Y = 0; //pCamera->s_bottom-y-txData1.h - txData2.h;
            break;
        }

        double imgPos_X = 0.0;
        refPointX = box.left() - x;
        //If referrence point is positive (for example, changing size of section lefter than section), fix it!
        if(refPointX > 0.0)
            refPointX = -((fWidth2 * m_second_repeat_h) - std::fmod(refPointX, fWidth2 * m_second_repeat_h));

        imgPos_X = std::fmod((refPointX / m_second_repeat_h), fWidth2);
        lenght = 0;

        while((lenght <= w * 2.0) || (lenght <= fWidth * 2))
        {
            m_backgrndG.setRect(imgPos_X, imgPos_Y, fWidth2, fHeight2);
            GlRenderer::renderTexture(&m_txData2,
                                      static_cast<float>(m_backgrndG.left()),
                                      static_cast<float>(m_backgrndG.top()),
                                      static_cast<float>(m_backgrndG.width()),
                                      static_cast<float>(m_backgrndG.height()),
                                      static_cast<float>(ani_x.first),
                                      static_cast<float>(ani_x.second));
            lenght +=   fWidth2;
            imgPos_X += fWidth2;
        }
    }
}

void StandardBackground::renderInScene(const PGE_RectF &/*box*/, double /*x*/, double /*y*/, double /*w*/, double /*h*/)
{
    //In-Scene not supported for standard backgrounds
}

void StandardBackground::renderForeground(const PGE_RectF &/*box*/, double /*x*/, double /*y*/, double /*w*/, double /*h*/)
{
    //Foregrounds are not supported for standard backgrounds
}
