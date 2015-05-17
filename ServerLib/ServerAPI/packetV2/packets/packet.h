#ifndef PACKET_H
#define PACKET_H

#include <QMetaType>
#include <QVariant>

class Packet
{
protected:
    Packet();

public:
    virtual ~Packet() = 0;

    virtual QVariant encode() = 0;
    virtual void decode(QVariant data) = 0;
};



#endif // PACKET_H
