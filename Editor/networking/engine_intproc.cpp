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

#include "engine_intproc.h"
#include <PGE_File_Formats/file_formats.h>
#include <common_features/main_window_ptr.h>
#include <common_features/app_path.h>
#include <QSharedPointer>

static QString base64_encode(QString string){
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}

IntEngine::IntEngine()
{}

IntEngine::~IntEngine()
{}

void IntEngine::onData()
{
    if(engine)
    {
        QByteArray strdata = engine->readAllStandardOutput();
        QString msg=QByteArray::fromBase64(strdata);
        if(msg.startsWith("CMD:"))
        {
            msg.remove("CMD:");
            if("CONNECT_TO_ENGINE"==msg)
            {
                sendLevelBuffer();
            } else if("ENGINE_CLOSED"==msg) {
                MainWinConnect::pMainWin->show();
                MainWinConnect::pMainWin->raise();
            }
            qDebug() << "ENGINE MESSAGE: >>\n" << msg << "\n<<ENGINE MESSAGE END";
        }
        else
        {
            emit engineInputMsg(msg);
        }
    }
}

LevelData IntEngine::testBuffer;
QProcess *IntEngine::engine=NULL;

QSharedPointer<IntEngine> IntEngine_helper(NULL);

void IntEngine::init(QProcess *engine_proc)
{
    if(IntEngine_helper.isNull())
    {
        IntEngine_helper = QSharedPointer<IntEngine>(new IntEngine);
    }
    if(!engine)
    {
        engine = engine_proc;
        engine->connect(engine, SIGNAL(readyReadStandardOutput()),
                        &(*IntEngine_helper), SLOT(onData()));
    }
}

void IntEngine::quit()
{
    FileFormats::CreateLevelData(testBuffer);
    if(engine)
    {
        engine->close();
    }
    engine=NULL;
}

bool IntEngine::isWorking()
{
    bool isRuns=false;
    isRuns = (engine!=NULL); if(!isRuns) return false;
    isRuns = (engine->state()==QProcess::Running);
    return isRuns;
}

bool IntEngine::sendCheat(QString _args)
{
    if(engine) {
        if(_args.isEmpty()) return false;
        _args=_args.replace('\n', "\\n");
        return sendMessage(QString("CHEAT: %1").arg(_args));
    }
    else
        return false;
}

bool IntEngine::sendMessageBox(QString _args)
{
    if(engine) {
        if(_args.isEmpty()) return false;
        _args=_args.replace('\n', "\\n");
        return sendMessage(QString("MSGBOX: %1").arg(_args));
    }
    else
        return false;
}

bool IntEngine::sendItemPlacing(QString _args)
{
    if(engine) {
        qDebug() << "ENGINE: Place item command: " << _args;
        bool answer=sendMessage(QString("PLACEITEM: %1").arg(_args));
        return answer;
    }
    return false;
}

void IntEngine::sendLevelBuffer()
{
    if(engine)
    {
        qDebug() << "Attempt to send LVLX buffer";
        QString output;
        FileFormats::WriteExtendedLvlFileRaw(testBuffer, output);

        QString sendLvlx;
        if(!testBuffer.path.isEmpty())
            sendLvlx = QString("SEND_LVLX: %1/%2\n")
                    .arg(testBuffer.path)
                    .arg(testBuffer.filename+".lvlx");
        else
            sendLvlx = QString("SEND_LVLX: %1/%2\n")
                    .arg(ApplicationPath)
                    .arg("_untitled.lvlx");
        if(output.size()<=0)
        {
            output="HEAD\nEMPTY:1\nHEAD_END\n";
        }
        #ifdef DEBUG_BUILD
        qDebug()<<"Sent File data BEGIN >>>>>>>>>>>\n" << output << "\n<<<<<<<<<<<<Sent File data END";
        #endif

        sendMessage(sendLvlx);
        QString output_e = base64_encode(output)+"\n";
        engine->write(output_e.toUtf8());
        sendMessage("PARSE_LVLX");
        qDebug() << "LVLX buffer sent";
    }
}

void IntEngine::setTestLvlBuffer(LevelData &buffer)
{
    testBuffer = buffer;
    testBuffer.metaData.script.reset(); //avoid editor crash after file reloading
}

bool IntEngine::sendMessage(QString msg)
{
    QString output_e = base64_encode(msg)+"\n";
    return (engine->write(output_e.toUtf8())>0);
}
