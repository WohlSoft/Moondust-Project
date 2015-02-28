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

#include <QElapsedTimer>
#include <QApplication>
#include <QTextBlock>

#include <common_features/app_path.h>
#include <common_features/logger.h>

#include "engine_intproc.h"
#include "engine_client.h"

/**
 * @brief EngineClient::LocalServer
 *  Constructor
 */

bool EngineClient::alreadyRequested=false;

EngineClient::EngineClient() : QThread(NULL)
{
    engine = NULL;
    qRegisterMetaType<QAbstractSocket::SocketState > ("QAbstractSocket::SocketState");
    qRegisterMetaType<QLocalSocket::LocalSocketError > ("QLocalSocket::LocalSocketError");
    qRegisterMetaType<QTextBlock > ("QTextBlock");

    readyToSendLvlx = false;
    _connected = false;
    _busy = false;
    doSendData = false;
}

/**
 * @brief EngineClient::~LocalServer
 *  Destructor
 */
EngineClient::~EngineClient()
{
    if(engine)
        delete engine;
}

void EngineClient::sendLevelData(LevelData _data)
{
    if(!_data.ReadFileValid) return;
    if(!engine) return;

    doSendData = false;

    if(!engine->isOpen())
        OpenConnection();

    QString sendLvlx;
    if(!_data.path.isEmpty())
        sendLvlx = QString("SEND_LVLX: %1/%2\n\n").arg(_data.path).arg(_data.filename);
    else
        sendLvlx = QString("SEND_LVLX: %1/%2\n\n").arg(ApplicationPath).arg("_untitled.lvlx");

    WriteToLog(QtDebugMsg, "Send LVLX data to engine "+sendLvlx, true);
    _busy=true;

    QString rawData = FileFormats::WriteExtendedLvlFile(_data);
    rawData.append("\n");
    _buffer_out = rawData;
    WriteToLog(QtDebugMsg, "Buffer generated, size "+QString::number(_buffer_out.size()), true);

    msleep(20);
    WriteToLog(QtDebugMsg, "sleep 20, sendcommand", true);
    if(!sendCommand(sendLvlx))
    {
        WriteToLog(QtDebugMsg, "Fail to send 'SEND_LVLX' command", true);
        IntEngine::quit();
        WriteToLog(QtDebugMsg, "engine client exited", true);
        return;
    }
    _busy=false;

    WriteToLog(QtDebugMsg, "readyToSendLvlx is true", true);
    readyToSendLvlx=false;
    WriteToLog(QtDebugMsg, "Wait for 'READY'", true);
}

bool EngineClient::sendCommand(QString command)
{
    if(!engine)
        return false;
    if(!_connected)
        return false;
    if(!engine->isOpen())
        return false;

    QElapsedTimer timeout;
    timeout.start();

    //WriteToLog(QtDebugMsg, "do 'while command is not empty'", true);
    while(!command.isEmpty())
    {
        QByteArray bytes;
        QString send;

        if(command.size()>2048)
        {
            send = command.mid(0, 2048);
            command.remove(0, 2048);
        }
        else
        {
            send = command;
            command.clear();
        }
        bytes = send.toUtf8();

        //WriteToLog(QtDebugMsg, "Write", true);
        engine->write(bytes);
        //WriteToLog(QtDebugMsg, "Flush", true);
        engine->flush();
        //WriteToLog(QtDebugMsg, "Wait", true);
        engine->waitForBytesWritten(10000);
        //WriteToLog(QtDebugMsg, "Check for timeout", true);
        if(timeout.elapsed()>5000) break;
    }
    WriteToLog(QtDebugMsg, "done", true);
    return true;
}

void EngineClient::OpenConnection()
{
    if(!engine)
    {
        engine = new QLocalSocket;
        connect(engine, SIGNAL(error(QLocalSocket::LocalSocketError)),
                this, SLOT(displayError(QLocalSocket::LocalSocketError)));
        connect(this, SIGNAL(privateDataReceived(QString)),
                         this, SLOT(slotOnData(QString)));
    }

    _buffer.clear();

    _busy=false;
    if(engine->isOpen())
    {
        qDebug()<<"Connected to "+engine->serverName();
        return;
    }

    qRegisterMetaType<QAbstractSocket::SocketState> ("QAbstractSocket::SocketState");

    WriteToLog(QtDebugMsg, QString("Connect to Engine ")+ENGINE_SERVER_NAME, true);

    engine->connectToServer(QString(ENGINE_SERVER_NAME));
    WriteToLog(QtDebugMsg, "Wait for connection", true);

    if(engine->waitForConnected(20000))
    {
        WriteToLog(QtDebugMsg, "Connected", true);
    }
    else
    {
        WriteToLog(QtDebugMsg, "Failed to connect: "+engine->errorString(), true);
    }
}

