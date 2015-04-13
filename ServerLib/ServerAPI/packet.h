#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QByteArray>

class Packet : public QObject
{
    Q_OBJECT
public:
    explicit Packet(QObject *parent = 0);

    virtual void executePacket();

    virtual void unserializePacket(QByteArray* data);
    virtual QByteArray serializePacket();

    virtual bool isClientPacket(){ return true; }
    virtual bool isServerPacket(){ return true; }


signals:

public slots:

};

#endif // PACKET_H
