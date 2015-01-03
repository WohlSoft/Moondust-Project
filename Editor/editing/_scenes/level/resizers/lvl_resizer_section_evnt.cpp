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
#include <file_formats/file_formats.h>

#include "../lvl_scene.h"
#include "../items/item_block.h"
#include "../items/item_water.h"
#include "../lvl_item_placing.h"

static long eventID=0;
void LvlScene::setEventSctSizeResizer(long event, bool enabled, bool accept)
{
    if(event>=0)
        eventID=event;

    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
        int y = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
        int w = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
        int h = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::yellow, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=1;
        pResizer->_minSize = QSizeF(800, 600);
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
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                //long oldL = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
                //long oldR = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
                //long oldT = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
                //long oldB = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;
                QList<QVariant> sizeData;
                sizeData.push_back((qlonglong)LvlData->CurSection);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_top);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_right);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_bottom);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_left);
                sizeData.push_back((qlonglong)t);
                sizeData.push_back((qlonglong)r);
                sizeData.push_back((qlonglong)b);
                sizeData.push_back((qlonglong)l);
                addChangeEventSettingsHistory(LvlData->events[eventID].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

                LvlData->events[eventID].sets[LvlData->CurSection].position_left = l;
                LvlData->events[eventID].sets[LvlData->CurSection].position_right = r;
                LvlData->events[eventID].sets[LvlData->CurSection].position_top = t;
                LvlData->events[eventID].sets[LvlData->CurSection].position_bottom = b;

                //addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
                LvlData->modified = true;
                MainWinConnect::pMainWin->eventSectionSettingsSync();
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


