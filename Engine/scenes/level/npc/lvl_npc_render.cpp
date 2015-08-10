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

#include "../lvl_npc.h"

#include <graphics/gl_renderer.h>
#include <data_configs/config_manager.h>
#include <graphics/window.h>
#include <fontman/font_manager.h>

void LVL_Npc::render(double camX, double camY)
{
    if(killed) return;
    if(isGenerator) return;
    if((!isActivated)&&(!warpSpawing)) return;

    AniPos x(0,1);
    if(animated)
    {
        if(is_scenery)
            x=ConfigManager::Animator_NPC[animator_ID].image(_direction);
        else
            x=animator.image(_direction);
    }

    PGE_RectF tPos; tPos.setLeft(0.0); tPos.setRight(1.0);
    tPos.setTop(x.first); tPos.setBottom(x.second);

    PGE_RectF npc;
    npc.setRect(round(posX()-offset.x())-camX,
                   round(posY()-offset.y())-camY,
                   frameSize.w(),
                   frameSize.h()
                   );
    if(isWarping)
    {
        if(warpSpriteOffset>=1.0)
            return;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(warpDirectO)
        {
            case WARP_LEFT://Left entrance, right Exit
                {
                    float wOfs = offset.x()/warpFrameW;//Relative X offset
                    float wOfsF = frameSize.w()/warpFrameW; //Relative hitbox width
                    tPos.setLeft(tPos.left()+wOfs+(warpSpriteOffset*wOfsF));
                    npc.setLeft( npc.left()+offset.x() );
                    npc.setRight( npc.right()-(warpSpriteOffset*_width) );
                }
                break;
            case WARP_TOP://Up entrance, down exit
                {
                    float hOfs = offset.y()/warpFrameH;//Relative Y offset
                    float hOfsF = frameSize.h()/warpFrameH; //Relative hitbox Height
                    tPos.setTop(tPos.top()+hOfs+(warpSpriteOffset*hOfsF));
                    npc.setTop( npc.top()+offset.y() );
                    npc.setBottom( npc.bottom()-(warpSpriteOffset*frameSize.h()) );
                }
                break;
            case WARP_RIGHT://right emtramce. left exit
                {
                    float wOfs =  offset.x()/warpFrameW;               //Relative X offset
                    float fWw =   1.0;   //Relative width of frame
                    float wOfHB = frameSize.w()/warpFrameW;                 //Relative width of hitbox
                    float wWAbs = warpFrameW*fWw;                   //Absolute width of frame
                    if(!warpResizedBody)
                    {
                        tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpSpriteOffset*wOfHB));
                        npc.setLeft( npc.left()+(warpSpriteOffset*frameSize.w()) );
                        npc.setRight( npc.right()-(wWAbs-offset.x()-frameSize.w()) );
                    }
                    else
                    {
                        tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpSpriteOffset*wOfHB));
                        npc.setLeft( npc.left()+offset.x() );
                        npc.setRight( npc.right()-(wWAbs-offset.x()-frameSize.w()*(1.0-warpSpriteOffset) ) );
                    }
                }
                break;
            case WARP_BOTTOM://down entrance, up exit
                {
                    float hOfs =  offset.y()/warpFrameH;               //Relative Y offset
                    float fHh =   animator.sizeOfFrame().h();  //Relative height of frame
                    float hOfHB = frameSize.h()/warpFrameH;                //Relative height of hitbox
                    float hHAbs = warpFrameH*fHh;                   //Absolute height of frame
                    if(!warpResizedBody)
                    {
                        tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpSpriteOffset*hOfHB));
                        npc.setTop( npc.top()+(warpSpriteOffset*frameSize.h()) );
                        npc.setBottom( npc.bottom()-(hHAbs-offset.y()-frameSize.h()) );
                    }
                    else
                    {
                        tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpSpriteOffset*hOfHB));
                        npc.setTop( npc.top()+offset.y() );
                        npc.setBottom( npc.bottom()-(hHAbs-offset.y()-frameSize.h()*(1.0-warpSpriteOffset) ) );
                    }
                }
                break;
            default:
                break;
        }
    }

    GlRenderer::renderTexture(&texture,
                              npc.x(),
                              npc.y(),
                              npc.width(),
                              npc.height(),
                              tPos.top(),
                              tPos.bottom(),
                              tPos.left(),
                              tPos.right());

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(QString(" %1 \n%2%3%4\n %5")
                               .arg(collided_top.size())
                               .arg(collided_left.size())
                               .arg(collided_center.size())
                               .arg(collided_right.size())
                               .arg(collided_bottom.size())
                               , round(20+posX()-camX), -50+round(posY()-camY), 3);
    }
}

