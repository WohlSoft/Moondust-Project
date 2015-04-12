#ifndef PGESOCKET_H
#define PGESOCKET_H

#include <QObject>
#include "pgesocketdefines.h"
#include <QMap>
#include <QThread>

class PGESocket : public QThread
{
    Q_OBJECT
public:

    //This is the default constructor
    explicit PGESocket(QObject *parent = 0);
    //This is the constructor + registerPackets in one go
    explicit PGESocket(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);

    //Here you can register your packets
    void registerPackets(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets);
    void registerPacket(QMetaObject* packetMetaObject, PGEPackets packetType);

signals:

public slots:

private:
    QMap<PGEPackets, QMetaObject*> m_registeredPackets;

};

#endif // PGESOCKET_H
