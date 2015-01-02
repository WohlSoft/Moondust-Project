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

#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QThread>
#include <QVector>
#include <QLocalServer>
#include <QLocalSocket>

#define LOCAL_SERVER_NAME "PGEEditor335jh3c3n8g7"

class LocalServer : public QThread
{
  Q_OBJECT
public:
  LocalServer();
  ~LocalServer();
  void shut();

protected:
  void run();
  void exec();

signals:
  void dataReceived(QString data);
  void privateDataReceived(QString data);
  void showUp();
  void openFile(QString path);
  void acceptedCommand(QString cmd);

private slots:
  void slotNewConnection();
  void slotOnData(QString data);

private:
  QLocalServer* server;
  QVector<QLocalSocket*> clients;
  void onCMD(QString data);

};

#endif // LOCALSERVER_H
