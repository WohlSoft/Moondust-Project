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

    virtual void executePacket() Q_DECL_OVERRIDE Q_DECL_FINAL;

    virtual void unserializePacket(const QByteArray& data) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QByteArray serializePacket()  Q_DECL_OVERRIDE Q_DECL_FINAL;

signals:

public slots:

private:
    QString m_username;
    qlonglong m_pgeNetworkVersion;


};

#endif // PACKETHANDSHAKE_H
