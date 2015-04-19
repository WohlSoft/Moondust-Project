#ifndef PGESERVER_H
#define PGESERVER_H

#include <pgeconnection.h>
#include <pgeconnecteduser.h>

class PGEServer : public PGEConnection
{
    Q_OBJECT
public:
    explicit PGEServer(QObject *parent = 0);
    explicit PGEServer(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);
    ~PGEServer();
public slots:
    //On Server-Side this function is broadcasting
    virtual void dispatchPacket(Packet * packet) Q_DECL_OVERRIDE Q_DECL_FINAL;
    void dispatchPacketToUser(PGEConnectedUser* user, Packet * packet);

signals:
    void userConnected(PGEConnectedUser* user);
    void userDisconnected(PGEConnectedUser* user);
    void receivedData();

protected:
    virtual void run();

private slots:
    void server_incomingConnection();
    void serversocket_reportError(QAbstractSocket::SocketError error);

    void socket_receivePacket();
    void socket_disconnect();

private:
    void init();
    PGEConnectedUser* getPGEUserBySocket(QTcpSocket* socket);

    //This is the main handler for incoming connection
    QTcpServer* m_server;

    QList<QTcpSocket*> m_unindentifiedSockets;
    QList<PGEConnectedUser*> m_connectedUser;

};
#endif // PGESERVER_H
