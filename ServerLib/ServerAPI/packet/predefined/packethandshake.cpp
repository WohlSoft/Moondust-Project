#include "packethandshake.h"
#include <utils/pgewriterutils.h>
#include <pgesocketdefines.h>

/*
 * Packet data:
 * qlonglong PGENetworkID
 * int lengthOfUsername
 * char* username
 *
 */

PacketHandshake::PacketHandshake(QObject *parent) :
    Packet(parent)
{
    m_pgeNetworkVersion = PGENetworkVersion;
}

PGEConnectedUser *PacketHandshake::createUserOfData()
{
    PGEConnectedUser* newUser = new PGEConnectedUser();
    newUser->Username = m_username;
    newUser->PGENetworkVersion = m_pgeNetworkVersion;
    return newUser;
}

void PacketHandshake::executePacket()
{}

void PacketHandshake::unserializePacket(const QByteArray &data)
{
    QDataStream reader(data);

    reader.readRawData(reinterpret_cast<char*>(&m_pgeNetworkVersion), sizeof(decltype(m_pgeNetworkVersion)));
    m_username = readStreamDataStr(reader);
}

QByteArray PacketHandshake::serializePacket()
{
    QByteArray retArray;
    QDataStream writer(&retArray, QIODevice::WriteOnly);

    writer.writeRawData(reinterpret_cast<const char*>(&m_pgeNetworkVersion), sizeof(decltype(m_pgeNetworkVersion)));
    writeStreamData(writer, m_username);

    return retArray;
}
