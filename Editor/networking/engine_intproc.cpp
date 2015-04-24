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
#include <PGE_File_Formats/file_formats.h>
#include <common_features/mainwinconnect.h>

EngineClient * IntEngine::engineSocket=NULL;
IntEngine *IntEngine_protector=NULL;

IntEngine::IntEngine()
{
    qRegisterMetaType<QAbstractSocket::SocketState> ("QAbstractSocket::SocketState");
}

IntEngine::~IntEngine()
{}

LevelData IntEngine::testBuffer;

void IntEngine::init()
{
    //testBuffer = FileFormats::dummyLvlDataArray();

    if(!IntEngine_protector)
    {
        IntEngine_protector = new IntEngine;
        connect(MainWinConnect::pMainWin, SIGNAL(destroyed()), IntEngine_protector, SLOT(destroyEngine()));
    }

    //if(lvlData)
    //    testBuffer = (*lvlData);

    qDebug() << "INIT Interprocessing...";
    if(!isWorking())
    {
        if(!engineSocket)
        {
            qDebug() << "Installing new engine socket";
            engineSocket = new EngineClient();
            engineSocket->setParent(0);
        }
        qDebug() << "Starting session";
        engineSocket->start();
        qDebug() << "done";
    }
}

void IntEngine::quit()
{
    testBuffer = FileFormats::dummyLvlDataArray();

    qDebug() << "isWorking check";
    if(isWorking())
    {
        while(engineSocket->isConnected());
        qDebug() << "exit";
        engineSocket->exit(1000);
        engineSocket->terminate();
        qDebug() << "delete";
        delete engineSocket;
        qDebug() << "done!";
        engineSocket = NULL;
    }
}

bool IntEngine::isWorking()
{
    bool isRuns=false;
    isRuns = (engineSocket!=NULL); if(!isRuns) return false;
    isRuns = (engineSocket->isRunning());
    return isRuns;
}

bool IntEngine::sendCheat(QString _args)
{
    if(isWorking())
    {
        return engineSocket->sendCommand(QString("CHEAT: %1\n\n").arg(_args));
    }
    else
        return false;
}

void IntEngine::sendLevelBuffer()
{
    if(isWorking())
    {
        engineSocket->doSendData=true;
    }
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
