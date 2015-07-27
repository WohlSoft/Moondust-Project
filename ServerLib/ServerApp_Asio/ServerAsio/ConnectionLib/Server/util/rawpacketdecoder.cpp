#include "rawpacketdecoder.h"

#include <QDataStream>
#include <iostream>
#include <QJsonDocument>

#include <ConnectionLib/Shared/util/threadedlogger.h>

RawPacketDecoder::RawPacketDecoder(PGENET_DefPacketRegister *packetRegister, PGENET_UserManager *userManager) :
    m_incomingPackets(new ThreadedQueue_RawData()),
    m_fullPackets(new ThreadedQueue<Packet* >()),
    m_fullPacketsUnindentified(new ThreadedQueue_UnindentifedPackets()),
    m_packetRegister(packetRegister),
    m_userManager(userManager)
{
    _asyncWorkerThread = std::thread([this](){_asyncWorkerProc();});
}

RawPacketDecoder::~RawPacketDecoder()
{
    m_incomingPackets->doExit();
    _asyncWorkerThread.join();
}

std::shared_ptr<ThreadedQueue_RawData> RawPacketDecoder::incomingPacketsQueue() const
{
    return m_incomingPackets;
}

std::shared_ptr<ThreadedQueue<Packet* > > RawPacketDecoder::fullPacketsQueue() const
{
    return m_fullPackets;
}

void RawPacketDecoder::_asyncWorkerProc()
{
    while(true){
        std::pair<std::shared_ptr<PGENETLL_Session>, std::string> nextPacketPart = m_incomingPackets->pop();
        if(m_incomingPackets->shouldExit())
            return;

        // Load the json content
        QJsonParseError possibleError;

        QJsonDocument data = QJsonDocument::fromJson(QByteArray(nextPacketPart.second.c_str(), nextPacketPart.second.size()), &possibleError);
        if(possibleError.error != QJsonParseError::NoError){
            gThreadedLogger->logWarning("Failed to parse JSON packet: " + possibleError.errorString());
            continue;
        }

        QJsonValue headerValue = data.object().value("header");
        if(headerValue.type() != QJsonValue::Object){
            gThreadedLogger->logWarning("JSON Error: header doesn't exist or has wrong type! The header value is type-id: " + (int)headerValue.type());
            continue;
        }
        QJsonObject headerObject = headerValue.toObject();

        QJsonValue packetValue = data.object().value("packet");
        if(packetValue .type() != QJsonValue::Object){
            gThreadedLogger->logWarning("JSON Error: packet doesn't exist or has wrong type! The header value is type-id: " + (int)packetValue.type());
            continue;
        }
        QJsonObject packetObject = packetValue.toObject();

        // QDataStream packetStream(QByteArray(nextPacketPart.second.c_str(), nextPacketPart.second.size()));

        int packetID;
        QString username;
        int sessionID;

        if(!headerObject.contains("packetID")){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"packetID\"");
            continue;
        }
        if(!headerObject.value("packetID").isDouble()){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"packetID\" with value double. Instead the type is: " + (int)headerObject.value("packetID").type());
            continue;
        }


        if(!headerObject.contains("username")){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"username\"");
            continue;
        }
        if(!headerObject.value("username").isString()){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"username\" with value string. Instead the type is: " + (int)headerObject.value("username").type());
            continue;
        }


        if(!headerObject.contains("sessionID")){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"sessionID\"");
            continue;
        }
        if(!headerObject.value("sessionID").isDouble()){
            gThreadedLogger->logWarning("JSON Error: header does not have key \"sessionID\" with value double. Instead the type is: " + (int)headerObject.value("sessionID").type());
            continue;
        }

        packetID = (int)headerObject.value("packetID").toDouble();
        username = headerObject.value("username").toString();
        sessionID = (int)headerObject.value("sessionID").toDouble();

        std::cout << "Incoming Packet (with valid header): " << std::endl;
        std::cout << "packetID: " << packetID << std::endl;
        std::cout << "username: " << username.toStdString().c_str() << std::endl;
        std::cout << "sessionID: " << sessionID << std::endl;
        std::cout << std::endl;


        Packet* newPacket = m_packetRegister->createPacketById(static_cast<PacketID>(packetID));
        if(newPacket == nullptr){
            std::cout << "Invalid Packet!" << std::endl;
            continue;
        }
        newPacket->setUser(m_userManager->getUserByName(username));
        newPacket->setSessionID(sessionID);
        newPacket->decode(packetObject);

        if(username.isEmpty()){
            m_fullPacketsUnindentified->push(make_pair(nextPacketPart.first, newPacket));
        }else{
            m_fullPackets->push(newPacket);
        }

    }
}
std::shared_ptr<ThreadedQueue_UnindentifedPackets> RawPacketDecoder::fullPacketsUnindentified() const
{
    return m_fullPacketsUnindentified;
}




