#include "../data_configs/config_manager.h"
#include "graphics.h"
#include "../graphics/window.h"

#include "graphics_lvl_backgrnd.h"

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

    switch(bgType)
    {
        case single_row:
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
                qDebug()<<"SingleRow";
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
        case tiled:

            qDebug()<<"Tiled";
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

        switch(bgType)
        {
        case single_row:
        case double_row:
            {
                int imgPos_X;
                int imgPos_Y;
                if(setup->repeat_h>0)
                {
                    imgPos_X = (int)round((pCamera->s_left-x)/setup->repeat_h) % (int)round(txData1.w);
                }
                else
                {
                    imgPos_X = (int)round((pCamera->s_left-x)/setup->repeat_h) % (int)round(txData1.w);

                    if(txData1.w < PGE_Window::Width) //If image height less than screen
                        imgPos_X = 0;
                    else

                    if( sWidth > (double)txData1.h )
                        imgPos_X =
                                (pCamera->s_left-x)
                                /
                                (
                                    (sWidth - PGE_Window::Width)/
                                    (txData1.w - PGE_Window::Width)
                                );
                    else
                        imgPos_X = pCamera->s_left-x;
                }


                //     tmpstr = bgset.value("repeat-v", "NR").toString();
                //   if(tmpstr=="NR") //Without repeat, parallax is proportional to section height
                //      sbg.repead_v = 0;
                //    else if(tmpstr=="ZR") //Static: without repeat and without parallax
                //       sbg.repead_v = 1;
                //   else if(tmpstr=="RP") //Repeat with coefficient x2
                //    sbg.repead_v = 2;
                //   else if(tmpstr=="RZ") //repeat without parallax
                //          sbg.repead_v = 3;
                //    else sbg.repead_v = 0;

                switch(setup->repead_v)
                {
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

                QRectF blockG;
                //draw!


                AniPos ani_x(0,1);

                if(isAnimated) //Get current animated frame
                    ani_x = ConfigManager::Animator_BG[animator_ID]->image();
                int lenght=0;
                while((lenght <= PGE_Window::Width*2) || (lenght <=txData1.w*2))
                {
                    blockG = QRectF(QPointF(imgPos_X, imgPos_Y), QPointF(imgPos_X+txData1.w, imgPos_Y+txData1.h) );
                    glColor4f( 1.f, 1.f, 1.f, 1.f);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture( GL_TEXTURE_2D, txData1.texture );
                    glBegin( GL_QUADS );
                        glTexCoord2f( 0, ani_x.first );
                        glVertex2f( blockG.left(), blockG.top());

                        glTexCoord2f( 1, ani_x.first );
                        glVertex2f(  blockG.right(), blockG.top());

                        glTexCoord2f( 1, ani_x.second );
                        glVertex2f(  blockG.right(),  blockG.bottom());

                        glTexCoord2f( 0, ani_x.second );
                        glVertex2f( blockG.left(),  blockG.bottom());

                    glEnd();
                    lenght += txData1.w;
                    imgPos_X += txData1.w;
                }

                if(bgType==double_row)
                {
                    ani_x = AniPos(0,1);


                    if(setup->second_attached==0)
                        imgPos_Y = pCamera->s_bottom-y-txData1.h - txData2.h;

                    int imgPos_X = (int)round((pCamera->s_left-x)/setup->second_repeat_h) % (int)round(txData2.w);

                    lenght = 0;
                    while((lenght <= PGE_Window::Width*2) || (lenght <=txData1.w*2))
                    {
                        blockG = QRectF(QPointF(imgPos_X, imgPos_Y), QPointF(imgPos_X+txData2.w, imgPos_Y+txData2.h) );
                        glColor4f( 1.f, 1.f, 1.f, 1.f);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture( GL_TEXTURE_2D, txData2.texture );
                        glBegin( GL_QUADS );
                            glTexCoord2f( 0, ani_x.first );
                            glVertex2f( blockG.left(), blockG.top());

                            glTexCoord2f( 1, ani_x.first );
                            glVertex2f(  blockG.right(), blockG.top());

                            glTexCoord2f( 1, ani_x.second );
                            glVertex2f(  blockG.right(),  blockG.bottom());

                            glTexCoord2f( 0, ani_x.second );
                            glVertex2f( blockG.left(),  blockG.bottom());

                        glEnd();
                        lenght += txData2.w;
                        imgPos_X += txData2.w;
                    }
                }
            }
            break;
        default:
            break;

        }
    }
}

