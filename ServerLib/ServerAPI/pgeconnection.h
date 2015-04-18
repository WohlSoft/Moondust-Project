#ifndef PGESOCKET_H
#define PGESOCKET_H


#include <pgesocketdefines.h>
#include <packet/packet.h>
#include <packet/predefined/packethandshake.h>
#include <packet/base/handshake/packethandshakeaccepted.h>

#include <QObject>
#include <QMap>
#include <QThread>

#include <QTcpServer>
#include <QTcpSocket>

#include <qlogging.h>
#include <QList>
#include <QSharedPointer>

class PGEConnection : public QThread
{
    Q_OBJECT

public:
    //Static functions
    static bool isBaseOfPacket(QMetaObject* possiblePacket);

public:
    //This is the default constructor
    explicit PGEConnection(QObject *parent = 0);
    //This is the constructor + registerPackets in one go
    explicit PGEConnection(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);

    ~PGEConnection();

    //Here you can register your packets
    void registerPackets(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets);
    void registerPacket(QMetaObject* packetMetaObject, PGEPackets packetType);

public slots:
    //Packet functions

    //This function will sent a packet to the server or client.
    virtual void dispatchPacket(Packet * packet) = 0;

    //Must be synced with the GUI-Thread
    Q_INVOKABLE void execAllPacket();

protected:
    QList<QSharedPointer<Packet> > m_packets;
    QMap<PGEPackets, const QMetaObject*> m_registeredPackets;

    PGEPackets getPacketTypeByMetaObject(const QMetaObject *packetObject);


private:
    void init();


};

#endif // PGESOCKET_H
