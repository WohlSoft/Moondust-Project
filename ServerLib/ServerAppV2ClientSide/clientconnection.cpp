#include "clientconnection.h"

ClientConnection::ClientConnection(QTcpSocket *socket, QObject *parent) :
    QObject(parent), connectionToServer(socket)
{
    connect(connectionToServer, SIGNAL(readyRead()), this, SLOT(messageReceived()), Qt::DirectConnection);
}

void ClientConnection::connectToServer(QString hostname, int port)
{
    qDebug() << "attempt to connect";
    connectionToServer->connectToHost(hostname, port);
}

void ClientConnection::sendData(QByteArray bytes)
{
    connectionToServer->write(bytes);
}

void ClientConnection::disconnectFromServer()
{
    qDebug() << "attempt to disconnect";
    connectionToServer->disconnectFromHost();
}

void ClientConnection::messageReceived()
{
    emit messageReceived(connectionToServer->readAll());
}
