#include "packetuserauth.h"

#include "../../../pgenet_global.h"

#include <QDataStream>

PacketUserAuth::PacketUserAuth() : m_username(""), m_networkVersionNumber(PGENET_Global::NetworkVersion)
{}



void PacketUserAuth::encode(QDataStream &stream)
{
    stream << m_username << m_networkVersionNumber;
}

void PacketUserAuth::decode(QDataStream &stream)
{
    stream >> m_username >> m_networkVersionNumber;
}
QString PacketUserAuth::username() const
{
    return m_username;
}

void PacketUserAuth::setUsername(const QString &username)
{
    m_username = username;
}
int PacketUserAuth::networkVersionNumber() const
{
    return m_networkVersionNumber;
}

void PacketUserAuth::setNetworkVersionNumber(int networkVersionNumber)
{
    m_networkVersionNumber = networkVersionNumber;
}


