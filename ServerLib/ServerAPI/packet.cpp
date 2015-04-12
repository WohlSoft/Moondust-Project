#include "packet.h"

Packet::Packet(QObject *parent) :
    QObject(parent)
{

}

void Packet::executePacket()
{}

QString Packet::serializePacket()
{
    return "";
}
