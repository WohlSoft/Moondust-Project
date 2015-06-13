#ifndef PACKETHANDSHAKEACCEPTED_H
#define PACKETHANDSHAKEACCEPTED_H

#include <packet/packet.h>

class PacketHandshakeAccepted : public Packet
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit PacketHandshakeAccepted(QObject *parent = 0);

    virtual void executePacket() Q_DECL_OVERRIDE;

    virtual void unserializePacket(const QByteArray& data) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QByteArray serializePacket()  Q_DECL_OVERRIDE Q_DECL_FINAL;

signals:

public slots:

private:
    qlonglong m_serverPGENetworkVersion;
};

#endif // PACKETHANDSHAKEACCEPTED_H
