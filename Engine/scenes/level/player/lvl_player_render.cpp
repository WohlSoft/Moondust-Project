/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_player.h"
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <fontman/font_manager.h>

void LVL_Player::render(double camX, double camY)
{
    if(!isAlive) return;
    if(!_isInited) return;
    if(_no_render) return;

    if(blink_screen)
    {
        blink_screen_state=!blink_screen_state;
        if(blink_screen_state) return;
    }

    PGE_RectF tPos = animator.curFrame();
    PGE_PointF Ofs = animator.curOffset();

    PGE_RectF player;
    player.setRect(round(posX()-camX)-Ofs.x(),
                   round(posY()-Ofs.y())-camY,
                            frameW,
                            frameH
                         );

    if(isWarping)
    {
        if(warpPipeOffset>=1.0)
            return;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(warpDirectO)
        {
            case 2://Left entrance, right Exit
                {
                    float wOfs = Ofs.x()/warpFrameW;//Relative X offset
                    float wOfsF = _width/warpFrameW; //Relative hitbox width
                    tPos.setLeft(tPos.left()+wOfs+(warpPipeOffset*wOfsF));
                    player.setLeft( player.left()+Ofs.x() );
                    player.setRight( player.right()-(warpPipeOffset*_width) );
                }
                break;
            case 1://Up entrance, down exit
                {
                    float hOfs = Ofs.y()/warpFrameH;//Relative Y offset
                    float hOfsF = _height/warpFrameH; //Relative hitbox Height
                    tPos.setTop(tPos.top()+hOfs+(warpPipeOffset*hOfsF));
                    player.setTop( player.top()+Ofs.y() );
                    player.setBottom( player.bottom()-(warpPipeOffset*_height) );
                }
                break;
            case 4://right emtramce. left exit
                {
                    float wOfs =  Ofs.x()/warpFrameW;               //Relative X offset
                    float fWw =   animator.sizeOfFrame().w();   //Relative width of frame
                    float wOfHB = _width/warpFrameW;                 //Relative width of hitbox
                    float wWAbs = warpFrameW*fWw;                   //Absolute width of frame
                    tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpPipeOffset*wOfHB));
                    player.setLeft( player.left()+(warpPipeOffset*_width) );
                    player.setRight( player.right()-(wWAbs-Ofs.x()-_width) );
                }
                break;
            case 3://down entrance, up exit
                {
                    float hOfs =  Ofs.y()/warpFrameH;               //Relative Y offset
                    float fHh =   animator.sizeOfFrame().h();  //Relative height of frame
                    float hOfHB = _height/warpFrameH;                //Relative height of hitbox
                    float hHAbs = warpFrameH*fHh;                   //Absolute height of frame
                    tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpPipeOffset*hOfHB));
                    player.setTop( player.top()+(warpPipeOffset*_height) );
                    player.setBottom( player.bottom()-(hHAbs-Ofs.y()-_height) );
                }
                break;
            default:
                break;
        }
    }


    GlRenderer::renderTexture(&texture,
                              player.x(),
                              player.y(),
                              player.width(),
                              player.height(),
                              tPos.top(),
                              tPos.bottom(),
                              tPos.left(),
                              tPos.right());

    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(QString("%1-%2").arg(characterID).arg(stateID), round(posX()-camX), round(posY()-camY));
        FontManager::printText(QString(" %1 \n%2%3%4\n %5 ")
                               .arg(collided_top.size())
                               .arg(collided_left.size())
                               .arg(collided_center.size())
                               .arg(collided_right.size())
                               .arg(collided_bottom.size())
                               , round(20+posX()-camX), -50+round(posY()-camY), 3);
    }

}

