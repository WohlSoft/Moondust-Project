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


IpsEngineClient::IpsEngineClient(QObject *parent) :
    QObject(parent)
{
    socket=NULL;
    opened=false;
    busy=false;
}

IpsEngineClient::~IpsEngineClient()
{
    if(socket) delete socket;
}

bool IpsEngineClient::doConnect()
{
    if(!socket) socket = new QTcpSocket(this);
    opened=false;
    busy=true;
    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost(QHostAddress::LocalHost, 58486);

    // we need to wait...
    if(!socket->waitForConnected(10000))
    {
        qDebug() << "Error: " << socket->errorString();
        busy=false;
        return false;
    }
    busy=false;
    return true;
}

QString IpsEngineClient::errString()
{
    if(socket)
        return socket->errorString();
    else
        return "Socket is not constructed!";
}

bool IpsEngineClient::isOpen()
{
    if(socket)
        return socket->isOpen();
    else
        return false;
}

void IpsEngineClient::close()
{
    if(socket)
    {
        socket->disconnectFromHost();
        socket->close();
    }
    opened=false;
}

bool IpsEngineClient::openIsSuccess()
{
    return opened;
}

bool IpsEngineClient::isBusy()
{
    return busy;
}

void IpsEngineClient::doClose()
{
    close();
}

void IpsEngineClient::doOpen()
{
    opened=doConnect();
}


void IpsEngineClient::sendMessage(QString msg)
{
    if(!socket) return;
    socket->write(msg.toUtf8());
    socket->waitForBytesWritten(1000);
}

void IpsEngineClient::connected()
{
    qDebug() << "connected...";
}

void IpsEngineClient::disconnected()
{
    qDebug() << "disconnected...";
    emit closeThread();
}

void IpsEngineClient::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void IpsEngineClient::readyRead()
{
    if(!socket) return;
    qDebug() << "reading...";

    // read the data from the socket
    QByteArray arr = socket->readAll();
    emit messageIn(QString::fromUtf8(arr));
}


void IpsEngineClient::displayError(QAbstractSocket::SocketError socketError)
{
    if(!socket) return;

    switch (socketError)
    {
        case QTcpSocket::SocketTimeoutError:
            break;
        default:
            WriteToLog(QtDebugMsg, QString("EDITOR: The following error occurred: %1.")
                       .arg(socket->errorString()), true);
    }
}



IpsEngineClient * ipClient=NULL;






/**
 * @brief EngineClient::LocalServer
 *  Constructor
 */

bool EngineClient::alreadyRequested=false;

EngineClient::EngineClient() : QThread(NULL)
{
    if(!ipClient) ipClient = new IpsEngineClient();
    connect(this, SIGNAL(sendMessage(QString)), ipClient, SLOT(sendMessage(QString)));
    connect(ipClient, SIGNAL(messageIn(QString)), this, SLOT(slotOnData(QString)));
    connect(this, SIGNAL(closed()), ipClient, SLOT(doClose()));
    connect(this, SIGNAL(open()), ipClient, SLOT(doOpen()));
    connect(ipClient, SIGNAL(closeThread()), this, SLOT(connectionLost()));

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
    if(ipClient)
        delete ipClient;
    ipClient=NULL;
}

void EngineClient::sendLevelData(LevelData _data)
{
    if(!_data.ReadFileValid) return;
    if(!ipClient) return;

    doSendData = false;

    if(!ipClient->isOpen())
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
    if(!ipClient)
        return false;
    if(!_connected)
        return false;
    if(!ipClient->isOpen())
        return false;

    QElapsedTimer timeout;
    timeout.start();

    //WriteToLog(QtDebugMsg, "do 'while command is not empty'", true);
    while(!command.isEmpty())
    {
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

        emit sendMessage(send);
        if(timeout.elapsed()>5000) break;
    }
    WriteToLog(QtDebugMsg, "done", true);
    return true;
}

void EngineClient::OpenConnection()
{
    if(!ipClient) return;

    _buffer.clear();
    _busy=false;
    if(ipClient->isOpen())
    {
        qDebug()<<"Connected to localhost:58486";
        return;
    }

    WriteToLog(QtDebugMsg, QString("Connect to Engine localhost:58486"), true);

    WriteToLog(QtDebugMsg, "Wait for connection", true);
    emit open();

    while(ipClient->isBusy());//!< wait for connection

    if(ipClient->openIsSuccess())
    {
        WriteToLog(QtDebugMsg, "Connected", true);
    }
    else
    {
        WriteToLog(QtDebugMsg, "Failed to connect: "+ipClient->errString(), true);
    }
}

void EngineClient::closeConnection()
{
    emit closed();
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
    emit closed();
    _connected=false;
}

void EngineClient::exec()
{
    while(_connected)
    {
        if(doSendData)
        {
            doSendData=false;
            sendLevelData(IntEngine::testBuffer);
        }
        //WriteToLog(QtDebugMsg, "EDITOR: sleep", true);
        msleep(100);
    }
    WriteToLog(QtDebugMsg, "EDITOR: Connection with engine was finished", true);
}

void EngineClient::connectionLost()
{
    _connected=false;
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
        emit sendMessage(str);
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
        emit closed();
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
