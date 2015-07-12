#ifndef PACKETUSERAUTH_H
#define PACKETUSERAUTH_H

#include "../packet.h"

class PacketUserAuth : public Packet
{
public:
    PacketUserAuth();

    // Packet interface
public:
    void encode(QDataStream &stream);
    void decode(QDataStream &stream);

    QString username() const;
    void setUsername(const QString &username);

    int networkVersionNumber() const;
    void setNetworkVersionNumber(int networkVersionNumber);

private:
    QString m_username;
    int m_networkVersionNumber;
};

Q_DECLARE_METATYPE(PacketUserAuth)

#endif // PACKETUSERAUTH_H
