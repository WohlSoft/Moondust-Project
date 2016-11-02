/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "msgbox_queue.h"

#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>

void MessageBoxQueue::showMsg(const std::string &message)
{
    messages.push_back(QString::fromStdString(message));
}

void MessageBoxQueue::showMsg(const QString &message)
{
    messages.push_back(message);
}

void MessageBoxQueue::process()
{
    while(!messages.empty())
    {
        QString msg = messages.front();
        PGE_MsgBox box(m_scene, msg,
                       PGE_MsgBox::msg_info, PGE_Point(-1, -1),
                       ConfigManager::setup_message_box.box_padding,
                       ConfigManager::setup_message_box.sprite);
        box.exec();
        messages.pop_front();
    }
}
