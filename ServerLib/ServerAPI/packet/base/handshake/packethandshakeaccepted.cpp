#include "packethandshakeaccepted.h"
#include <pgesocketdefines.h>
#include <QDataStream>
#include <QByteArray>

PacketHandshakeAccepted::PacketHandshakeAccepted(QObject *parent) :
    Packet(parent)
{
    m_serverPGENetworkVersion = PGENetworkVersion;
}

void PacketHandshakeAccepted::executePacket()
{}

void PacketHandshakeAccepted::unserializePacket(const QByteArray &data)
{
    QDataStream reader(data);

    reader.readRawData(reinterpret_cast<char*>(&m_serverPGENetworkVersion), sizeof(decltype(m_serverPGENetworkVersion)));
}

QByteArray PacketHandshakeAccepted::serializePacket()
{
    QByteArray retArray;
    QDataStream writer(&retArray, QIODevice::WriteOnly);

    writer.writeRawData(reinterpret_cast<const char*>(&m_serverPGENetworkVersion), sizeof(decltype(m_serverPGENetworkVersion)));

    return retArray;
}
