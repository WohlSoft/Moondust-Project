/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../lvl_scene.h"
#include "../items/item_block.h"
#include "../items/item_water.h"
#include "../lvl_item_placing.h"

void LvlScene::setPhysEnvResizer(QGraphicsItem * targetRect, bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = ((ItemPhysEnv *)targetRect)->m_data.x;
        int y = ((ItemPhysEnv *)targetRect)->m_data.y;
        int w = ((ItemPhysEnv *)targetRect)->m_data.w;
        int h = ((ItemPhysEnv *)targetRect)->m_data.h;

        pResizer = new ItemResizer( QSize(w, h), Qt::darkYellow, 16 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=3;
        pResizer->targetItem = targetRect;
        pResizer->_minSize = QSizeF(16, 16);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        //MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        SwitchEditingMode(MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("Water RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = pResizer->pos().x();
                long y = pResizer->pos().y();
                long w = pResizer->_width;
                long h = pResizer->_height;
                long oldX = ((ItemPhysEnv *)pResizer->targetItem)->m_data.x;
                long oldY = ((ItemPhysEnv *)pResizer->targetItem)->m_data.y;
                long oldW = ((ItemPhysEnv *)pResizer->targetItem)->m_data.w;
                long oldH = ((ItemPhysEnv *)pResizer->targetItem)->m_data.h;
                ((ItemPhysEnv *)pResizer->targetItem)->m_data.x = x;
                ((ItemPhysEnv *)pResizer->targetItem)->m_data.y = y;
                ((ItemPhysEnv *)pResizer->targetItem)->m_data.w = w;
                ((ItemPhysEnv *)pResizer->targetItem)->m_data.h = h;

                ((ItemPhysEnv *)pResizer->targetItem)->setRectSize( QRect(x,y,w,h) );
                LvlData->modified = true;

                addResizeWaterHistory(((ItemPhysEnv *)pResizer->targetItem)->m_data, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}
