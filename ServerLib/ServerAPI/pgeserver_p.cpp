#include "pgeserver.h"
#include <QApplication>

#include <utils/pgemiscutils.h>
#include <QMetaEnum>


void PGEServer::init()
{
    qDebug() << "Starting Server Thread...";
    this->start();
}

void PGEServer::server_incomingConnection()
{
    QTcpSocket* incomingSocket = m_server->nextPendingConnection();
    m_unindentifiedSockets << incomingSocket;

    connect(incomingSocket, SIGNAL(readyRead()), this, SLOT(socket_receivePacket()));
    connect(incomingSocket, SIGNAL(disconnected()), this, SLOT(socket_disconnect()));

}

void PGEServer::serversocket_reportError(QAbstractSocket::SocketError error)
{
    int indexOfSocketErrorEnum = QAbstractSocket::staticMetaObject.indexOfEnumerator("SocketError");
    QMetaEnum socketErrorEnum = QAbstractSocket::staticMetaObject.enumerator(indexOfSocketErrorEnum);
    const char* keyName = socketErrorEnum.valueToKey(static_cast<int>(error));

    QObject* senderObj = sender();
    if(!senderObj){
        qWarning() << "Sender for serversocket_reportError is invalid!";
        return;
    }

    QTcpServer* possibleServer = 0;
    QTcpSocket* possibleSocket = 0;

    if((possibleSocket = qobject_cast<QTcpSocket*>(senderObj))){
        PGEConnectedUser* user = getPGEUserBySocket(qobject_cast<QTcpSocket*>(senderObj));
        if(user){
            qCritical() << "The socket of connected user \"" << user->Username << "\" report error: " << keyName;
        }else{
            qCritical() << "<Unknown socket> report error: " << keyName;
        }
        qCritical() << "Error text: " << possibleSocket->errorString();
    }else if((possibleServer = qobject_cast<QTcpServer*>(senderObj))){
        qCritical() << "Server report error: " << keyName;
        qCritical() << "Error text: " << possibleServer->errorString();
    }else{
        qCritical() << senderObj->metaObject()->className() << " report error: " << keyName;
    }
}

void PGEServer::socket_receivePacket()
{
    warnIfQAppNotRunning();
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

        //Now check the list with the unindetified sockets
        bool foundUserInUnindentifiedSockets = false;
        for(int i = 0; i < m_unindentifiedSockets.size(); ++i){
            if(senderSocketObj == m_unindentifiedSockets[i]){
                foundUserInUnindentifiedSockets = true;
                //IF found then remove him from the "unindetified sockets"-list
                m_unindentifiedSockets.removeAt(i);
                break;
            }
        }

        //If NOT found in the unindetified socket list:
        if(!foundUserInUnindentifiedSockets){
            //Check if he is already in the connected state and hit out a warning
            for(int i = 0; i < m_connectedUser.size(); ++i){
                if(m_connectedUser[i]->socket == senderSocketObj){
                    qWarning() << "User \"" << m_connectedUser[i]->Username << "\" sent a packet for handshake, even though he is already marked as connected!";
                    return;
                }
            }
            qWarning() << "Did not found this user in the unindentifiedSockets-List and not in the connectedUser-List. How the heck is he connected O.o?";
            return;
        }

        //If everything fine then register him to the server.
        PGEConnectedUser* newUser = handshake->createUserOfData();
        newUser->socket = senderSocketObj;
        m_connectedUser << newUser;

        delete handshake;
        emit userConnected(newUser);

        qApp->processEvents();

        //Now write the response packet.
        QObject* packetObj = m_registeredPackets[PGEPackets::PACKET_HANDSHAKE_ACCEPTED]->newInstance();
        if(!packetObj){
            qWarning() << "Cannot write response handshake: PACKET_HANDSHAKE_ACCEPTED has no invokeable constructors.";
        }

        PacketHandshakeAccepted* responsePacket = qobject_cast<PacketHandshakeAccepted*>(packetObj);
        //maybe add data to the responePacket later

        //now send the packet
        dispatchPacketToUser(newUser, responsePacket);
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

void PGEServer::run()
{
    qDebug() << "Starting server...";
    m_server = new QTcpServer();
    m_server->moveToThread(this);
    m_server->listen(QHostAddress::Any, PGENetworkPort);
    qDebug() << "Server is now running at port " << PGENetworkPort;
    connect(m_server, SIGNAL(newConnection()), this, SLOT(server_incomingConnection()));
    connect(m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(serversocket_reportError(QAbstractSocket::SocketError)));
    (void) exec();
}


PGEConnectedUser *PGEServer::getPGEUserBySocket(QTcpSocket *socket)
{
    foreach (PGEConnectedUser* nextUser, m_connectedUser) {
        if(nextUser->socket == socket)
            return nextUser;
    }
    return nullptr;
}
