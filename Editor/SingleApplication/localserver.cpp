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

#include <QFile>
#include <QStringList>
#include <QtDebug>

#include <common_features/mainwinconnect.h>
#include <networking/engine_intproc.h>

#include "localserver.h"


IntProcServer::IntProcServer()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

IntProcServer::~IntProcServer()
{}

void IntProcServer::stateChanged(QAbstractSocket::SocketState stat)
{
    switch(stat)
    {
        case QAbstractSocket::UnconnectedState: qDebug()<<"The socket is not connected.";break;
        case QAbstractSocket::HostLookupState: qDebug()<<"The socket is performing a host name lookup.";break;
        case QAbstractSocket::ConnectingState: qDebug()<<"The socket has started establishing a connection.";break;
        case QAbstractSocket::ConnectedState: qDebug()<<"A connection is established.";break;
        case QAbstractSocket::BoundState: qDebug()<<"The socket is bound to an address and port.";break;
        case QAbstractSocket::ClosingState: qDebug()<<"The socket is about to close (data may still be waiting to be written).";break;
        case QAbstractSocket::ListeningState: qDebug()<<"[For internal]";break;
    }
}

void IntProcServer::readData()
{
    while (hasPendingDatagrams())
    {
            QByteArray datagram;
            datagram.resize(pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            emit messageIn(QString::fromUtf8(datagram));
    }
}

void IntProcServer::displayError(QTcpSocket::SocketError socketError)
{
    switch (socketError)
    {
        default:
            qDebug() << QString("EDITOR SingleAPP: The following error occurred: %1.")
                                     .arg(errorString());
    }
}

IntProcServer *ipServer=NULL;




/**
 * @brief LocalServer::LocalServer
 *  Constructor
 */
LocalServer::LocalServer()
{
    ipServer = new IntProcServer();
    connect(ipServer, SIGNAL(messageIn(QString)), this, SLOT(slotOnData(QString)));
    connect(this, SIGNAL(privateDataReceived(QString)), this, SLOT(slotOnData(QString)));
    ipServer->bind(QHostAddress::LocalHost, 58487);
}


/**
 * @brief LocalServer::~LocalServer
 *  Destructor
 */
LocalServer::~LocalServer()
{
  ipServer->close();
  delete ipServer;
}




/**
 * -----------------------
 * QThread requred methods
 * -----------------------
 */

/**
 * @brief run
 *  Initiate the thread.
 */
void LocalServer::run()
{
  exec();
}

/**
 * @brief LocalServer::exec
 *  Keeps the thread alive. Waits for incomming connections
 */
void LocalServer::exec()
{
  while(ipServer->isOpen())
  {
    msleep(100);
  }
}


/**
 * -------
 * SLOTS
 * -------
 */

void LocalServer::stopServer()
{
    if(ipServer) ipServer->close();
}



/**
 * @brief LocalServer::slotOnData
 *  Executed when data is received
 * @param data
 */
void LocalServer::slotOnData(QString data)
{
  qDebug() << data;
  QStringList args = data.split('\n');
  foreach(QString c, args)
  {
      if(c.startsWith("CMD:", Qt::CaseInsensitive))
      {
        onCMD(c);
      }
      else
      {
        emit dataReceived(c);
      }
  }
}



/**
 * -------
 * Helper methods
 * -------
 */
void LocalServer::onCMD(QString data)
{
  //  Trim the leading part from the command
  if(data.startsWith("CMD:"))
  {
    data.remove("CMD:");

    qDebug()<<"Accepted data: "+data;

    QStringList commands;
    commands << "showUp";
    commands << "CONNECT_TO_ENGINE";
    commands << "ENGINE_CLOSED";
    commands << "Is editor running?";

    if(MainWinConnect::pMainWin->continueLoad)
        switch(commands.indexOf(data))
        {
            case 0:
            {
                emit showUp();
                MainWinConnect::pMainWin->setWindowState((MainWinConnect::pMainWin->windowState()&
                                                         (~(MainWinConnect::pMainWin->windowState()&Qt::WindowMinimized)))
                                                          |Qt::WindowActive);
                if(MainWinConnect::pMainWin->isMinimized())
                {
                    MainWinConnect::pMainWin->raise();
                    MainWinConnect::pMainWin->activateWindow();
                    MainWinConnect::pMainWin->showNormal();
                }
                qApp->setActiveWindow(MainWinConnect::pMainWin);
                break;
            }
            case 1:
            {
                if(!IntEngine::isWorking())
                {
                    IntEngine::init();
                }
                IntEngine::sendLevelBuffer();
                //MainWinConnect::pMainWin->showMinimized();
                //IntEngine::engineSocket->sendLevelData(IntEngine::testBuffer);
                break;
            }
            case 2:
            {                
                if(IntEngine::isWorking())
                {
                    IntEngine::quit();
                }
                break;
            }
            case 3:
            {
                QUdpSocket answer;
                answer.connectToHost(QHostAddress::LocalHost, 58488);
                answer.waitForConnected(100);
                answer.write(QString("Yes, I'm runs!").toUtf8());
                answer.waitForBytesWritten(100);
                answer.flush();
                break;
            }
            default:
              emit acceptedCommand(data);
        }
  }
  else
      emit acceptedCommand(data);
}
