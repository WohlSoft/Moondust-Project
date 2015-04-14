#include "pgeserver.h"


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
    connect(incomingSocket, SIGNAL(disconnected()), this, SLOT(socket_disconnect()));
}

void PGEServer::socket_receivePacket()
{
    //Here to check:
    // 1. Incoming handshake packet for indetifiy
    // 2. Normal packets of already connected users

    QTcpSocket* senderSocketObj = qobject_cast<QTcpSocket*>(sender());
    if(!senderSocketObj){
        qWarning() << "Invalid sender for slot socket_receivePacket()! Expected QTcpSocket, got " << sender()->metaObject()->className();
        return;
    }

    //start reading
    QDataStream socketStream(senderSocketObj);

    PGEPackets packetType;
    socketStream.readRawData(reinterpret_cast<char*>(&packetType), sizeof(decltype(packetType)));

    int lengthOfData;
    socketStream.readRawData(reinterpret_cast<char*>(&lengthOfData), sizeof(decltype(lengthOfData)));

    QByteArray data(lengthOfData, '\0');
    socketStream.readRawData(data.data(), lengthOfData);

    if(!m_registeredPackets.contains(packetType)){
        qWarning() << "Got unregistered packet, ignoring Packet";
        return;
    }

    QObject* possiblePacket = m_registeredPackets[packetType]->newInstance();
    if(!possiblePacket){
        qWarning() << "Could not create instance of " << m_registeredPackets[packetType]->className() << "! Did you forget Q_INVOKABLE?";
        return;
    }

    Packet* packet = qobject_cast<Packet*>(possiblePacket);
    if(!packet){
        qWarning() << "Failed qobject_cast for Packet-Class!";
        return;
    }

    packet->unserializePacket(data);

    // Here to create PGEConnectedUser
    if(packetType == PGEPackets::PACKET_HANDSHAKE){
        PacketHandshake* handshake = qobject_cast<PacketHandshake*>(packet);
        if(!handshake){
            qWarning() << "Failed qobject_cast for PacketHandshake-Class!";
            return;
        }

        bool foundUserInUnindentifiedSockets = false;
        for(int i = 0; i < m_unindentifiedSockets.size(); ++i){
            if(senderSocketObj == m_unindentifiedSockets[i]){
                foundUserInUnindentifiedSockets = true;
                m_unindentifiedSockets.removeAt(i);
                break;
            }
        }

        if(!foundUserInUnindentifiedSockets){
            for(int i = 0; i < m_connectedUser.size(); ++i){
                if(m_connectedUser[i]->socket == senderSocketObj){
                    qWarning() << "User \"" << m_connectedUser[i]->Username << "\" sent a packet for handshake, even though he is already marked as connected!";
                    return;
                }
            }
            qWarning() << "Did not found this user in the unindentifiedSockets-List and not in the connectedUser-List. How the heck is he connected O.o?";
            return;
        }

        PGEConnectedUser* newUser = handshake->createUserOfData();
        newUser->socket = senderSocketObj;

        delete handshake;
        emit userConnected(newUser);
    }else{
        m_packets << QSharedPointer<Packet>(packet);
        emit receivedData();
    }
}


void PGEServer::socket_disconnect()
{
    QTcpSocket* senderSocketObj = qobject_cast<QTcpSocket*>(sender());
    if(!senderSocketObj){
        qWarning() << "Invalid sender for slot socket_disconnect()! Expected QTcpSocket, got " << sender()->metaObject()->className();
        return;
    }

    m_unindentifiedSockets.removeAll(senderSocketObj);
    for(int i = 0; i < m_connectedUser.size(); i++){
        if(m_connectedUser[i]->socket == senderSocketObj){
            PGEConnectedUser* userDisconnecting = m_connectedUser[i];
            emit userDisconnected(userDisconnecting);

            userDisconnecting->socket->close();
            m_connectedUser.removeAt(i);
            delete userDisconnecting->socket;
            delete userDisconnecting;
            return;
        }
    }
}
