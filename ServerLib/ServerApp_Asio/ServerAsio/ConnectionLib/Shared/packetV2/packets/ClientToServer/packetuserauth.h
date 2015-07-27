#ifndef PACKETUSERAUTH_H
#define PACKETUSERAUTH_H

#include "../packet.h"

class PacketUserAuth : public Packet
{
    Q_OBJECT
    Q_DISABLE_COPY(PacketUserAuth)

    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(int networkVersionNumber READ networkVersionNumber WRITE setNetworkVersionNumber)
public:
    Q_INVOKABLE PacketUserAuth(QObject* parent = 0);

    // Packet interface
public:

    QString username() const
    {
        return m_username;
    }

    int networkVersionNumber() const
    {
        return m_networkVersionNumber;
    }


public slots:
    void setUsername(QString username)
    {
        m_username = username;
    }

    void setNetworkVersionNumber(int networkVersionNumber)
    {
        m_networkVersionNumber = networkVersionNumber;
    }

private:
//    QString m_username;
    //    int m_networkVersionNumber;
    QString m_username;
    int m_networkVersionNumber;
};


#endif // PACKETUSERAUTH_H
