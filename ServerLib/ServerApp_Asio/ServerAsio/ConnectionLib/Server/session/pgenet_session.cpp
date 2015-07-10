#include "pgenet_session.h"

PGENET_Session::PGENET_Session()
{
    m_packetLoopThread = std::thread([this](){runPacketLoop();});
}

PGENET_Session::~PGENET_Session()
{
    m_nextPacketQueue.doExit();
    m_packetLoopThread.join();
}

void PGENET_Session::receiveNextPacket(Packet *nextPacket)
{
    m_nextPacketQueue.push(nextPacket);
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

