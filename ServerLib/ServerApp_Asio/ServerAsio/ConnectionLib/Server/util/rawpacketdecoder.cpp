#include "rawpacketdecoder.h"

#include <QDataStream>
#include <iostream>

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

        // Do Packet Header...

        QDataStream packetStream(QByteArray(nextPacketPart.second.c_str(), nextPacketPart.second.size()));

        int packetID;
        QString username;
        int sessionID;

        packetStream >> packetID >> username >> sessionID;

        std::cout << "Incoming Packet: " << std::endl;
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

        newPacket->decode(packetStream);

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




