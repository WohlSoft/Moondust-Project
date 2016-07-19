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

#include <mainwindow.h>
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../lvl_history_manager.h"
#include "../lvl_item_placing.h"

void LvlScene::setBlockResizer(QGraphicsItem * targetBlock, bool enabled, bool accept)
{
    if((enabled)&&(m_resizeBox==NULL))
    {
        m_mw->on_actionSelect_triggered(); //Reset mode

        int x = ((ItemBlock *)targetBlock)->m_data.x;
        int y = ((ItemBlock *)targetBlock)->m_data.y;
        int w = ((ItemBlock *)targetBlock)->m_data.w;
        int h = ((ItemBlock *)targetBlock)->m_data.h;

        m_resizeBox = new ItemResizer( QSize(w, h), Qt::blue, 32 );
        this->addItem(m_resizeBox);
        m_resizeBox->setPos(x, y);
        m_resizeBox->type=2;
        m_resizeBox->targetItem = targetBlock;
        m_resizeBox->_minSize = QSizeF(64, 64);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        //MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        SwitchEditingMode(MODE_Resizing);
        m_mw->resizeToolbarVisible(true);
    }
    else
    {
        if( m_resizeBox != nullptr )
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("BLOCK RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = m_resizeBox->pos().x();
                long y = m_resizeBox->pos().y();
                long w = m_resizeBox->_width;
                long h = m_resizeBox->_height;
                long oldX = ((ItemBlock *)m_resizeBox->targetItem)->m_data.x;
                long oldY = ((ItemBlock *)m_resizeBox->targetItem)->m_data.y;
                long oldW = ((ItemBlock *)m_resizeBox->targetItem)->m_data.w;
                long oldH = ((ItemBlock *)m_resizeBox->targetItem)->m_data.h;
                ((ItemBlock *)m_resizeBox->targetItem)->m_data.x = x;
                ((ItemBlock *)m_resizeBox->targetItem)->m_data.y = y;
                ((ItemBlock *)m_resizeBox->targetItem)->m_data.w = w;
                ((ItemBlock *)m_resizeBox->targetItem)->m_data.h = h;

                ((ItemBlock *)m_resizeBox->targetItem)->setBlockSize( QRect(x,y,w,h) );
                m_data->modified = true;

                m_history->addResizeBlock(((ItemBlock *)m_resizeBox->targetItem)->m_data, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
            }
            delete m_resizeBox;
            m_resizeBox = nullptr;
            m_mw->on_actionSelect_triggered();
            m_mw->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        m_busyMode=false;
    }
}
