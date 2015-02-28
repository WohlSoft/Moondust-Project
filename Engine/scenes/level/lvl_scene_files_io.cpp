/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QApplication>

#include "../../networking/intproc.h"

bool LevelScene::loadFile(QString filePath)
{
    data.ReadFileValid = false;
    if(!QFileInfo(filePath).exists())
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    data = FileFormats::OpenLevelFile(filePath);
    if(!data.ReadFileValid)
        errorMsg += "Bad file format\n";
    return data.ReadFileValid;
}


bool LevelScene::loadFileIP()
{
    if(!IntProc::isEnabled()) return false;
    if(!IntProc::isWorking()) return false;

    data.ReadFileValid = false;

    if(!IntProc::editor->sendToEditor("CMD:CONNECT_TO_ENGINE"))
    {
        errorMsg += "Editor is not started!\n";
        return false;
    }

    QElapsedTimer time;
    time.start();
    //wait for accepting of level data
    bool timeOut=false;

    while(!IntProc::editor->levelIsLoad())
    {
        loaderStep();
        //Abort loading process and exit from game if window was closed
        if(!isLevelContinues) return false;

        qApp->processEvents();
        if(time.elapsed()>10000)
        {
            errorMsg += "Wait timeout\n";
            timeOut=true;
            break;
        }
        SDL_Delay(30);
    }

    data = IntProc::editor->accepted_lvl;

    if(!timeOut && !data.ReadFileValid)
        errorMsg += "Bad file format\n";

    IntProc::state="Done. Starting game...";

    return data.ReadFileValid;
}

