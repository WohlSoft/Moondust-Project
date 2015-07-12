#ifndef RAWPACKETDECODER_H
#define RAWPACKETDECODER_H

#include <memory>
#include <future>
#include <atomic>
#include <condition_variable>

#include <ConnectionLib/Shared/util/ThreadedQueue.h>
#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include <ConnectionLib/Shared/pgenet_packetmanager.h>
#include <ConnectionLib/Server/user/pgenet_usermanager.h>

#include <ConnectionLib/Server/low-level/pgenetll_session.h>

#include <utility>

#include <asio.hpp>



using ThreadedQueue_UnindentifedPackets = ThreadedQueue<std::pair<std::shared_ptr<PGENETLL_Session>, Packet*> >;

class RawPacketDecoder
{
public:
    RawPacketDecoder(PGENET_DefPacketRegister* packetRegister, PGENET_UserManager* userManager);
    ~RawPacketDecoder();


    std::shared_ptr<ThreadedQueue_RawData> incomingPacketsQueue() const;
    std::shared_ptr<ThreadedQueue<Packet* > > fullPacketsQueue() const;
    std::shared_ptr<ThreadedQueue_UnindentifedPackets> fullPacketsUnindentified() const;

private:
    // ASYNC STUFF

    std::thread _asyncWorkerThread;
    void _asyncWorkerProc();

    // Input:
    std::shared_ptr<ThreadedQueue_RawData> m_incomingPackets;

    // Output:
    std::shared_ptr<ThreadedQueue<Packet* > > m_fullPackets;
    std::shared_ptr<ThreadedQueue_UnindentifedPackets> m_fullPacketsUnindentified;

    PGENET_DefPacketRegister* m_packetRegister;
    PGENET_UserManager* m_userManager;
};

#endif // RAWPACKETDECODER_H
