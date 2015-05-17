#ifndef PACKETCURSOR_H
#define PACKETCURSOR_H

#include "packet.h"

class PacketCursor : public Packet
{
private:
    int x;
    int y;

public:
    PacketCursor();
    PacketCursor(const PacketCursor& obj);
    ~PacketCursor();

    QVariant encode();
    void decode(QVariant data);
    int getX() const;
    void setX(int value);
    int getY() const;
    void setY(int value);
};

Q_DECLARE_METATYPE(PacketCursor)

#endif // PACKETCURSOR_H
