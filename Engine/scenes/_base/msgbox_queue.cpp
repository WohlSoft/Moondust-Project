/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "msgbox_queue.h"

#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>

void MessageBoxQueue::showMsg(const std::string &message)
{
    messages.push_back(message);
}

void MessageBoxQueue::process()
{
    while(!messages.empty())
    {
        std::string msg = messages.front();
        PGE_MsgBox box(m_scene, msg,
                       PGE_MsgBox::msg_info, PGE_Point(-1, -1),
                       ConfigManager::setup_message_box.box_padding,
                       ConfigManager::setup_message_box.sprite);
        box.exec();
        messages.pop_front();
    }
}
