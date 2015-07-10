#ifndef PACKETMESSAGE_H
#define PACKETMESSAGE_H

#include "../packet.h"

class PacketMessage : public Packet
{
public:
    PacketMessage(const QString& message);
    PacketMessage();

    QString message() const;
    void setMessage(const QString &message);


    // Packet interface
public:
    void encode(QDataStream &stream);
    void decode(QDataStream &stream);


private:
    QString m_message;
};

Q_DECLARE_METATYPE(PacketMessage)

#endif // PACKETMESSAGE_H
