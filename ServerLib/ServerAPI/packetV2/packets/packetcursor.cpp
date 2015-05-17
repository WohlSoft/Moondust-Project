#include "packetcursor.h"



PacketCursor::PacketCursor() :
    Packet(),
    x(0),
    y(0)
{}

PacketCursor::PacketCursor(const PacketCursor &obj) :
    Packet(),
    x(0),
    y(0)
{
    x = obj.getX();
    y = obj.getY();
}

QVariant PacketCursor::encode()
{
    QMap<QString, QVariant> dataMap;
    dataMap["x"] = x;
    dataMap["y"] = y;
    return QVariant(dataMap);
}

void PacketCursor::decode(QVariant data)
{
    QMap<QString, QVariant> dataMap = data.toMap();
    x = dataMap["x"].toInt();
    y = dataMap["y"].toInt();
}


int PacketCursor::getX() const
{
    return x;
}

void PacketCursor::setX(int value)
{
    x = value;
}

int PacketCursor::getY() const
{
    return y;
}

void PacketCursor::setY(int value)
{
    y = value;
}
