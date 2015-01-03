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

#include <common_features/app_path.h>

#include "engine_intproc.h"
#include "engine_client.h"

/**
 * @brief EngineClient::LocalServer
 *  Constructor
 */

EngineClient::EngineClient()
{
    engine = new QLocalSocket();

    connect(engine, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(displayError(QLocalSocket::LocalSocketError)));

    readyToSendLvlx = false;
    _connected = true;
}

/**
 * @brief EngineClient::~LocalServer
 *  Destructor
 */
EngineClient::~EngineClient()
{
  engine->close();
  delete engine;
}

void EngineClient::sendLevelData(LevelData _data)
{
    if(!_data.ReadFileValid) return;

    if(!engine->isOpen())
        OpenConnection();


    QString sendLvlx;
    if(!_data.path.isEmpty())
        sendLvlx = QString("SEND_LVLX: %1/%2").arg(_data.path).arg(_data.filename);
    else
        sendLvlx = QString("SEND_LVLX: %1/%2").arg(ApplicationPath).arg("_untitled.lvlx");

    qDebug() << sendLvlx;
    sendCommand(sendLvlx);
    qApp->processEvents();

    if(engine->waitForReadyRead(5000))
    {
        QByteArray data = engine->readAll();
        QString acceptedData = QString::fromUtf8(data);

        if(!acceptedData.isEmpty())
        {
            QStringList StrData = acceptedData.split("\n\n");
            foreach(QString x, StrData)
            {
                if(x=="READY")
                {
                    qDebug() << "Engine is ready to accept data";
                    readyToSendLvlx = true;
                }
            }
        }
    }

    //Pause
    QElapsedTimer time;
    time.start();
    while(!readyToSendLvlx)
    {
        if(time.elapsed()>5000)
        {
            qDebug() << "Time out: Engine is not answer, abort operation";
            IntEngine::quit();
            return;
        }
    }
    readyToSendLvlx=false;

    QString rawData = FileFormats::WriteExtendedLvlFile(_data);
    rawData.append("\n");

    qDebug() << "Sending data to engine...";
    sendCommand(rawData);
    sendCommand("PARSE_LVLX\n\n");
    qDebug() << "Sent, testing is started";

    msleep(100);
}

void EngineClient::sendCommand(QString command)
{
    if(!engine->isOpen())
            OpenConnection();

    QByteArray bytes;
    bytes = command.toUtf8();
    engine->write(bytes);
    if(!engine->waitForBytesWritten(10000))
    {
        qDebug()<<"Error of command sending";
    }
}

void EngineClient::OpenConnection()
{
    qDebug()<<"Connect to Engine " << ENGINE_SERVER_NAME;

    engine->connectToServer(QString(ENGINE_SERVER_NAME));

    qDebug()<<"Wait for connection";

    if(engine->waitForConnected(20000))
    {
        qDebug()<<"Connected";
    }
}

bool EngineClient::isConnected()
{
    return _connected;
}


void EngineClient::run()
{
    _connected=true;
    msleep(100);
    exec();
    _connected=false;
}

void EngineClient::exec()
{
    QElapsedTimer pingPong;
    pingPong.start();
    bool pingSent=false;
      while(engine->isOpen() && !this->isFinished())
      {
            if(engine->waitForReadyRead(1000))
            {
                QByteArray data = engine->readAll();
                QString acceptedData = QString::fromUtf8(data);

                if(!acceptedData.isEmpty())
                {
                    QStringList StrData = acceptedData.split("\n\n");

                    qDebug()<< "EngineClient::exec() accepted: " << acceptedData;

                    foreach(QString x, StrData)
                    {
                        qDebug()<<"Accepted from engine: " << x;

                        if(x=="PING")
                        {
                            QString str = QString("PONG\n\n");
                            QByteArray bytes;
                            bytes = str.toUtf8();
                            engine->write(bytes);
                            engine->flush();
                        }
                        else
                        if(x=="PONG")
                        {
                            qDebug() << "Accepted PONG";
                            pingSent = false;
                        }
                        else
                        if(x=="READY")
                        {
                            qDebug() << "Accepted READY";
                            readyToSendLvlx = true;
                        }
                    }
                }
            }
            else
                engine->close();

            if(pingPong.elapsed()>5000)
            {
                if(pingSent)
                {
                    qDebug() << "Mr. Ping Timeout is here! :D. Engine is not answer";
                    engine->close();
                    _connected=false;
                    break;
                }
                else
                {
                    qDebug() << "Ping";
                    pingPong.restart();
                    QString str = QString("PING\n\n");
                    QByteArray bytes;
                    bytes = str.toUtf8();
                    engine->write(bytes);
                    engine->flush();
                    pingSent=true;
                }
            }
            qDebug() << "Tick";
            msleep(100);
      }
      qDebug() << "Connection with engine was finished";
}




/**
 * -------
 * SLOTS
 * -------
 */
void EngineClient::slotOnData(QString data)
{
  QStringList args = data.split("\n\n");
  foreach(QString c, args)
  {
     emit dataReceived(c);
  }
}

void EngineClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        default:
            qDebug() << QString("The following error occurred: %1.")
                                     .arg(engine->errorString());
    }

}
