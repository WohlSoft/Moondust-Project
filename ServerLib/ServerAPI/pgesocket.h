#ifndef PGESOCKET_H
#define PGESOCKET_H

#include <QObject>
#include "pgesocketdefines.h"
#include "packet.h"
#include <QMap>
#include <QThread>

#include <QTcpServer>
#include <QTcpSocket>

#include <qlogging.h>

class PGESocket : public QThread
{
    Q_OBJECT
public:

    enum class PGESocketType {
        PGEServer,
        PGESocket
    };

    //This is the default constructor
    explicit PGESocket(QObject *parent = 0);
    explicit PGESocket(PGESocketType type, QObject *parent = 0);
    //This is the constructor + registerPackets in one go
    explicit PGESocket(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);
    explicit PGESocket(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, PGESocketType type, QObject *parent = 0);



    //Here you can register your packets
    void registerPackets(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets);
    void registerPacket(QMetaObject* packetMetaObject, PGEPackets packetType);


    //Packet functions
    void dispatchPacket(Packet * packet);
    //Must be synced with the GUI-Thread
    void execAllPacket();


signals:

public slots:

public:
    //Static functions
    static bool isBaseOfPacket(QMetaObject* possiblePacket);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    void init();

    QMap<PGEPackets, QMetaObject*> m_registeredPackets;

    PGESocketType m_socketType;

    //If host is server, then use QTcpServer
    QTcpServer* m_server;

    //If host is socket, then use QTcpSocket
    QTcpSocket* m_socket;
};

#endif // PGESOCKET_H
