#include "pgeserver.h"

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

void PGEServer::init()
{
    m_server.moveToThread(this);
    m_server.listen(QHostAddress::Any, PGENetworkPort);
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(server_incomingConnection()));
}

void PGEServer::server_incomingConnection()
{
    QTcpSocket* incomingSocket = m_server.nextPendingConnection();
    m_unindentifiedSockets << incomingSocket;

    connect(incomingSocket, SIGNAL(readyRead()), this, SLOT(socket_receivePacket()));
}

void PGEServer::socket_receivePacket()
{
    //Here to check:
    // 1. Incoming handshake packet for indetifiy
    // 2. Normal packets of already connected users

    // Here to create PGEConnectedUser
}


void PGEServer::dispatchPacket(Packet *packet)
{
    foreach(PGEConnectedUser* user, m_connectedUser){
        dispatchPacketToUser(user, packet);
    }
}

void PGEServer::dispatchPacketToUser(PGEConnectedUser *user, Packet *packet)
{
    //First check for errors
    if(!m_server.isListening()){
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
    packetData.writeBytes(reinterpret_cast<const char*>(&packetType), sizeof(PGEPackets));

    //Now write the actual packet data
    QByteArray serializePacket = packet->serializePacket();

    //First write the length of the packet
    int sizeOfData = serializePacket.length();
    packetData.writeBytes(reinterpret_cast<const char*>(&sizeOfData), sizeof(decltype(sizeOfData)));
    //The write the acutal data
    packetData.writeBytes(serializePacket.data(), serializePacket.length());
}




