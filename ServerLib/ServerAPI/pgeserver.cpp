#include "pgeserver.h"
#include <utils/pgewriterutils.h>
#include <utils/pgemiscutils.h>
#include <QApplication>

PGEServer::PGEServer(QObject *parent) :
    PGEConnection(parent)
{
    init();
}

PGEServer::PGEServer(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) :
    PGEConnection(toRegisterPackets, parent)
{
    init();
}

PGEServer::~PGEServer()
{
    foreach(PGEConnectedUser* user, m_connectedUser){
        user->socket->disconnectFromHost();
    }
    qDeleteAll(m_connectedUser);

    foreach(QTcpSocket* socket, m_unindentifiedSockets){
        socket->disconnectFromHost();
    }
    m_server->close();
    m_server->deleteLater();
}


void PGEServer::dispatchPacket(Packet *packet)
{
    foreach(PGEConnectedUser* user, m_connectedUser){
        dispatchPacketToUser(user, packet);
    }
}

void PGEServer::dispatchPacketToUser(PGEConnectedUser *user, Packet *packet)
{
    warnIfQAppNotRunning();

    //First check for errors
    if(!m_server->isListening()){
        qWarning() << "Tried to sent a packet, without listening!";
        return;
    }

    //Then get packet type
    PGEPackets packetType = getPacketTypeByMetaObject(packet->metaObject());
    if(packetType == PGEPackets::PACKET_UNDEFINED){
        qWarning() << "Tried to sent unregistered packet.";
        return;
    }


    //Now write the packet stream
    QDataStream packetData(user->socket);
    //First write the packet-ID
    packetData.writeBytes(reinterpret_cast<const char*>(&packetType), sizeof(decltype(packetType)));

    //Now write the actual packet data
    QByteArray serializePacket = packet->serializePacket();

    //Write the data:
    // 1. the length of the data
    // 2. the actual data
    writeStreamData(packetData, serializePacket);

    //Be sure that the events get dispatched.
    qApp->processEvents();

    delete packet;
}




