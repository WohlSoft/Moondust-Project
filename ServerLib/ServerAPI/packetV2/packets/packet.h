#ifndef PACKET_H
#define PACKET_H

#include <QMetaType>
#include <QVariant>

class Packet
{

public:
    Packet() {}
    Packet(const Packet& obj) {Q_UNUSED(obj)}
    virtual ~Packet() {}

    virtual QVariant encode() { return QVariant(); }
    virtual void decode(QVariant data) {Q_UNUSED(data)}
};

Q_DECLARE_METATYPE(Packet)


#endif // PACKET_H
