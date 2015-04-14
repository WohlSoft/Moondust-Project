#include "pgeconnection.h"



PGEConnection::PGEConnection(QObject *parent) :
    QThread(parent)
{
    init();
}


PGEConnection::PGEConnection(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) :
    QThread(parent)
{
    init();
    registerPackets(toRegisterPackets);
}


void PGEConnection::init()
{
    m_registeredPackets[PGEPackets::PACKET_HANDSHAKE] = &PacketHandshake::staticMetaObject;
}

void PGEConnection::registerPackets(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets)
{
    for(QMap<PGEPackets, QMetaObject *>::const_iterator it  = toRegisterPackets.begin(); it != toRegisterPackets.end(); ++it){
        registerPacket(it.value(), it.key());
    }
}

void PGEConnection::registerPacket(QMetaObject *packetMetaObject, PGEPackets packetType)
{
    if(QString(packetMetaObject->className()) == "Packet"){
        qWarning() << "Tried to register base packet class!";
        return;
    }
    if(packetType == PGEPackets::PACKET_UNDEFINED){
        qWarning() << "Tried to register PACKET_UNDEFINED";
        return;
    }
    if(packetType == PGEPackets::PACKET_HANDSHAKE){
        qWarning() << "Tried to register PACKET_HANDSHAKE";
        return;
    }



    if(isBaseOfPacket(packetMetaObject))
        m_registeredPackets[packetType] = packetMetaObject;
}


void PGEConnection::execAllPacket()
{
    for(int i = 0; i < m_packets.size(); i++)
        m_packets[i]->executePacket();

    m_packets.clear();
}

PGEPackets PGEConnection::getPacketTypeByMetaObject(const QMetaObject *packetObject)
{
    //I would rather use decltype(m_registeredPackets), but QtCreator says no: https://bugreports.qt.io/browse/QTCREATORBUG-13726
    for(QMap<PGEPackets, const QMetaObject *>::const_iterator it  = m_registeredPackets.cbegin(); it != m_registeredPackets.cend(); ++it){
        if(it.value() == packetObject)
            return it.key();
    }
    return PGEPackets::PACKET_UNDEFINED;
}

bool PGEConnection::isBaseOfPacket(QMetaObject *possiblePacket)
{
    const QMetaObject* superObj = possiblePacket;
    while((superObj = superObj->superClass())){
        if(QString(superObj->className()) == "Packet"){
            return true;
        }
    }

    qWarning() << possiblePacket->className() << " doesn't belong to the Packet-Class!";
    return false;
}

