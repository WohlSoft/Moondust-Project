#ifndef PGENET_SESSION_H
#define PGENET_SESSION_H

#include <QObject>

#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include <ConnectionLib/Shared/util/ThreadedQueue.h>
#include <memory>
#include <thread>

#include <ConnectionLib/Server/low-level/pgenetll_session.h>

class PGENET_Session
{
    Q_DISABLE_COPY(PGENET_Session)
public:
    enum SessionLoopType {
        SESSIONTYPE_Packet,
        SESSIONTYPE_PacketAndUnindentifiedPacket
    };

    PGENET_Session(SessionLoopType type);
    virtual ~PGENET_Session();

    void receiveNextPacket(Packet* nextPacket);
    void receiveNextPacketUnindentified(std::pair<std::shared_ptr<PGENETLL_Session>, Packet*> nextPacket);

protected:
    virtual void manageNextPacket(std::shared_ptr<Packet> nextPacket) = 0;
    virtual void manageNextPacketUnindentified(std::shared_ptr<PGENETLL_Session> lowLevelClient, std::shared_ptr<Packet> nextPacket) = 0;
private:
    SessionLoopType m_type;

    // Async Stuff
    ThreadedQueue<Packet*> m_nextPacketQueue;
    std::thread m_packetLoopThread;
    void runPacketLoop();

    ThreadedQueue<std::pair<std::shared_ptr<PGENETLL_Session>, Packet*> > m_nextPacketUnindentifiedQueue;
    std::thread m_packetUnindentifiedLoopThread;
    void runPacketUnindentifiedLoop();
};

#endif // PGENET_SESSION_H