void EngineClient::closeConnection()
{
    _busy=false;
    _connected=false;
    doSendData = false;
}

bool EngineClient::isConnected()
{
    return _connected;
}


void EngineClient::run()
{
    //Set default values
    readyToSendLvlx = false;
    _busy = false;
    OpenConnection();
    _connected=true;
    msleep(100);
    alreadyRequested=false;
    exec();
    if(engine)
    {
        disconnect(engine, SIGNAL(error(QLocalSocket::LocalSocketError)),
                this, SLOT(displayError(QLocalSocket::LocalSocketError)));
        disconnect(this, SIGNAL(privateDataReceived(QString)),
                         this, SLOT(slotOnData(QString)));

        delete engine;
        engine = NULL;
    }
    _connected=false;
}

void EngineClient::exec()
{
    while(engine->isOpen() && !this->isFinished() && _connected)
    {
        if(doSendData)
        {
            doSendData=false;
            sendLevelData(IntEngine::testBuffer);
        }

        WriteToLog(QtDebugMsg, "EDITOR: wait for ready to read...", true);
        if(engine->waitForReadyRead())
        {
            QByteArray data = engine->readAll();
            QString acceptedData = QString::fromUtf8(data);
            WriteToLog(QtDebugMsg, "EDITOR: ready!", true);

            emit privateDataReceived(acceptedData);
        }
        WriteToLog(QtDebugMsg, "EDITOR: sleep", true);
        msleep(100);
    }
    engine->close();
    WriteToLog(QtDebugMsg, "EDITOR: Connection with engine was finished", true);
}




/**
 * -------
 * SLOTS
 * -------
 */
void EngineClient::slotOnData(QString data)
{
    unsigned int nc=0;
    foreach(QChar c, data)
    {
        _buffer.append(c);

        if(c==QChar(QChar::LineFeed))
            nc++;
        else
            nc=0;

        if(nc==2)
        {
            _buffer.resize(_buffer.size()-2);
            icomingData(_buffer);
            _buffer.clear();
        }
    }
}

void EngineClient::icomingData(QString in)
{
    //emit dataReceived(in);
    WriteToLog(QtDebugMsg, "EngineClient::icomingData() accepted: "+in, true);

    if(in=="PING")
    {
        QString str = QString("PONG\n\n");
        QByteArray bytes;
        bytes = str.toUtf8();
        engine->write(bytes);
        engine->flush();
    }
    else
    if(in=="PONG")
    {
        WriteToLog(QtDebugMsg, "Accepted PONG", true);
    }
    else
    if(in=="BYE")
    {
        WriteToLog(QtDebugMsg, "Engine was closed", true);
        //engine->disconnectFromServer();
        _connected=false;
    }
    else
    if(in=="READY")
    {
        WriteToLog(QtDebugMsg, "Accepted READY", true);
        if(alreadyRequested)
        {
            WriteToLog(QtDebugMsg, "Duplicated call!", true);
            return;
        }
        alreadyRequested=true;

        if(_buffer_out.size()<=0) _buffer_out="HEAD\nEMPTY:1\nHEAD_END\n\n";

        QFile temp(AppPathManager::userAppDir()+"/._tmp_interprocess.lvlx");
        if(temp.exists())
            temp.remove();
        temp.open(QFile::WriteOnly);
        QTextStream(&temp)<<_buffer_out;
        temp.close();

        if(sendCommand(temp.fileName()+"\n\n"))
            WriteToLog(QtDebugMsg, "LVLX temp path sent Sent", true);
        else
        {
            WriteToLog(QtDebugMsg, "Send fail", true);
            _buffer_out.clear();
            this->closeConnection();
            return;
        }

        msleep(50);
        //if(sendCommand("PARSE_LVLX\n\n"))
        if(sendCommand("OPEN_TEMP_LVLX\n\n"))
            WriteToLog(QtDebugMsg, "OPEN_TEMP_LVLX command Sent", true);
    }
}



void EngineClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        case QLocalSocket::SocketTimeoutError:
            break;
        default:
            WriteToLog(QtDebugMsg, QString("EDITOR: The following error occurred: %1.")
                       .arg(engine->errorString()), true);
    }
}
