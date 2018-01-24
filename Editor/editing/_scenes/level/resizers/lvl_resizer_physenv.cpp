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

void LvlScene::setPhysEnvResizer(QGraphicsItem *targetRect, bool enabled, bool accept)
{
    if((enabled) && (m_resizeBox == nullptr))
    {
        m_mw->on_actionSelect_triggered(); //Reset mode
        ItemPhysEnv *env = dynamic_cast<ItemPhysEnv *>(targetRect);

        int x = (int)env->m_data.x;
        int y = (int)env->m_data.y;
        int w = (int)env->m_data.w;
        int h = (int)env->m_data.h;

        m_resizeBox = new ItemResizer(QSize(w, h), Qt::darkYellow, 16);
        this->addItem(m_resizeBox);
        m_resizeBox->setPos(x, y);
        m_resizeBox->type = 3;
        m_resizeBox->targetItem = targetRect;
        m_resizeBox->_minSize = QSizeF(16, 16);
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
                ItemPhysEnv *env = dynamic_cast<ItemPhysEnv *>(m_resizeBox->targetItem);
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("Water RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = (long)m_resizeBox->pos().x();
                long y = (long)m_resizeBox->pos().y();
                long w = (long)m_resizeBox->_width;
                long h = (long)m_resizeBox->_height;
                long oldX = env->m_data.x;
                long oldY = env->m_data.y;
                long oldW = env->m_data.w;
                long oldH = env->m_data.h;
                env->m_data.x = x;
                env->m_data.y = y;
                env->m_data.w = w;
                env->m_data.h = h;

                env->setRectSize(QRect((int)x, (int)y, (int)w, (int)h));
                m_data->meta.modified = true;

                m_history->addResizePhysEnv(env->m_data, oldX, oldY, oldX + oldW, oldY + oldH, x, y, x + w, y + h);
            }
            delete m_resizeBox;
            m_resizeBox = nullptr;
            m_mw->on_actionSelect_triggered();
            m_mw->resizeToolbarVisible(false);
        }
        m_busyMode = false;
    }
}
