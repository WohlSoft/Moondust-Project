/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"
#include <common_features/logger.h>
#include <Utils/files.h>
#include <Utils/elapsed_timer.h>
#include <DirManager/dirman.h>

#include "../../networking/intproc.h"

bool LevelScene::loadFile(std::string filePath)
{
    m_data.meta.ReadFileValid = false;

    if(!Files::fileExists(filePath))
    {
        m_errorMsg += "File not exist\n\n";
        m_errorMsg += filePath;
        return false;
    }

    if(!FileFormats::OpenLevelFile(filePath, m_data))
        m_errorMsg += "Bad file format\n";

    return m_data.meta.ReadFileValid;
}


bool LevelScene::loadFileIP()
{
    if(!IntProc::isEnabled()) return false;

    FileFormats::CreateLevelData(m_data);
    m_data.meta.ReadFileValid = false;
    pLogDebug("ICP: Requesting editor for a file....");
    IntProc::sendMessage("CMD:CONNECT_TO_ENGINE");
    ElapsedTimer time;
    time.start();
    //wait for accepting of level data
    bool timeOut = false;
    int attempts = 0;
    pLogDebug("ICP: Waiting reply....");

    while(!IntProc::editor->levelIsLoad())
    {
        loaderStep();

        //Abort loading process and exit from game if window was closed
        if(!m_isLevelContinues)
            return false;

        if(time.elapsed() > 1500)
        {
            pLogDebug("ICP: Waiting #%d....", attempts);
            time.restart();
            attempts += 1;
        }

        if(attempts > 4)
        {
            m_errorMsg += "Wait timeout\n";
            timeOut = true;
            break;
        }

        SDL_Delay(30);
    }

    m_data = IntProc::editor->m_acceptedLevel;

    if(!timeOut && !m_data.meta.ReadFileValid)
        m_errorMsg += "Bad file format\n";

    pLogDebug("ICP: Done, starting a game....");
    IntProc::setState("Done. Starting game...");
    return m_data.meta.ReadFileValid;
}
