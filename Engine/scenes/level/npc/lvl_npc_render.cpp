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

#include "../lvl_npc.h"

#include <graphics/gl_renderer.h>
#include <data_configs/config_manager.h>
#include <graphics/window.h>
#include <fontman/font_manager.h>
#include <common_features/fmt_format_ne.h>

void LVL_Npc::render(double camX, double camY)
{
    if(killed) return;
    if(m_isGenerator) return;
    if((!isActivated)&&(!warpSpawing)) return;

    bool doDraw=true;
    AniPos x(0,1);
    if(animated)
    {
        if(is_shared_animation)
            x=ConfigManager::Animator_NPC[int(animator_ID)].image(_direction);
        else
            x=animator.image(_direction);
    }
    double offsetX = offset.x()+lua_offset.x();
    double offsetY = offset.y()+lua_offset.y();

    PGE_RectF tPos; tPos.setLeft(0.0); tPos.setRight(1.0);
    tPos.setTop(x.first); tPos.setBottom(x.second);

    PGE_RectF npc;
    npc.setRect(round(posX()-offsetX)-camX,
                   round(posY()-offsetY)-camY,
                   frameSize.w(),
                   frameSize.h()
                   );
    if(isWarping)
    {
        if(warpSpriteOffset >= 1.0)
            return;
        PGE_RectF  bodyPos = m_momentum.rectF();
                   bodyPos.setPos(round(bodyPos.x()-camX), round(bodyPos.y()-camY));
        PGE_RectF &textPos = npc;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch( warpDirectO )
        {
            case WARP_LEFT://Left entrance, right Exit
                {
                    //Offset at right side, crop left side
                    double cropLeft = 0.0;
                    double offset = (warpResizedBody?double(setup->setup.width) : bodyPos.width())*double(warpSpriteOffset);
                    bodyPos.setRight( bodyPos.right()-offset );
                    textPos.setPos(textPos.x()-offset, textPos.y());
                    if(textPos.left() < bodyPos.left())
                    {
                        cropLeft = fabs(bodyPos.left()-textPos.left());
                        textPos.setLeft(bodyPos.left());
                    }
                    double wOfs = cropLeft/double(warpFrameW);//Relative X offset
                    tPos.setLeft( tPos.left()+wOfs );
                    if( textPos.right() <= bodyPos.left() )
                        doDraw = false;
                }
                break;
            case WARP_TOP://Up entrance, down exit
                {
                    //Offset at bottom side, crop top side
                    double cropTop = 0.0;
                    double offset = (warpResizedBody?double(setup->setup.height) : bodyPos.height())*double(warpSpriteOffset);
                    bodyPos.setBottom( bodyPos.bottom()-offset );
                    textPos.setPos(textPos.x(), textPos.y()-offset);
                    if(textPos.top() < bodyPos.top())
                    {
                        cropTop = fabs(bodyPos.top()-textPos.top());
                        textPos.setTop(bodyPos.top());
                    }
                    double wOfs = ( cropTop/double(warpFrameH) ) * (double(texture.frame_h)/double(texture.h));//Relative X offset
                    tPos.setTop( tPos.top()+wOfs );
                    if( textPos.bottom() <= bodyPos.top() )
                        doDraw = false;
                }
                break;
            case WARP_RIGHT://right emtramce. left exit
                {
                    //Offset at left side, crop right side
                    double cropRight = 0.0;
                    double offset = (warpResizedBody?double(setup->setup.width) : bodyPos.width())*double(warpSpriteOffset);
                    bodyPos.setLeft( bodyPos.left()+offset );
                    textPos.setPos(textPos.x()+(warpResizedBody ? 0.0 : offset), textPos.y());
                    if(textPos.right() > bodyPos.right())
                    {
                        cropRight = fabs(textPos.right()-bodyPos.right());
                        textPos.setRight(bodyPos.right());
                    }
                    double wOfs = cropRight/double(warpFrameW);//Relative X offset
                    tPos.setRight( tPos.right()-wOfs );
                    if( textPos.left() >= bodyPos.right() )
                        doDraw = false;
                }
                break;
            case WARP_BOTTOM://down entrance, up exit
                {
                    //Offset at top side, crop bottop side
                    double cropBottom = 0.0;
                    double offset = (warpResizedBody?double(setup->setup.height) : bodyPos.height())*double(warpSpriteOffset);
                    bodyPos.setTop( bodyPos.top()+offset );
                    textPos.setPos(textPos.x(), textPos.y()+(warpResizedBody ? 0.0 : offset));
                    if(textPos.bottom() > bodyPos.bottom())
                    {
                        cropBottom  = fabs(textPos.bottom()-bodyPos.bottom());
                        textPos.setBottom(bodyPos.bottom());
                    }
                    double wOfs = ( cropBottom/double(warpFrameH) ) * (double(texture.frame_h)/double(texture.h));//Relative X offset
                    tPos.setBottom( tPos.bottom()-wOfs );
                    if( textPos.top() >= bodyPos.bottom() )
                        doDraw = false;
                }
                break;
            default:
                break;
        }
    }

    if(doDraw)
    {
        GlRenderer::renderTexture(&texture,
                                  float(npc.x()),
                                  float(npc.y()),
                                  float(npc.width()),
                                  float(npc.height()),
                                  float(tPos.top()),
                                  float(tPos.bottom()),
                                  float(tPos.left()),
                                  float(tPos.right()));
    }

    if(PGE_Window::showDebugInfo)
    {
        std::string warpingInfo;
        if(isWarping)
        {
            switch(warpDirectO)
            {
            case WARP_LEFT:     warpingInfo="LEFT"; break;
            case WARP_RIGHT:    warpingInfo="RIGHT"; break;
            case WARP_TOP:      warpingInfo="TOP"; break;
            case WARP_BOTTOM:   warpingInfo="BOTTOM"; break;
            }
        }
        FontManager::printText(fmt::format_ne(  " {0} \n"
                                           "{1}{2}{3}\n"
                                             " {4}  {5} {6}",
                                        l_contactT.size(),
                                        l_contactL.size(),
                                        l_contactAny.size(),
                                        l_contactR.size(),
                                        l_contactB.size(),
                               //.arg(collision_speed_add.size())
                                        warpingInfo,
                                    std::string(m_cliff ? "CLIFF" : "")+
                                    std::string(m_blockedAtLeft?"|<" : "")+
                                    std::string(m_blockedAtRight?">|" : "")
                                    )
                               //.arg(LEGACY_m_speedAddingTopElements.size())
                               //.arg(LEGACY_m_speedAddingBottomElements.size())
                               , int(round(20+posX()-camX)), -50+int(round(posY()-camY)), FontManager::DefaultRaster);
    }
}

