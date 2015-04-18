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

    virtual void unserializePacket(const QByteArray& data);
    virtual QByteArray serializePacket();


signals:

public slots:

};

#endif // PACKET_H
