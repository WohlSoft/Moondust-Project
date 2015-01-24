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

EngineClient::EngineClient()
{
    engine = NULL;
    qRegisterMetaType<QAbstractSocket::SocketState > ("QAbstractSocket::SocketState");
    qRegisterMetaType<QTextBlock > ("QTextBlock");

    readyToSendLvlx = false;
    _connected = false;
    _busy = false;
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

    msleep(20);
    if(!sendCommand(sendLvlx))
    {
        IntEngine::quit();
        return;
    }
    _busy=false;

    WriteToLog(QtDebugMsg, "readyToSendLvlx is true", true);
    readyToSendLvlx=false;
    qDebug() << "Wait for 'READY'";
}

bool EngineClient::sendCommand(QString command)
{
    if(!engine)
        return false;
    if(!_connected)
        return false;
    if(!engine->isOpen())
        return false;

    QByteArray bytes;
    bytes = command.toUtf8();
    engine->write(bytes);
    engine->flush();
    engine->waitForBytesWritten(10000);
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

    _busy=false;
    if(engine->isOpen())
    {
        qDebug()<<"Connected to "+engine->serverName();
        return;
    }

    qRegisterMetaType<QAbstractSocket::SocketState> ("QAbstractSocket::SocketState");

    qDebug()<<"Connect to Engine " << ENGINE_SERVER_NAME;

    engine->connectToServer(QString(ENGINE_SERVER_NAME));
    qDebug()<<"Wait for connection";

    if(engine->waitForConnected(20000))
    {
        qDebug()<<"Connected";
    }
    else
    {
        qDebug()<<"Failed to connect: " << engine->errorString();
    }
}

void EngineClient::closeConnection()
{
    _busy=false;
    _connected=false;
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
    exec();
    if(engine)
    {
        delete engine;
        engine = NULL;
    }
    _connected=false;
}

void EngineClient::exec()
{

      while(engine->isOpen() && !this->isFinished() && _connected)
      {
            if(engine->waitForReadyRead(1000))
            {
                QByteArray data = engine->readAll();
                QString acceptedData = QString::fromUtf8(data);

                emit privateDataReceived(acceptedData);
            }
            msleep(100);
      }
      engine->close();
      WriteToLog(QtDebugMsg, "Connection with engine was finished", true);
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

        if(c==QChar('\n'))
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
    emit dataReceived(in);

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
        engine->disconnectFromServer();
        _connected=false;
    }
    else
    if(in=="READY")
    {
        WriteToLog(QtDebugMsg, "Accepted READY", true);
        if(_buffer_out.isEmpty()) _buffer_out="HEAD\nEMPTY:1\nHEAD_END\n\n";
        if(sendCommand(_buffer_out))
            WriteToLog(QtDebugMsg, "LVLX buffer Sent", true);
        else
        {
            WriteToLog(QtDebugMsg, "Send fail", true);
            _buffer_out.clear();
            this->closeConnection();
        }

        if(sendCommand("PARSE_LVLX\n\n"))
            WriteToLog(QtDebugMsg, "PARSE_LVLX command Sent", true);
        _buffer_out.clear();
    }
}



void EngineClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        default:
            WriteToLog(QtDebugMsg, QString("The following error occurred: %1.")
                       .arg(engine->errorString()), true);
    }
}
