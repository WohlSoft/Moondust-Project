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

#include <QtDebug>

#include "singleapplication.h"

/**
 * @brief SingleApplication::SingleApplication
 *  Constructor. Checks and fires up LocalServer or closes the program
 *  if another instance already exists
 * @param argc
 * @param argv
 */
SingleApplication::SingleApplication(QStringList &args)
{
  _shouldContinue = false; // By default this is not the main process

  socket = new QUdpSocket();
  server = NULL;
  QUdpSocket acceptor;
  acceptor.bind(QHostAddress::LocalHost, 58488, QUdpSocket::ReuseAddressHint|QUdpSocket::ShareAddress);

  // Attempt to connect to the LocalServer
  socket->connectToHost(QHostAddress::LocalHost, 58487);
  QString isServerRuns;
  if(socket->waitForConnected(100))
  {
      socket->write(QString("CMD:Is editor running?").toUtf8());
      socket->flush();
      if(acceptor.waitForReadyRead(100))
      {
          //QByteArray dataGram;//Yes, I'm runs!
          QByteArray datagram;
          datagram.resize(acceptor.pendingDatagramSize());
          QHostAddress sender;
          quint16 senderPort;
          acceptor.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
          if(QString::fromUtf8(datagram)=="Yes, I'm runs!")
          {
              isServerRuns="Yes!";
          }
      }
  }

  if(args.contains("--force-run", Qt::CaseInsensitive))
  {
      isServerRuns.clear();
      args.removeAll("--force-run");
  }
  _arguments = args;

  if(!isServerRuns.isEmpty())
  {
    QString str = QString("CMD:showUp");
    QByteArray bytes;
    for(int i=1; i<_arguments.size(); i++)
    {
       str.append(QString("\n%1").arg(_arguments[i]));
    }
    bytes = str.toUtf8();
    socket->write(bytes);
    socket->flush();
    QThread::msleep(100);
    socket->close();
  }
  else
  {
    // The attempt was insuccessful, so we continue the program
    _shouldContinue = true;
    server = new LocalServer();
    server->start();
    QObject::connect(server, SIGNAL(showUp()), this, SLOT(slotShowUp()));
    QObject::connect(server, SIGNAL(dataReceived(QString)), this, SLOT(slotOpenFile(QString)));
    QObject::connect(server, SIGNAL(acceptedCommand(QString)), this, SLOT(slotAcceptedCommand(QString)));
    QObject::connect(this, SIGNAL(stopServer()), server, SLOT(stopServer()));
  }
}

/**
 * @brief SingleApplication::~SingleApplication
 *  Destructor
 */
SingleApplication::~SingleApplication()
{
  if(_shouldContinue)
  {
      emit stopServer();
      if((server) && (!server->wait(5000)))
      {
          qDebug() << "TERMINATOR RETURNS BACK single application! 8-)";
          server->terminate();
          qDebug() << "Wait for nothing";
          server->wait();
          qDebug() << "Terminated!";
      }
  }
  if(server) delete server;
}

/**
 * @brief SingleApplication::shouldContinue
 *  Weather the program should be terminated
 * @return bool
 */
bool SingleApplication::shouldContinue()
{
    return _shouldContinue;
}

QStringList SingleApplication::arguments()
{
    return _arguments;
}

/**
 * @brief SingleApplication::slotShowUp
 *  Executed when the showUp command is sent to LocalServer
 */
void SingleApplication::slotShowUp()
{
  emit showUp();
}

void SingleApplication::slotOpenFile(QString path)
{
    emit openFile(path);
}

void SingleApplication::slotAcceptedCommand(QString cmd)
{
    emit acceptedCommand(cmd);
}

