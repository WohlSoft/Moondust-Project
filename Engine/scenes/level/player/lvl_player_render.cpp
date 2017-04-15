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

#include "../lvl_player.h"
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <fontman/font_manager.h>
#include <fmt/fmt_format.h>
#include <scenes/scene_level.h>

void LVL_Player::render(double camX, double camY)
{
    if(!isAlive) return;
    if(!m_isInited) return;
    if(m_noRender) return;

    if(m_blinkScreen)
    {
        //Syncronious blinking on all cameras!
        if(m_scene->m_blinkStateFlag)
            return;
    }

    PGE_RectF tPos = m_animator.curFrame();
    PGE_PointF Ofs = m_animator.curOffset();

    PGE_RectF player;
    player.setRect(round(posX() - camX) - Ofs.x(),
                   round(posY() - Ofs.y()) - camY,
                   m_frameW,
                   m_frameH
                  );

    if(m_isWarping)
    {
        if(m_warpPipeOffset >= 1.0)
            return;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(m_warpDirectO)
        {
        case 2://Left entrance, right Exit
        {
            double wOfs = Ofs.x() / m_warpFrameW; //Relative X offset
            double wOfsF = m_width_registered / m_warpFrameW; //Relative hitbox width
            tPos.setLeft(tPos.left() + wOfs + (m_warpPipeOffset * wOfsF));
            player.setLeft(player.left() + Ofs.x());
            player.setRight(player.right() - (m_warpPipeOffset * m_width_registered));
        }
        break;
        case 1://Up entrance, down exit
        {
            double hOfs = Ofs.y() / m_warpFrameH; //Relative Y offset
            double hOfsF = m_height_registered / m_warpFrameH; //Relative hitbox Height
            tPos.setTop(tPos.top() + hOfs + (m_warpPipeOffset * hOfsF));
            player.setTop(player.top() + Ofs.y());
            player.setBottom(player.bottom() - (m_warpPipeOffset * m_height_registered));
        }
        break;
        case 4://right emtramce. left exit
        {
            double wOfs =  Ofs.x() / m_warpFrameW;             //Relative X offset
            double fWw =   m_animator.sizeOfFrame().w();   //Relative width of frame
            double wOfHB = m_width_registered / m_warpFrameW;               //Relative width of hitbox
            double wWAbs = m_warpFrameW * fWw;                 //Absolute width of frame
            tPos.setRight(tPos.right() - (fWw - wOfHB - wOfs) - (m_warpPipeOffset * wOfHB));
            player.setLeft(player.left() + (m_warpPipeOffset * m_width_registered));
            player.setRight(player.right() - (wWAbs - Ofs.x() - m_width_registered));
        }
        break;
        case 3://down entrance, up exit
        {
            double hOfs =  Ofs.y() / m_warpFrameH;             //Relative Y offset
            double fHh =   m_animator.sizeOfFrame().h();  //Relative height of frame
            double hOfHB = m_height_registered / m_warpFrameH;              //Relative height of hitbox
            double hHAbs = m_warpFrameH * fHh;                 //Absolute height of frame
            tPos.setBottom(tPos.bottom() - (fHh - hOfHB - hOfs) - (m_warpPipeOffset * hOfHB));
            player.setTop(player.top() + (m_warpPipeOffset * m_height_registered));
            player.setBottom(player.bottom() - (hHAbs - Ofs.y() - m_height_registered));
        }
        break;
        default:
            break;
        }
    }

    GlRenderer::renderTexture(&texture,
                              float(player.x()),
                              float(player.y()),
                              float(player.width()),
                              float(player.height()),
                              float(tPos.top()),
                              float(tPos.bottom()),
                              float(tPos.left()),
                              float(tPos.right()));

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(fmt::format(" {0} \n"
                                           "{1}{2}{3}\n"
                                           " {4} {5}",
                                           l_contactT.size(),
                                           l_contactL.size(), l_contactAny.size(), l_contactR.size(),
                                           l_contactB.size(), (m_slopeFloor.has ? "slope!" : ""))
                               //.arg(m_speedAddingTopElements.size())
                               //.arg(m_speedAddingBottomElements.size())
                               , int(round(20 + posX() - camX)), -50 + int(round(posY() - camY)), 3);
    }

}
