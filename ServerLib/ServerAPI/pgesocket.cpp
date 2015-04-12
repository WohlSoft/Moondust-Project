#include "pgesocket.h"



PGESocket::PGESocket(QObject *parent) :
    QThread(parent)
{}

PGESocket::PGESocket(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) :
    QThread(parent)
{
    registerPackets(toRegisterPackets);
}

void PGESocket::registerPackets(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets)
{
    for(QMap<PGEPackets, QMetaObject *>::const_iterator it  = toRegisterPackets.begin(); it != toRegisterPackets.end(); ++it){
        registerPacket(it.value(), it.key());
    }
}

void PGESocket::registerPacket(QMetaObject *packetMetaObject, PGEPackets packetType)
{
    m_registeredPackets[packetType] = packetMetaObject;
}
