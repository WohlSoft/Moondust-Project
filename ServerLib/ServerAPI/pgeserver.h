#ifndef PGESERVER_H
#define PGESERVER_H

#include "pgeconnection.h"
#include "pgeconnecteduser.h"

class PGEServer : public PGEConnection
{
    Q_OBJECT
public:
    explicit PGEServer(QObject *parent = 0);
    explicit PGEServer(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);

signals:

public slots:
    //On Server-Side this function is broadcasting
    virtual void dispatchPacket(Packet * packet) Q_DECL_OVERRIDE Q_DECL_FINAL;
    void dispatchPacketToUser(PGEConnectedUser* user, Packet * packet);

private slots:
    void server_incomingConnection();

    void socket_disconnect();
    void socket_receivePacket();

private:
    void init();

    //This is the main handler for incoming connection
    QTcpServer m_server;

    QList<QTcpSocket*> m_unindentifiedSockets;
    QList<PGEConnectedUser*> m_connectedUser;

};
#endif // PGESERVER_H
