/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QtMath>

#include <common_features/mainwinconnect.h>
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

        int x = ((ItemWater *)targetRect)->waterData.x;
        int y = ((ItemWater *)targetRect)->waterData.y;
        int w = ((ItemWater *)targetRect)->waterData.w;
        int h = ((ItemWater *)targetRect)->waterData.h;

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
                long oldX = ((ItemWater *)pResizer->targetItem)->waterData.x;
                long oldY = ((ItemWater *)pResizer->targetItem)->waterData.y;
                long oldW = ((ItemWater *)pResizer->targetItem)->waterData.w;
                long oldH = ((ItemWater *)pResizer->targetItem)->waterData.h;
                ((ItemWater *)pResizer->targetItem)->waterData.x = x;
                ((ItemWater *)pResizer->targetItem)->waterData.y = y;
                ((ItemWater *)pResizer->targetItem)->waterData.w = w;
                ((ItemWater *)pResizer->targetItem)->waterData.h = h;

                ((ItemWater *)pResizer->targetItem)->setRectSize( QRect(x,y,w,h) );
                LvlData->modified = true;

                addResizeWaterHistory(((ItemWater *)pResizer->targetItem)->waterData, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

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
