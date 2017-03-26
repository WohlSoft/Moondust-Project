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

#include <data_configs/config_manager.h>
#include <Utils/maths.h>
#include <graphics/graphics.h>
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <fmt/fmt_format.h>
#include <common_features/logger.h>

#include "lvl_backgrnd.h"


LVL_Background::LVL_Background()
{
    construct();
}

LVL_Background::LVL_Background(const LVL_Background &_bg)
{
    _isInited = _bg._isInited;
    setup = _bg.setup;
    color = _bg.color;
    box   = _bg.box;
    isNoImage = _bg.isNoImage;
    bgType = _bg.bgType;
    txData1 = _bg.txData1;
    txData2 = _bg.txData2;
    isAnimated = _bg.isAnimated;
    animator_ID = _bg.animator_ID;
    isMagic = _bg.isMagic;
    strips = _bg.strips;
}

void LVL_Background::construct()
{
    _isInited = false;
    isNoImage = true;
    color.r = 0.0f;
    color.g = 0.0f;
    color.b = 0.0f;
    bgType = single_row;
    isAnimated = false;
    isMagic = false;
    animator_ID = 0;
}



LVL_Background::~LVL_Background()
{}


void LVL_Background::setBg(obj_BG &bg)
{
    if((!bg.isInit) && (!bg.second_isInit))
    {
        std::string CustomTxt = ConfigManager::Dir_BG.getCustomFile(fmt::format("background2-{0}.ini", bg.id));
        if(CustomTxt.empty())
            CustomTxt = ConfigManager::Dir_BG.getCustomFile(fmt::format("background2-{0}.txt", bg.id));
        if(!CustomTxt.empty())
            ConfigManager::loadLevelBackground(bg, "background2", &bg, CustomTxt);
    }

    setup = bg;
    bgType = static_cast<type>(setup.type);
    pLogDebug("BG Type %d", bgType);
    //Reset magic background parameters
    isMagic = false;
    strips.clear();

    switch((int)bgType)
    {
    case single_row:
    case tiled:
    {
        int tID = ConfigManager::getBGTexture(bg.id);
        if(tID >= 0)
        {
            txData1 = ConfigManager::level_textures[tID];

            if(bg.attached)
                color = bg.Color_lower;
            else
                color = bg.Color_upper;

            isAnimated = bg.animated;
            animator_ID = bg.animator_ID;

            if(bg.magic)
            {
                for(uint32_t i = 0; i < bg.magic_strips; i++)
                {
                    LVL_Background_strip x;

                    if((i > 0) && (i - 1 <  bg.magic_splits_i.size()))
                    {
                        x.top = (static_cast<double>(bg.magic_splits_i[i - 1])
                                / static_cast<double>(txData1.frame_h));
                    }
                    else
                        x.top = 0.0;

                    if(i < bg.magic_splits_i.size())
                        x.bottom = static_cast<double>(bg.magic_splits_i[i]) / static_cast<double>(txData1.frame_h);
                    else
                        x.bottom = 1.0;

                    x.height = Maths::iRound(
                                   ( (i < bg.magic_splits_i.size()) ? bg.magic_splits_i[i] : txData1.frame_h)
                                   - (i == 0 ? 0.0 : (bg.magic_splits_i[i - 1]))
                               );

                    if(i < bg.magic_speeds_i.size())
                        x.repeat_h = bg.magic_speeds_i[i];
                    else
                        x.repeat_h = bg.repeat_h;

                    if(x.repeat_h <= 0)
                        x.repeat_h = 1;
                    //                            qDebug() << "Magic " << (i==0 ? 0 : bg.magic_splits_i[i-1] )
                    //                                    << (( i <  bg.magic_splits_i.size())
                    //                                       ? bg.magic_splits_i[i]: txData1.h )
                    //                                    << x.height
                    //                                    << x.repeat_h
                    //                                    << x.top << x.bottom;
                    strips.push_back(x);
                }

                //qDebug() <<  bg.magic_splits_i.size() << bg.magic_speeds_i.size();
                isMagic = true;
            }
        }

        //qDebug()<<"SingleRow";
    }
    break;

    case double_row:
    {
        int tID = ConfigManager::getBGTexture(bg.id);

        if(tID >= 0)
        {
            txData1 = ConfigManager::level_textures[tID];

            if(bg.attached)
                color = bg.Color_lower;
            else
                color = bg.Color_upper;

            isAnimated = bg.animated;
            animator_ID = bg.animator_ID;
        }

        int tID2 = ConfigManager::getBGTexture(bg.id, true);

        if(tID2 >= 0)
        {
            txData2 = ConfigManager::level_textures[tID2];

            /*
            if(tmpstr=="overfirst")
                sbg.second_attached = 0;
            else if(tmpstr=="bottom")
                sbg.second_attached = 1;
            else if(tmpstr=="top")
                sbg.second_attached = 2;
            else sbg.second_repeat_v = 0;
            */

            if(bg.second_attached)
                color = bg.Color_upper;
            else
                color = bg.second_Color_upper;
        }

        pLogDebug("DoubleRow");
    }
    break;

    default:
        break;
    }

    isNoImage = false;
}

