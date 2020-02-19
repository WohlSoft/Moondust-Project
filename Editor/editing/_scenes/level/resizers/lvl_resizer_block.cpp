/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

void LvlScene::setBlockResizer(QGraphicsItem *targetBlock, bool enabled, bool accept)
{
    if((enabled) && (m_resizeBox == NULL))
    {
        m_mw->on_actionSelect_triggered(); //Reset mode
        ItemBlock *blk = dynamic_cast<ItemBlock *>(targetBlock);

        int x = (int)blk->m_data.x;
        int y = (int)blk->m_data.y;
        int w = (int)blk->m_data.w;
        int h = (int)blk->m_data.h;
        int gridSize = blk->m_localProps.setup.grid;

        if(m_opts.grid_override)
            gridSize = m_opts.customGrid.width();

        m_resizeBox = new ItemResizer(QSize(w, h), Qt::blue, gridSize);
        this->addItem(m_resizeBox);
        m_resizeBox->setPos(x, y);
        m_resizeBox->m_resizerType = ItemResizer::Resizer_Item;
        m_resizeBox->m_targetItem = targetBlock;
        m_resizeBox->m_minSize = QSizeF(64, 64);
        this->setFocus(Qt::ActiveWindowFocusReason);
        SwitchEditingMode(MODE_Resizing);
        m_mw->resizeToolbarVisible(true);
    }
    else
    {
        if(m_resizeBox != nullptr)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("BLOCK RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                ItemBlock *blk = dynamic_cast<ItemBlock *>(m_resizeBox->m_targetItem);
                long x = (long)m_resizeBox->pos().x();
                long y = (long)m_resizeBox->pos().y();
                long w = (long)m_resizeBox->m_width;
                long h = (long)m_resizeBox->m_height;
                long oldX = blk->m_data.x;
                long oldY = blk->m_data.y;
                long oldW = blk->m_data.w;
                long oldH = blk->m_data.h;
                blk->m_data.x = x;
                blk->m_data.y = y;
                blk->m_data.w = w;
                blk->m_data.h = h;

                blk->setBlockSize(QRect((int)x, (int)y, (int)w, (int)h));
                m_data->meta.modified = true;

                m_history->addResizeBlock(blk->m_data, oldX, oldY, oldX + oldW, oldY + oldH, x, y, x + w, y + h);
            }
            delete m_resizeBox;
            m_resizeBox = nullptr;
            m_mw->on_actionSelect_triggered();
            m_mw->resizeToolbarVisible(false);
        }
        m_busyMode = false;
    }
}
