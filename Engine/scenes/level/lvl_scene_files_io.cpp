/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"
#include <common_features/logger.h>
#include <Utils/files.h>
#include <Utils/elapsed_timer.h>
#include <DirManager/dirman.h>

#include "../../networking/intproc.h"

bool LevelScene::loadFile(std::string filePath)
{
    data.meta.ReadFileValid = false;

    if(!Files::fileExists(filePath))
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    if(!FileFormats::OpenLevelFile(filePath, data))
        errorMsg += "Bad file format\n";

    return data.meta.ReadFileValid;
}


bool LevelScene::loadFileIP()
{
    if(!IntProc::isEnabled()) return false;

    FileFormats::CreateLevelData(data);
    data.meta.ReadFileValid = false;
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
        if(!isLevelContinues)
            return false;

        if(time.elapsed() > 1500)
        {
            pLogDebug("ICP: Waiting #%d....", attempts);
            time.restart();
            attempts += 1;
        }

        if(attempts > 4)
        {
            errorMsg += "Wait timeout\n";
            timeOut = true;
            break;
        }

        SDL_Delay(30);
    }

    data = IntProc::editor->m_acceptedLevel;

    if(!timeOut && !data.meta.ReadFileValid)
        errorMsg += "Bad file format\n";

    pLogDebug("ICP: Done, starting a game....");
    IntProc::setState("Done. Starting game...");
    return data.meta.ReadFileValid;
}
