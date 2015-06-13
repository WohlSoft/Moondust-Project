#include "_tcp_server.h"
#include <QTime>
#include <QDate>
#include <QDateTime>

PGE_TcpServer::PGE_TcpServer()
{
    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

PGE_TcpServer::~PGE_TcpServer()
{}

void PGE_TcpServer::writeMessage(QString msg, QString clientID)
{
    if(registredClients.contains(clientID))
    {
        QTcpSocket *client = registredClients[clientID];
        client->write(msg.toUtf8());
        client->waitForBytesWritten(1000);
    }
}

void PGE_TcpServer::stateChanged(QAbstractSocket::SocketState stat)
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

void PGE_TcpServer::sendMessageToAllClients(QString msg)
{
    QString message = msg;
    foreach(QTcpSocket *client, clientConnections)
    {
        client->write(message.toUtf8());
    }
}

void PGE_TcpServer::stop()
{
    this->close();
    qDeleteAll(clientConnections);
}

void PGE_TcpServer::readData()
{
    QObject * object = QObject::sender(); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    if (!object)
        return;
    QTcpSocket * socket = static_cast<QTcpSocket *>(object);
    QByteArray arr =  socket->readAll();
    emit messageIn(QString::fromUtf8(arr));
}

void PGE_TcpServer::handleNewConnection()
{
    while(hasPendingConnections())
    {
        QTcpSocket *client = nextPendingConnection();
        connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        connect(client, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
        clientConnections.append(client);
    }
}

void PGE_TcpServer::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if (!client)
        return;

    clientConnections.removeAll(client);
    client->deleteLater();
}

void PGE_TcpServer::displayError(QTcpSocket::SocketError socketError)
{
    switch (socketError)
    {
        default:
            qDebug() << QString("ENGINE: The following error occurred: %1.")
                                     .arg(errorString());
    }
}

void PGE_TcpServer::doPingPong()
{
    QString message = QString("PING\n\n");
    foreach(QTcpSocket *client, clientConnections)
    {
        ClientPing ping;
        ping.first = client;
        ping.second = QDateTime::currentMSecsSinceEpoch();
        client->write(message.toUtf8());
        waitsPong.append(ping);
    }

    foreach(ClientPing ping, waitsPong)
    {
        if(QDateTime::currentMSecsSinceEpoch()-ping.second > 160)
        {
            ping.first->disconnectFromHost();
            waitsPong.removeAll(ping);
        }
    }
}

