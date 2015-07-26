#ifndef PACKETMESSAGE_H
#define PACKETMESSAGE_H

#include "../packet.h"

class PacketMessage : public Packet
{
    Q_OBJECT
    Q_DISABLE_COPY(PacketMessage)

    Q_PROPERTY(QString message READ message WRITE setMessage)
public:
    PacketMessage(const QString& message, QObject* parent = 0);
    Q_INVOKABLE PacketMessage(QObject *parent = 0);



    // Packet interface
public:


    QString message() const
    {
        return m_message;
    }

public slots:
    void setMessage(QString message)
    {
        m_message = message;
    }

private:
    QString m_message;
};

#endif // PACKETMESSAGE_H