void LVL_Background::setNone()
{
    isNoImage = true;
    setup.id = 0;
    color.r = 0.0f;
    color.g = 0.0f;
    color.b = 0.0f;
}

void LVL_Background::setBox(PGE_RectF &_box)
{
    box = _box;
}

void LVL_Background::draw(double x, double y, double w, double h)
{
    GlRenderer::renderRect(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h), color.r, color.g, color.b, 1.0f);

    if(isNoImage)
        return; //draw BG if is set

    int     sHeightI = Maths::iRound(box.height());
    double  sHeight = box.height();
    double  sWidth  = box.width();
    double  fWidth  = static_cast<double>(txData1.frame_w);
    double  fHeight1  = static_cast<double>(txData1.frame_h);
    double  imgPos_X = 0;
    double  imgPos_Y = 0;

    if(setup.repeat_h > 0)
        imgPos_X = std::fmod((box.left() - x) / setup.repeat_h, txData1.frame_w);
    else
    {
        if(fWidth < w) //If image height less than screen
            imgPos_X = 0.0;
        else if(sWidth > fWidth)
            imgPos_X = (box.left() - x) / ((sWidth - w) / (fWidth - w));
        else
            imgPos_X = box.left() - x;
    }

    //     tmpstr = bgset.value("repeat-v", "NR").toString();
    //   if(tmpstr=="NR") //Without repeat, parallax is proportional to section height
    //      sbg.repead_v = 0;
    //    else if(tmpstr=="ZR") //Static: without repeat and without parallax
    //       sbg.repead_v = 1;
    //   else if(tmpstr=="RP") //paralax with coefficient x2
    //    sbg.repead_v = 2;
    //   else if(tmpstr=="RZ") //repeat without parallax
    //          sbg.repead_v = 3;
    //    else sbg.repead_v = 0;

    switch(setup.repead_v)
    {
    case 2: //Repeat vertical with paralax coefficient =2
        imgPos_Y = (setup.attached == 1) ?
                   std::fmod((box.top() - y) / 2.0, fHeight1) :
                   std::fmod((box.bottom() + h - y) / 2.0, fHeight1);
        break;

    case 1: //Zero Repeat
        imgPos_Y = (setup.attached == 1) ?
                   (box.top() - y) :
                   (box.bottom() - y - fHeight1);
        break;

    case 0: //Proportional repeat
    default:
        if(fHeight1 < h) //If image height less than screen
            imgPos_Y = (setup.attached == 1) ? 0.0 : (h - fHeight1);
        else if(sHeight > fHeight1)
            imgPos_Y = (box.top() - y) / ((sHeight - h) / (txData1.frame_h - h));
        else if(sHeightI == txData1.frame_h)
            imgPos_Y = box.top() - y;
        else
            imgPos_Y = (setup.attached == 1) ?
                       (box.top() - y) :
                       (box.bottom() - y - fHeight1);

        break;
    }

    AniPos ani_x(0, 1);

    if(isAnimated) //Get current animated frame
        ani_x = ConfigManager::Animator_BG[static_cast<int>(animator_ID)].image();

    double lenght = 0;
    double lenght_v = 0;
    //for Tiled repeats
    int verticalRepeats = 1;

    if(bgType == tiled)
    {
        verticalRepeats = 0;
        lenght_v -= fHeight1;
        imgPos_Y -= fHeight1 * ((setup.attached == 0) ? -1.0 : 1.0);

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

        while((lenght <= w * 2) || (lenght <= txData1.frame_w * 2))
        {
            int magicRepeat = 1;

            if(isMagic)
                magicRepeat = strips.size();
            else
                backgrndG.setRect(draw_x, imgPos_Y, txData1.frame_w, txData1.frame_h);

            double drawnHeight = 0;
            double d_top    = ani_x.first;
            double d_bottom = ani_x.second;

            for(int mg = 0; mg < magicRepeat; mg++)
            {
                if(isMagic)
                {
                    draw_x       = std::fmod((box.left() - x) / strips[mg].repeat_h, fWidth);
                    draw_x      += lenght;
                    backgrndG.setRect(draw_x, imgPos_Y + drawnHeight, txData1.frame_w, strips[mg].height);
                    drawnHeight += strips[mg].height;
                    d_top        = strips[mg].top;
                    d_bottom     = strips[mg].bottom;
                }

                GlRenderer::renderTexture(&txData1,
                                          static_cast<float>(backgrndG.left()),
                                          static_cast<float>(backgrndG.top()),
                                          static_cast<float>(backgrndG.width()),
                                          static_cast<float>(backgrndG.height()),
                                          static_cast<float>(d_top),
                                          static_cast<float>(d_bottom));
            }

            lenght += txData1.frame_w;
            draw_x += txData1.frame_w;
        }

        verticalRepeats--;

        if(verticalRepeats > 0)
            imgPos_Y += txData1.frame_h * ((setup.attached == 0) ? -1 : 1);
    }

    if(bgType == double_row)
    {
        ani_x = AniPos(0, 1);
        double fWidth2 = static_cast<double>(txData2.frame_w);
        double fHeight2 = static_cast<double>(txData2.frame_h);

        if(setup.second_attached == 0) // over first
            imgPos_Y = box.bottom() - y - fHeight1 - fHeight2;
        else if(setup.second_attached == 1) //bottom
            imgPos_Y = box.bottom() - y - fHeight2;
        else if(setup.second_attached == 2) //top
            imgPos_Y = 0; //pCamera->s_bottom-y-txData1.h - txData2.h;

        double imgPos_X = std::fmod(((box.left() - x) / setup.second_repeat_h), fWidth2);
        lenght = 0;

        while((lenght <= w * 2.0) || (lenght <= fWidth * 2))
        {
            backgrndG.setRect(imgPos_X, imgPos_Y, fWidth2, fHeight2);
            GlRenderer::renderTexture(&txData2,
                                      static_cast<float>(backgrndG.left()),
                                      static_cast<float>(backgrndG.top()),
                                      static_cast<float>(backgrndG.width()),
                                      static_cast<float>(backgrndG.height()),
                                      static_cast<float>(ani_x.first),
                                      static_cast<float>(ani_x.second));
            lenght +=   fWidth2;
            imgPos_X += fWidth2;
        }
    }
}

bool LVL_Background::isInit()
{
    return _isInited;
}

unsigned long LVL_Background::curBgId()
{
    if(_isInited)
        return 0;
    else
        return setup.id;
}
