#ifndef PGENET_GLOBALSESSION_H
#define PGENET_GLOBALSESSION_H

#include "pgenet_session.h"
#include <ConnectionLib/Server/user/pgenet_usermanager.h>
#include <memory>

#include <ConnectionLib/Shared/packetV2/packets/ClientToServer/packetuserauth.h>

class PGENET_Server;

//The global session mainly handles incoming connections.

class PGENET_GlobalSession : public PGENET_Session
{
    Q_DISABLE_COPY(PGENET_GlobalSession)
public:
    PGENET_GlobalSession(PGENET_Server* server);
private:
    PGENET_Server* m_server;

    void managePacketAuth(std::shared_ptr<PGENETLL_Session> session, PacketUserAuth* auth);

    // PGENET_Session interface
protected:
    void manageNextPacket(std::shared_ptr<Packet> nextPacket);
    void manageNextPacketUnindentified(std::shared_ptr<PGENETLL_Session> lowLevelClient, std::shared_ptr<Packet> nextPacket);
};

#endif // PGENET_GLOBALSESSION_H
