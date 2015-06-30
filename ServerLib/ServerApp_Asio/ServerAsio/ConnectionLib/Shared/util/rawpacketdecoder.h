#ifndef RAWPACKETDECODER_H
#define RAWPACKETDECODER_H

#include <memory>
#include "ThreadedQueue.h"
#include <future>
#include <atomic>
#include <condition_variable>

#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include <ConnectionLib/Shared/pgenet_packetmanager.h>
#include <ConnectionLib/Shared/user/pgenet_usermanager.h>

class RawPacketDecoder
{
public:
    RawPacketDecoder(PGENET_DefPacketRegister* packetRegister, PGENET_UserManager* userManager);
    ~RawPacketDecoder();


    std::shared_ptr<ThreadedQueue<std::string> > incomingPacketsQueue() const;
    std::shared_ptr<ThreadedQueue<std::shared_ptr<Packet> > > fullPacketsQueue() const;

private:
    // ASYNC STUFF

    std::thread _asyncWorkerThread;
    void _asyncWorkerProc();

    std::shared_ptr<ThreadedQueue<std::string> > m_incomingPackets;
    std::shared_ptr<ThreadedQueue<std::shared_ptr<Packet> > > m_fullPackets;

    PGENET_DefPacketRegister* m_packetRegister;
    PGENET_UserManager* m_userManager;
};

#endif // RAWPACKETDECODER_H
