#ifndef SERVERPGE_H
#define SERVERPGE_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

#include "serverthread.h"

class PgeTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    PgeTcpServer();
    PgeTcpServer(qintptr port);
    virtual ~PgeTcpServer();

    bool startServer();
    bool changePort(qintptr port);

    void writeToAllClients(QByteArray);
    QByteArray getAllClientMessages();

public slots:
    void distributeMessage();

private slots:
    void addNewConnection();

private:
    qintptr port;
};

#endif // SERVERPGE_H
