#ifndef RAWPACKETDECODER_H
#define RAWPACKETDECODER_H

#include <memory>
#include "ThreadedQueue.h"
#include <future>
#include <atomic>
#include <condition_variable>

class RawPacketDecoder
{
public:
    RawPacketDecoder();
    ~RawPacketDecoder();


    std::shared_ptr<ThreadedQueue<std::string> > incomingPacketsQueue() const;
    std::shared_ptr<ThreadedQueue<std::string> > fullPacketsQueue() const;

private:
    // ASYNC STUFF

    std::thread _asyncWorkerThread;
    void _asyncWorkerProc();

    std::shared_ptr<ThreadedQueue<std::string> > m_incomingPackets;
    std::shared_ptr<ThreadedQueue<std::string> > m_fullPackets;
};

#endif // RAWPACKETDECODER_H
