#include "packetmessage.h"

PacketMessage::PacketMessage(const QString &message, QObject *parent) : Packet(parent), m_message(message)
{}

PacketMessage::PacketMessage(QObject *parent) : m_message("")
{}


