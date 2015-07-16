#include "pgenet_session.h"

PGENET_Session::PGENET_Session(PGENET_Session::SessionLoopType type)
{
    m_packetLoopThread = std::thread([this](){runPacketLoop();});
    if(type == PGENET_Session::SESSIONTYPE_PacketAndUnindentifiedPacket){
        m_packetUnindentifiedLoopThread = std::thread([this](){runPacketUnindentifiedLoop();});
    }
    m_type = type;
}

PGENET_Session::~PGENET_Session()
{
    m_nextPacketQueue.doExit();
    m_packetLoopThread.join();
    if(m_type == PGENET_Session::SESSIONTYPE_PacketAndUnindentifiedPacket){
        m_nextPacketUnindentifiedQueue.doExit();
        m_packetUnindentifiedLoopThread.join();
    }
}

void PGENET_Session::receiveNextPacket(Packet *nextPacket)
{
    m_nextPacketQueue.push(nextPacket);
}

void PGENET_Session::receiveNextPacketUnindentified(std::pair<std::shared_ptr<PGENETLL_Session>, Packet*> nextPacket)
{
    m_nextPacketUnindentifiedQueue.push(nextPacket);
}

void PGENET_Session::runPacketLoop()
{
    for(;;){
        Packet* nextPacket = m_nextPacketQueue.pop();
        if(m_nextPacketQueue.shouldExit())
            return;

        manageNextPacket(std::shared_ptr<Packet>(nextPacket));
    }
}

void PGENET_Session::runPacketUnindentifiedLoop()
{
    for(;;){
        std::pair<std::shared_ptr<PGENETLL_Session>, Packet *> nextPacket = m_nextPacketUnindentifiedQueue.pop();
        if(m_nextPacketUnindentifiedQueue.shouldExit())
            return;

        manageNextPacketUnindentified(nextPacket.first, std::shared_ptr<Packet>(nextPacket.second));
    }
}

