/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "engine_intproc.h"
#include <file_formats/file_formats.h>
#include <common_features/mainwinconnect.h>

EngineClient * IntEngine::engineSocket=NULL;
IntEngine *IntEngine_protector=NULL;

IntEngine::IntEngine()
{}

IntEngine::~IntEngine()
{}


LevelData IntEngine::testBuffer;

void IntEngine::init(LevelData *lvlData)
{
    testBuffer = FileFormats::dummyLvlDataArray();

    if(!IntEngine_protector)
    {
        IntEngine_protector = new IntEngine;
        connect(MainWinConnect::pMainWin, SIGNAL(destroyed()), IntEngine_protector, SLOT(destroyEngine()));
    }

    if(lvlData)
        testBuffer = (*lvlData);

    qDebug() << "INIT Interprocessing...";
    if(!isWorking())
    {
        qDebug() << "Installing new engine socket";
        if(!engineSocket) engineSocket = new EngineClient();
        qDebug() << "Starting session";
        engineSocket->closeConnection();
        engineSocket->OpenConnection();
        engineSocket->start();
        qDebug() << "done";
    }
}

void IntEngine::quit()
{
    if(isWorking())
    {
        engineSocket->closeConnection();
        engineSocket->quit();
        engineSocket->wait(100);
        engineSocket->terminate();
        //delete engineSocket;
        //engineSocket = NULL;
    }
}

bool IntEngine::isWorking()
{
    bool isRuns=false;
    isRuns = ((engineSocket!=NULL) && (engineSocket->isRunning()));
    return isRuns;
}

void IntEngine::setTestLvlBuffer(LevelData &buffer)
{
    testBuffer = buffer;
}

void IntEngine::destroyEngine()
{
    if(engineSocket)
    {
        delete engineSocket;
        engineSocket = NULL;
    }
}
