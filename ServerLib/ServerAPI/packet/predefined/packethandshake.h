#ifndef PACKETHANDSHAKE_H
#define PACKETHANDSHAKE_H

#include <packet/packet.h>
#include <pgeconnecteduser.h>

class PacketHandshake : public Packet
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit PacketHandshake(QObject *parent = 0);

    PGEConnectedUser* createUserOfData();

signals:

public slots:

};

#endif // PACKETHANDSHAKE_H
