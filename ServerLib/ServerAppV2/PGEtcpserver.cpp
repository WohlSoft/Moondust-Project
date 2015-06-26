#include "PGEtcpserver.h"

PgeTcpServer::PgeTcpServer(): port(5000)
{}

PgeTcpServer::PgeTcpServer(qintptr port): port(port)
{}

PgeTcpServer::~PgeTcpServer()
{
    this->close();
}

bool PgeTcpServer::startServer()
{
    connect(this, SIGNAL(newConnection()), this, SLOT(addNewConnection()));

    if (!listen(QHostAddress::Any, port))
    {
        qDebug() << "FAILED TO START SERVER";
        return false;
    }
    qDebug() << "STARTED SERVER";

    return true;
}

bool PgeTcpServer::changePort(qintptr port)
{
    if (this->isListening())
        return false;

    this->port = port;

    return true;
}

void PgeTcpServer::writeToAllClients(QByteArray bytes)
{
    const QList<ServerThread*> children = this->findChildren<ServerThread*>();

    for (int i = 0; i < children.size(); i++)
    {
        children[i]->writeToClient(bytes);
    }
}

QByteArray PgeTcpServer::getAllClientMessages()
{
    QByteArray bytes;
    const QList<ServerThread*> children = this->findChildren<ServerThread*>();

    for (int i = 0; i < children.size(); i++)
    {
        bytes.append(children[i]->getMessage());
        bytes.append("\r\n");
    }

    return bytes;
}

void PgeTcpServer::distributeMessage()
{
    writeToAllClients(getAllClientMessages());
}

void PgeTcpServer::addNewConnection()
{
    QTcpSocket *clientSocket = this->nextPendingConnection();

    ServerThread *thread = new ServerThread(clientSocket, this);
    thread->start();
}
