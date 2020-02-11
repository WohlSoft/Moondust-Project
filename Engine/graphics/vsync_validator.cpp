/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <graphics/window.h>
#include <settings/global_settings.h>
#include <gui/pge_msgbox.h>
#include <common_features/tr.h>
#include <data_configs/config_manager.h>

#include "vsync_validator.h"

VSyncValidator::VSyncValidator(Scene *parent, double tickDelay) :
    m_parentScene(parent)
{
    m_timeLimit = 120;
    m_maxStepsAllowed = static_cast<int>((1000.0 / tickDelay) * (m_timeLimit / 1000.0)) + 20;
    m_testDone = !PGE_Window::vsync;
}

bool VSyncValidator::isComplete()
{
    return m_testDone;
}

void VSyncValidator::update()
{
    if(m_testDone)
        return; // Do nothing

    if(m_prevTick == 0)
    {
        m_prevTick = SDL_GetTicks();
        return;// Skip this moment
    }

    Uint32 curTick = SDL_GetTicks();
    Uint32 tickDiff = (curTick - m_prevTick);

    m_steps++;
    if(curTick >= m_prevTick)
        m_timeLimit -= tickDiff;
    m_prevTick = curTick;

    if(m_timeLimit <= 0)
    {
        m_testDone = true;
        m_testPassed = (m_steps <= m_maxStepsAllowed);
        if(!m_testPassed)
        {
            PGE_Window::vsync = false;
            PGE_Window::toggleVSync(g_AppSettings.vsync);
            g_AppSettings.vsync = false;
            g_AppSettings.timeOfFrame = PGE_Window::frameDelay;
            g_AppSettings.apply();
            g_AppSettings.save();

            PGE_MsgBox msg2(m_parentScene,
                            /*% "Detected framerate is too high. "
                                "V-Sync will be disabled to prevent problematic results. "
                                "Please enable V-Sync in your video driver." */
                            qtTrId("VSYNC_ERROR_INFO"),
                            PGE_BoxBase::msg_info, PGE_Point(-1, -1),
                            ConfigManager::setup_message_box.box_padding,
                            ConfigManager::setup_message_box.sprite
                           );
            msg2.exec();
        }
    }
}
