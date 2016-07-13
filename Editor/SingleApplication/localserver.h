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

#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QThread>
#include <QVector>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QAtomicInteger>

#define PGE_EDITOR_SEMAPHORE     "PGE_EDITOR_SEMAF_wejQEThQetjqetJQEtjQeTJQTYJ"
#define PGE_EDITOR_SHARED_MEMORY "PGE_EDITOR_SHMEM_wejQEThQetjqetJQEtjQeTJQTYJ"

class LocalServer : public QThread
{
  Q_OBJECT
  //! Semaphore, avoids races
  QSystemSemaphore  m_sema;
  //! Shared memory, stable way to avoid concurrent running multiple copies of same application
  QSharedMemory     m_shmem;
  //! Trigger
  QAtomicInteger<bool> m_isWorking;
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
  void stopServer();
  void slotOnData(QString data);

private:
  void onCMD(QString data);

};

#endif // LOCALSERVER_H
