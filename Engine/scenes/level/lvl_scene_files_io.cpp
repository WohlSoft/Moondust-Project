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

#include "../scene_level.h"
#include <common_features/logger.h>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QApplication>

#include "../../networking/intproc.h"

bool LevelScene::loadFile(QString filePath)
{
    data.meta.ReadFileValid = false;
    if(!QFileInfo(filePath).exists())
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    if( !FileFormats::OpenLevelFile(filePath, data) )
        errorMsg += "Bad file format\n";

    return data.meta.ReadFileValid;
}


bool LevelScene::loadFileIP()
{
    if(!IntProc::isEnabled()) return false;

    data.meta.ReadFileValid = false;

    LogDebug("ICP: Requesting editor for a file....");

    if(!IntProc::editor->sendToEditor("CMD:CONNECT_TO_ENGINE"))
    {
        errorMsg += "Editor is not started!\n";
        return false;
    }

    QElapsedTimer time;
    time.start();
    //wait for accepting of level data
    bool timeOut=false;
    int attempts=0;

    LogDebug("ICP: Waiting reply....");

    while(!IntProc::editor->levelIsLoad())
    {
        loaderStep();
        //Abort loading process and exit from game if window was closed
        if(!isLevelContinues) return false;
        //#ifndef __APPLE__
        //qApp->processEvents();
        //#endif

        if(time.elapsed()>1500)
        {
            LogDebug(QString("ICP: Waiting #%1....").arg(attempts));
            time.restart();
            attempts+=1;
        }

        if(attempts>4)
        {
            errorMsg += "Wait timeout\n";
            timeOut=true;
            break;
        }
        SDL_Delay(30);
    }

    data = IntProc::editor->accepted_lvl;

    if(!timeOut && !data.meta.ReadFileValid)
        errorMsg += "Bad file format\n";

    LogDebug("ICP: Done, starting a game....");

    IntProc::setState("Done. Starting game...");

    return data.meta.ReadFileValid;
}

