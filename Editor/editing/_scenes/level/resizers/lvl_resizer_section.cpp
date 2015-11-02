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

#include <common_features/mainwinconnect.h>
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../lvl_scene.h"
#include "../items/item_block.h"
#include "../items/item_water.h"
#include "../lvl_item_placing.h"



void LvlScene::setSectionResizer(bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = LvlData->sections[LvlData->CurSection].size_left;
        int y = LvlData->sections[LvlData->CurSection].size_top;
        int w = LvlData->sections[LvlData->CurSection].size_right;
        int h = LvlData->sections[LvlData->CurSection].size_bottom;

        pResizer = new ItemResizer( QSize(abs(x-w), abs(y-h)), Qt::green, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=0;
        pResizer->_minSize = QSizeF(800, 600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
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
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                long oldL = LvlData->sections[LvlData->CurSection].size_left;
                long oldR = LvlData->sections[LvlData->CurSection].size_right;
                long oldT = LvlData->sections[LvlData->CurSection].size_top;
                long oldB = LvlData->sections[LvlData->CurSection].size_bottom;
                LvlData->sections[LvlData->CurSection].size_left = l;
                LvlData->sections[LvlData->CurSection].size_right = r;
                LvlData->sections[LvlData->CurSection].size_top = t;
                LvlData->sections[LvlData->CurSection].size_bottom = b;

                addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                drawSpace();
                LvlData->modified = true;
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

