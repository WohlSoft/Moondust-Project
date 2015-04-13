#include "pgeconnection.h"



PGEConnection::PGEConnection(QObject *parent) :
    QThread(parent)
{}


PGEConnection::PGEConnection(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) :
    QThread(parent)
{
    registerPackets(toRegisterPackets);
}


void PGEConnection::init()
{
    //Here the init code
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
    for(QMap<PGEPackets, QMetaObject *>::const_iterator it  = m_registeredPackets.begin(); it != m_registeredPackets.end(); ++it){
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

