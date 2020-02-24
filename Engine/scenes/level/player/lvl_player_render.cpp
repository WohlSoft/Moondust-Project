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

#include "../lvl_player.h"
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <fontman/font_manager.h>
#include <common_features/fmt_format_ne.h>
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
            double wOfsF = m_momentum.w / m_warpFrameW; //Relative hitbox width
            tPos.setLeft(tPos.left() + wOfs + (m_warpPipeOffset * wOfsF));
            player.setLeft(player.left() + Ofs.x());
            player.setRight(player.right() - (m_warpPipeOffset * m_momentum.w));
        }
        break;
        case 1://Up entrance, down exit
        {
            double hOfs = Ofs.y() / m_warpFrameH; //Relative Y offset
            double hOfsF = m_momentum.h / m_warpFrameH; //Relative hitbox Height
            tPos.setTop(tPos.top() + hOfs + (m_warpPipeOffset * hOfsF));
            player.setTop(player.top() + Ofs.y());
            player.setBottom(player.bottom() - (m_warpPipeOffset * m_momentum.h));
        }
        break;
        case 4://right emtramce. left exit
        {
            double wOfs =  Ofs.x() / m_warpFrameW;             //Relative X offset
            double fWw =   m_animator.sizeOfFrame().w();   //Relative width of frame
            double wOfHB = m_momentum.w / m_warpFrameW;               //Relative width of hitbox
            double wWAbs = m_warpFrameW * fWw;                 //Absolute width of frame
            tPos.setRight(tPos.right() - (fWw - wOfHB - wOfs) - (m_warpPipeOffset * wOfHB));
            player.setLeft(player.left() + (m_warpPipeOffset * m_momentum.w));
            player.setRight(player.right() - (wWAbs - Ofs.x() - m_momentum.w));
        }
        break;
        case 3://down entrance, up exit
        {
            double hOfs =  Ofs.y() / m_warpFrameH;             //Relative Y offset
            double fHh =   m_animator.sizeOfFrame().h();  //Relative height of frame
            double hOfHB = m_momentum.h / m_warpFrameH;              //Relative height of hitbox
            double hHAbs = m_warpFrameH * fHh;                 //Absolute height of frame
            tPos.setBottom(tPos.bottom() - (fHh - hOfHB - hOfs) - (m_warpPipeOffset * hOfHB));
            player.setTop(player.top() + (m_warpPipeOffset * m_momentum.h));
            player.setBottom(player.bottom() - (hHAbs - Ofs.y() - m_momentum.h));
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
        FontManager::printText(fmt::format_ne(" {0} \n"
                                           "{1}{2}{3}\n"
                                           " {4} {5}",
                                           l_contactT.size(),
                                           l_contactL.size(), l_contactAny.size(), l_contactR.size(),
                                           l_contactB.size(), (m_slopeFloor.has ? "slope!" : ""))
                               //.arg(m_speedAddingTopElements.size())
                               //.arg(m_speedAddingBottomElements.size())
                               , int(round(20 + posX() - camX)), -50 + int(round(posY() - camY)), FontManager::DefaultRaster);
    }

}
