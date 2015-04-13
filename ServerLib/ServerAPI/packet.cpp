#include "packet.h"

Packet::Packet(QObject *parent) :
    QObject(parent)
{

}

void Packet::executePacket()
{}

QByteArray Packet::serializePacket()
{
    return "";
}
