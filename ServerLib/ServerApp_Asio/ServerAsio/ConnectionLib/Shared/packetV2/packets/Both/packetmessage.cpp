#include "packetmessage.h"

PacketMessage::PacketMessage(const QString &message) : m_message(message)
{}

PacketMessage::PacketMessage() : m_message("")
{}



void PacketMessage::encode(QDataStream &stream)
{
}

void PacketMessage::decode(QDataStream &stream)
{
}


QString PacketMessage::message() const
{
    return m_message;
}

void PacketMessage::setMessage(const QString &message)
{
    m_message = message;
}
