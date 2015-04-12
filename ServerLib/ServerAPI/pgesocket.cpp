#include "pgesocket.h"



PGESocket::PGESocket(QObject *parent) : PGESocket(PGESocketType::PGEServer, parent) {}
PGESocket::PGESocket(PGESocket::PGESocketType type, QObject *parent) :
    QThread(parent),
    m_socketType(type),
    m_server(0),
    m_socket(0)
{
    init();
}

PGESocket::PGESocket(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) : PGESocket(toRegisterPackets, PGESocketType::PGEServer, parent) {}
PGESocket::PGESocket(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, PGESocket::PGESocketType type, QObject *parent) :
    QThread(parent),
    m_socketType(type),
    m_server(0),
    m_socket(0)
{
    registerPackets(toRegisterPackets);
    init();
}

void PGESocket::init()
{
    //Here the init code
}

void PGESocket::registerPackets(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets)
{
    for(QMap<PGEPackets, QMetaObject *>::const_iterator it  = toRegisterPackets.begin(); it != toRegisterPackets.end(); ++it){
        registerPacket(it.value(), it.key());
    }
}

void PGESocket::registerPacket(QMetaObject *packetMetaObject, PGEPackets packetType)
{
    if(isBaseOfPacket(packetMetaObject))
        m_registeredPackets[packetType] = packetMetaObject;
}

void PGESocket::run()
{

}



void PGESocket::dispatchPacket(Packet *packet)
{
    QString packetData = packet->serializePacket();


}

void PGESocket::execAllPacket()
{

}

bool PGESocket::isBaseOfPacket(QMetaObject *possiblePacket)
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

