#include "rawpacketdecoder.h"

#include <QDataStream>

RawPacketDecoder::RawPacketDecoder(PGENET_DefPacketRegister *packetRegister, PGENET_UserManager *userManager) :
    m_incomingPackets(new ThreadedQueue<std::string>()),
    m_fullPackets(new ThreadedQueue<std::shared_ptr<Packet> >()),
    m_packetRegister(packetRegister),
    m_userManager(userManager)
{
    _asyncWorkerThread = std::thread([this](){_asyncWorkerProc();});
}

RawPacketDecoder::~RawPacketDecoder()
{
    m_incomingPackets->push("_exit_now");
    _asyncWorkerThread.join();
}
std::shared_ptr<ThreadedQueue<std::string> > RawPacketDecoder::incomingPacketsQueue() const
{
    return m_incomingPackets;
}
std::shared_ptr<ThreadedQueue<std::shared_ptr<Packet> > > RawPacketDecoder::fullPacketsQueue() const
{
    return m_fullPackets;
}

void RawPacketDecoder::_asyncWorkerProc()
{
    while(true){
        std::string nextPacketPart = m_incomingPackets->pop();
        if(m_incomingPackets->shouldExit())
            return;

        // Do Packet Header...

        QDataStream packetStream(QByteArray(nextPacketPart.c_str(), nextPacketPart.size()));

        int packetID = 0;
        packetStream.readRawData((char*)&packetID, sizeof(packetID));

        int sizeOfUsername = 0;
        packetStream.readRawData((char*)&sizeOfUsername, sizeof(sizeOfUsername));

        QString username("");
        if(sizeOfUsername){
            std::wstring rawUsername(sizeOfUsername+1/2, '\0');
            packetStream.readRawData((char*)&rawUsername[0], sizeOfUsername);
            username = QString::fromStdWString(rawUsername);
        }

        int sessionID = 0;
        packetStream.readRawData((char*)&sessionID, sizeof(sessionID));

        Packet* newPacket = m_packetRegister->createPacketById(static_cast<PacketID>(packetID));
        newPacket->setUser(m_userManager->getUserByName(username));
        newPacket->setSessionID(sessionID);

        newPacket->decode(packetStream);

        m_fullPackets->push(std::shared_ptr<Packet>(newPacket));

    }
}



