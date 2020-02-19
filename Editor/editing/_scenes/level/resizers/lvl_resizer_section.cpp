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



void LvlScene::setSectionResizer(bool enabled, bool accept)
{
    if((enabled) && (m_resizeBox == nullptr))
    {
        m_mw->on_actionSelect_triggered(); //Reset mode

        int l = (int)m_data->sections[m_data->CurSection].size_left;
        int t = (int)m_data->sections[m_data->CurSection].size_top;
        int r = (int)m_data->sections[m_data->CurSection].size_right;
        int b = (int)m_data->sections[m_data->CurSection].size_bottom;
        int gridSize = 32;

        if(m_opts.grid_override)
            gridSize = m_opts.customGrid.width();

        m_resizeBox = new ItemResizer(QSize(abs(l - r), abs(t - b)), Qt::green, gridSize);
        this->addItem(m_resizeBox);
        m_resizeBox->setPos(l, t);
        m_resizeBox->m_resizerType = ItemResizer::Resizer_Section;
        m_resizeBox->m_minSize = QSizeF(800, 600);
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
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = (long)m_resizeBox->pos().x();
                long t = (long)m_resizeBox->pos().y();
                long r = l + (long)m_resizeBox->m_width;
                long b = t + (long)m_resizeBox->m_height;
                long oldL = m_data->sections[m_data->CurSection].size_left;
                long oldR = m_data->sections[m_data->CurSection].size_right;
                long oldT = m_data->sections[m_data->CurSection].size_top;
                long oldB = m_data->sections[m_data->CurSection].size_bottom;
                m_data->sections[m_data->CurSection].size_left = l;
                m_data->sections[m_data->CurSection].size_right = r;
                m_data->sections[m_data->CurSection].size_top = t;
                m_data->sections[m_data->CurSection].size_bottom = b;

                m_history->addResizeSection(m_data->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                ChangeSectionBG((int)m_data->sections[m_data->CurSection].background);
                drawSpace();
                m_data->meta.modified = true;
            }
            delete m_resizeBox;
            m_resizeBox = NULL;
            m_mw->on_actionSelect_triggered();
            m_mw->resizeToolbarVisible(false);
        }
        m_busyMode = false;
    }
}

