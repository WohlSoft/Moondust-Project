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

#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H

#include <QThread>
#include <QVector>
#include <QLocalServer>
#include <QLocalSocket>

#include <file_formats/file_formats.h>

#define ENGINE_SERVER_NAME "PGEEngine42e3j"

class EngineClient : public QThread
{
    Q_OBJECT
public:
    EngineClient();
    ~EngineClient();

    void sendLevelData(LevelData _data);
    void sendCommand(QString command);
    void OpenConnection();
    bool isConnected();

protected:
    void run();
    void exec();

signals:
    void dataReceived(QString data);
    void privateDataReceived(QString data);
    void showUp();
    void openFile(QString path);

private slots:
    void slotOnData(QString data);
    void displayError(QLocalSocket::LocalSocketError socketError);

private:
    QLocalSocket* engine;
    bool _connected;
    bool readyToSendLvlx;
};

#endif // ENGINE_CLIENT_H
