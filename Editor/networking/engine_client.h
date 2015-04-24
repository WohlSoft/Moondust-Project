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
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>

#include <PGE_File_Formats/file_formats.h>

#define ENGINE_SERVER_NAME "PGEEngine42e3j"


class IpsEngineClient : public QObject
{
    Q_OBJECT
public:
    explicit IpsEngineClient(QObject *parent = 0);
    ~IpsEngineClient();

    bool doConnect();
    QString errString();
    bool isOpen();
    void close();
    bool opeIsOk();

signals:
    void messageIn(QString msg);
    void closeThread();
public slots:
    void doClose();
    void doOpen();
    void sendMessage(QString msg);
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
private slots:
    void displayError(QAbstractSocket::SocketError socketError);

private:
    bool ok;
    QTcpSocket *socket;
};




class EngineClient : public QThread
{
    Q_OBJECT
public:
    EngineClient();
    ~EngineClient();

    void sendLevelData(LevelData _data);
    bool sendCommand(QString command);
    void OpenConnection();
    void closeConnection();
    bool isConnected();
protected:
    void run();
    void exec();

signals:
    void sendMessage(QString msg);
    void dataReceived(QString data);
    void showUp();
    void closed();
    void open();
    void openFile(QString path);

private slots:
    void connectionLost();
    void slotOnData(QString data);
    void icomingData(QString in);

private:
    static bool alreadyRequested;
    friend class IntEngine;
    bool _connected;
    bool _busy;
    bool readyToSendLvlx;
    bool doSendData;
    QString _buffer;
    QString _buffer_out;
};

#endif // ENGINE_CLIENT_H
