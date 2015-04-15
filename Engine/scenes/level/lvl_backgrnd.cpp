/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <graphics/graphics.h>
#include <graphics/window.h>

#include "lvl_backgrnd.h"

#include <QtDebug>

LVL_Background::LVL_Background()
{
    construct();
}

LVL_Background::LVL_Background(PGE_LevelCamera *parentCamera)
{
    construct();
    pCamera = parentCamera;
}

void LVL_Background::construct()
{
    pCamera = NULL;
    color.r = 0.0f;
    color.g = 0.0f;
    color.b = 0.0f;
    bgType = single_row;
    isAnimated = false;
    isMagic = false;
    animator_ID = 0;
    glClearColor(color.r, color.g, color.b, 1.0f);
}



LVL_Background::~LVL_Background()
{}


void LVL_Background::setBg(obj_BG &bg)
{
    setup = &bg;

    bgType = (type)bg.type;

    qDebug()<< "BG Type" << bgType;

    //Reset magic background parameters
    isMagic = false;
    strips.clear();

    switch(bgType)
    {
        case single_row:
        case tiled:
            {
                long tID = ConfigManager::getBGTexture(bg.id);
                if( tID >= 0 )
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
                        for(int i=0; (unsigned int)i < bg.magic_strips; i++)
                        {
                            LVL_Background_strip x;
                            if( i-1 <  bg.magic_splits_i.size())
                                x.top = (i==0 ? 0.0 : ((double)bg.magic_splits_i[i-1]/(double)txData1.h) );
                            else
                                x.top = 0.0;

                            if( i <  bg.magic_splits_i.size())
                                x.bottom = (double)bg.magic_splits_i[i] / (double)txData1.h;
                            else
                                x.bottom = 1.0;

                            x.height = ( (i<bg.magic_splits_i.size()) ? bg.magic_splits_i[i] : txData1.h)
                                                 - (i==0 ? 0.0 : (bg.magic_splits_i[i-1]) );

                            if( i <  bg.magic_speeds_i.size())
                                x.repeat_h = bg.magic_speeds_i[i];
                            else
                                x.repeat_h = bg.repeat_h;
                            if(x.repeat_h<=0) x.repeat_h = 1;

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
                    long tID = ConfigManager::getBGTexture(bg.id);
                    if( tID >= 0 )
                    {
                        txData1 = ConfigManager::level_textures[tID];
                        if(bg.attached)
                            color = bg.Color_lower;
                        else
                            color = bg.Color_upper;

                        isAnimated = bg.animated;
                        animator_ID = bg.animator_ID;
                    }

                    long tID2 = ConfigManager::getBGTexture(bg.id, true);
                    if( tID2 >= 0 )
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
                    qDebug()<<"DoubleRow";
            }
            break;
        default:
            break;
    }

    //set background color
    qDebug() << color.r << color.g << color.b;
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void LVL_Background::setNone()
{
    setup = NULL;
    color.r = 0.0f;
    color.g = 0.0f;
    color.b = 0.0f;
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void LVL_Background::draw(float x, float y)
{
    if(setup && pCamera) //draw BG if is set
    {
        double sHeight = fabs(pCamera->s_top-pCamera->s_bottom);
        double sWidth = fabs(pCamera->s_left-pCamera->s_right);

        int imgPos_X;
        int imgPos_Y;

        if(setup->repeat_h>0)
        {
            imgPos_X = (int)round((pCamera->s_left-x)/setup->repeat_h) % (int)round(txData1.w);
        }
        else
        {
            if(txData1.w < PGE_Window::Width) //If image height less than screen
                imgPos_X = 0;
            else

            if( sWidth > (double)txData1.w )
            {
                imgPos_X =
                        (pCamera->s_left-x)
                        /
                        (
                            (sWidth - PGE_Window::Width)/
                            (txData1.w - PGE_Window::Width)
                        );
            }
            else
            {
                imgPos_X = pCamera->s_left-x;
            }
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

        switch(setup->repead_v)
        {
            case 2: //Repeat vertical with paralax coefficient =2
                imgPos_Y = (setup->attached==1)?
                        (int)round((pCamera->s_top-y)/2) % (int)round(txData1.h):
                        (int)round((pCamera->s_bottom+pCamera->h()-y)/2) % (int)round(txData1.h);
                break;
            case 1: //Zero Repeat
                imgPos_Y = (setup->attached==1) ? pCamera->s_top-y : (pCamera->s_bottom-y-txData1.h);
                break;
            case 0: //Proportional repeat
            default:

                if(txData1.h < PGE_Window::Height) //If image height less than screen
                    imgPos_Y = (setup->attached==1) ? 0 : (PGE_Window::Height-txData1.h);
                else

                if( sHeight > (double)txData1.h )
                    imgPos_Y =
                            (pCamera->s_top-y)
                            /
                            (
                                (sHeight - PGE_Window::Height)/
                                (txData1.h - PGE_Window::Height)
                            );
                else if(sHeight == (double)txData1.h)
                    imgPos_Y = pCamera->s_top-y;
                else
                    imgPos_Y = (setup->attached==1) ? pCamera->s_top-y : (pCamera->s_bottom-y-txData1.h);
                break;
        }

        //((setup->attached==1)? pCamera->s_top-y : pCamera->s_bottom)

        //fabs(pCamera->s_top-pCamera->s_bottom)
        //txData1.h
        //PGE_Window::Height
        //pos_y

        QRectF backgrndG;
        //draw!


        AniPos ani_x(0,1);

        if(isAnimated) //Get current animated frame
            ani_x = ConfigManager::Animator_BG[animator_ID].image();
        int lenght=0;
        int lenght_v=0;
        //for Tiled repeats
        int verticalRepeats=1;

        if(bgType==tiled)
        {
            verticalRepeats=0;
            lenght_v -= txData1.h;
            imgPos_Y -= txData1.h * ( (setup->attached==0)? -1 : 1 );
            while(lenght_v <= PGE_Window::Height*2  ||  (lenght_v <=txData1.h*2))
            {
                verticalRepeats++;
                lenght_v += txData1.h;
            }
        }

        int draw_x = imgPos_X;
        while(verticalRepeats > 0)
        {
            draw_x = imgPos_X;
            lenght = 0;
            while((lenght <= PGE_Window::Width*2) || (lenght <=txData1.w*2))
            {
                int magicRepeat=1;
                if(isMagic)
                {
                    magicRepeat = strips.size();
                }
                else
                {
                    backgrndG = QRectF(QPointF(draw_x, imgPos_Y), QPointF(draw_x+txData1.w, imgPos_Y+txData1.h) );
                }

                int drawedHeight=0;
                float d_top    = ani_x.first;
                float d_bottom = ani_x.second;
                for(int mg=0; mg < magicRepeat;mg++)
                {
                    if(isMagic)
                    {
                        draw_x = (int)round((pCamera->s_left-x)/strips[mg].repeat_h) % (int)round(txData1.w);
                        draw_x += lenght;
                        backgrndG = QRectF(QPointF(draw_x, imgPos_Y+drawedHeight),
                                           QPointF(draw_x+txData1.w, imgPos_Y+drawedHeight+
                                                                                           strips[mg].height) );
                        drawedHeight += strips[mg].height;
                        d_top = strips[mg].top;
                        d_bottom = strips[mg].bottom;
                    }
                    glColor4f( 1.f, 1.f, 1.f, 1.f);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture( GL_TEXTURE_2D, txData1.texture );
                    glBegin( GL_QUADS );
                        glTexCoord2f( 0, d_top );
                        glVertex2f( backgrndG.left(), backgrndG.top());

                        glTexCoord2f( 1, d_top );
                        glVertex2f(  backgrndG.right(), backgrndG.top());

                        glTexCoord2f( 1, d_bottom );
                        glVertex2f(  backgrndG.right(),  backgrndG.bottom());

                        glTexCoord2f( 0, d_bottom );
                        glVertex2f( backgrndG.left(),  backgrndG.bottom());

                    glEnd();
                }
                lenght += txData1.w;
                draw_x += txData1.w;
            }

            verticalRepeats--;
            if(verticalRepeats>0)
            {
                imgPos_Y += txData1.h * ( (setup->attached==0)? -1 : 1 );
            }
        }



        if(bgType==double_row)
        {
            ani_x = AniPos(0,1);

            if(setup->second_attached==0) // over first
                imgPos_Y = pCamera->s_bottom-y-txData1.h - txData2.h;
            else
            if(setup->second_attached==1) //bottom
                imgPos_Y = pCamera->s_bottom-y - txData2.h;
                else
            if(setup->second_attached==2) //top
                imgPos_Y = 0; //pCamera->s_bottom-y-txData1.h - txData2.h;

            int imgPos_X = (int)round((pCamera->s_left-x)/setup->second_repeat_h) % (int)round(txData2.w);

            lenght = 0;
            while((lenght <= PGE_Window::Width*2) || (lenght <=txData1.w*2))
            {
                backgrndG = QRectF(QPointF(imgPos_X, imgPos_Y), QPointF(imgPos_X+txData2.w, imgPos_Y+txData2.h) );
                glColor4f( 1.f, 1.f, 1.f, 1.f);
                glEnable(GL_TEXTURE_2D);
                glBindTexture( GL_TEXTURE_2D, txData2.texture );
                glBegin( GL_QUADS );
                    glTexCoord2f( 0, ani_x.first );
                    glVertex2f( backgrndG.left(), backgrndG.top());

                    glTexCoord2f( 1, ani_x.first );
                    glVertex2f(  backgrndG.right(), backgrndG.top());

                    glTexCoord2f( 1, ani_x.second );
                    glVertex2f(  backgrndG.right(),  backgrndG.bottom());

                    glTexCoord2f( 0, ani_x.second );
                    glVertex2f( backgrndG.left(),  backgrndG.bottom());

                glEnd();
                lenght += txData2.w;
                imgPos_X += txData2.w;
            }
        }

    }
}

