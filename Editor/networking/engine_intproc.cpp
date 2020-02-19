/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/logger.h>
#include <QSharedPointer>

static void base64_encode(QByteArray &out, const char *msg)
{
    QByteArray ba(msg);
    out = ba.toBase64();
}

static void base64_encode(QByteArray &out, QString &string)
{
    QByteArray ba(string.toUtf8());
    out = ba.toBase64();
}

IntEngine::IntEngine()
{}

void IntEngine::onData()
{
    if(isWorking())
    {
        QByteArray strData = engine->readAllStandardOutput();
        QByteArray msg = QByteArray::fromBase64(strData);

        if(msg.startsWith("CMD:"))
        {
            char *msgP = msg.data() + 4; //"CMD:"
            LogDebug("ENGINE COMMAND: >>");
            LogDebug(msgP);
            LogDebug("<<ENGINE COMMAND END");

            if(strcmp(msgP, "CONNECT_TO_ENGINE") == 0)
                sendLevelBuffer();
            else if(strcmp(msgP, "ENGINE_CLOSED") == 0)
            {
                MainWinConnect::pMainWin->show();
                MainWinConnect::pMainWin->raise();
            }
        }
        else
            emit engineInputMsg(QString::fromUtf8(msg));
    }
}

LevelData IntEngine::testBuffer;
QProcess *IntEngine::engine = nullptr;

static QSharedPointer<IntEngine> IntEngine_helper(nullptr);

void IntEngine::init(QProcess *engine_proc)
{
    if(IntEngine_helper.isNull())
        IntEngine_helper = QSharedPointer<IntEngine>(new IntEngine);

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
        engine->close();

    engine = nullptr;
}

bool IntEngine::isWorking()
{
    return ((engine != nullptr) && (engine->state() == QProcess::Running));
}

bool IntEngine::sendCheat(QString _args)
{
    if(isWorking())
    {
        if(_args.isEmpty())
            return false;

        _args.replace('\n', "\\n");
        QString out = QString("CHEAT: %1").arg(_args);
        return sendMessage(out);
    }
    else
        return false;
}

bool IntEngine::sendMessageBox(QString _args)
{
    if(isWorking())
    {
        if(_args.isEmpty())
            return false;

        _args.replace('\n', "\\n");
        QString out = QString("MSGBOX: %1").arg(_args);
        return sendMessage(out);
    }
    else
        return false;
}

bool IntEngine::sendItemPlacing(QString _args)
{
    if(isWorking())
    {
        LogDebug("ENGINE: Place item command: " + _args);
        QString out = QString("PLACEITEM: %1").arg(_args);
        bool ret = sendMessage(out);
        return ret;
    }

    return false;
}

void IntEngine::sendLevelBuffer()
{
    if(isWorking())
    {
        LogDebug("Attempt to send LVLX buffer");
        QString output;
        FileFormats::WriteExtendedLvlFileRaw(testBuffer, output);
        QString sendLvlx;

        if(!testBuffer.meta.path.isEmpty())
            sendLvlx = QString("SEND_LVLX: %1/%2\n")
                       .arg(testBuffer.meta.path)
                       .arg(testBuffer.meta.filename + ".lvlx");
        else
            sendLvlx = QString("SEND_LVLX: %1/%2\n")
                       .arg(ApplicationPath)
                       .arg("_untitled.lvlx");

        if(output.size() <= 0)
            output = "HEAD\nEMPTY:1\nHEAD_END\n";

        //#ifdef DEBUG_BUILD
        //        qDebug() << "Sent File data BEGIN >>>>>>>>>>>\n" << output << "\n<<<<<<<<<<<<Sent File data END";
        //#endif
        sendMessage(sendLvlx);
        QByteArray output_e;
        base64_encode(output_e, output);
        output_e.append('\n');
        engine->write(output_e);
        sendMessage("PARSE_LVLX");
        LogDebug("LVLX buffer sent");
    }
}

void IntEngine::setTestLvlBuffer(LevelData &buffer)
{
    testBuffer = buffer;
}

bool IntEngine::sendMessage(const char *msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e;
    base64_encode(output_e, msg);
    output_e.append('\n');
    return (engine->write(output_e) > 0);
}


bool IntEngine::sendMessage(QString &msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e;
    base64_encode(output_e, msg);
    output_e.append('\n');
    return (engine->write(output_e) > 0);
}
