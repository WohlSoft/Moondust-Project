#ifndef PACKET_H
#define PACKET_H

#include <QMetaType>

#include <ConnectionLib/Shared/user/pgenet_user.h>

class Packet
{

public:
    Packet() {}
    Packet(const Packet& obj) {Q_UNUSED(obj)}
    virtual ~Packet() {}

    virtual void encode(QDataStream& stream){Q_UNUSED(stream)}
    virtual void decode(QDataStream& stream){Q_UNUSED(stream)}

    PGENET_User *getUser() const
    {
        return user;
    }
    void setUser(PGENET_User *value)
    {
        user = value;
    }

    int getSessionID() const
    {
        return sessionID;
    }
    void setSessionID(int value)
    {
        sessionID = value;
    }

protected:
    PGENET_User* user;
    int sessionID;
};










Q_DECLARE_METATYPE(Packet)


#endif // PACKET_H
