#ifndef PGENET_SESSION_H
#define PGENET_SESSION_H

#include <QObject>

#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include <ConnectionLib/Shared/util/ThreadedQueue.h>
#include <memory>
#include <thread>

class PGENET_Session
{
    Q_DISABLE_COPY(PGENET_Session)
public:
    PGENET_Session();
    virtual ~PGENET_Session();

    void receiveNextPacket(Packet* nextPacket);

protected:
    virtual void manageNextPacket(std::shared_ptr<Packet> nextPacket) = 0;
private:
    // Async Stuff
    ThreadedQueue<Packet*> m_nextPacketQueue;
    std::thread m_packetLoopThread;
    void runPacketLoop();
};

#endif // PGENET_SESSION_H
