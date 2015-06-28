#include "rawpacketdecoder.h"

RawPacketDecoder::RawPacketDecoder() :
    m_incomingPackets(new ThreadedQueue<std::string>()),
    m_fullPackets(new ThreadedQueue<std::string>())
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
std::shared_ptr<ThreadedQueue<std::string> > RawPacketDecoder::fullPacketsQueue() const
{
    return m_fullPackets;
}

void RawPacketDecoder::_asyncWorkerProc()
{
    while(true){
        std::string nextPacketPart = m_incomingPackets->pop();
        if(nextPacketPart == "_exit_now")
            return;

        // Do Packet together...

        // In the meanwhile I directly push the packet.
        m_fullPackets->push(nextPacketPart);
    }
}



