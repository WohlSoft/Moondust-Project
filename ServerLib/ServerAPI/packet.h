#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QString>

class Packet : public QObject
{
    Q_OBJECT
public:
    explicit Packet(QObject *parent = 0);

    virtual void executePacket();

    virtual void unserializePacket();
    virtual QString serializePacket();


signals:

public slots:

};

#endif // PACKET_H
