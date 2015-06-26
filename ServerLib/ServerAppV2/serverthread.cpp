#include "serverthread.h"
#include <iostream>

ServerThread::ServerThread(QTcpSocket *clientSocket, QObject *parent)
    : QThread(parent), clientSocket(clientSocket)
{}

ServerThread::~ServerThread()
{}

void ServerThread::writeToClient(QByteArray bytes)
{
    clientSocket->write(bytes);
}

QByteArray ServerThread::getMessage()
{
    return message;
}

void ServerThread::run()
{
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(displayMessage()), Qt::DirectConnection);
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteThread()), Qt::DirectConnection);
    connect(this, SIGNAL(outputSent()), this->parent(), SLOT(distributeMessage()));

    qDebug() << "RUNNING";
    exec();
}

void ServerThread::displayMessage()
{
    message = clientSocket->readAll();
    qDebug() << message;

    emit outputSent();
}

void ServerThread::deleteThread()
{
    qDebug() << "CLIENT DISCONNECTED";

    clientSocket->disconnectFromHost();
    exit(0);
}
