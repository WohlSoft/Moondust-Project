#include "pgenet_globalsession.h"

#include "../pgenet_server.h"
#include <iostream>

#include <ConnectionLib/Shared/util/threadedlogger.h>

PGENET_GlobalSession::PGENET_GlobalSession(PGENET_Server *server) :
    PGENET_Session(PGENET_Session::SESSIONTYPE_PacketAndUnindentifiedPacket),
    m_server(server)
{}

void PGENET_GlobalSession::managePacketAuth(std::shared_ptr<PGENETLL_Session> session, PacketUserAuth *auth)
{
    if(!auth) //FIXME: Handle Warning.
        return;

    gThreadedLogger->logDebug(QString("New Auth Username: ") + auth->username());
    gThreadedLogger->logDebug(QString("New Auth Network Number: ") + QString::number(auth->networkVersionNumber()));

    m_server->getUserManager()->registerUser(auth->username(), session);
}


void PGENET_GlobalSession::manageNextPacket(std::shared_ptr<Packet> nextPacket)
{

}

void PGENET_GlobalSession::manageNextPacketUnindentified(std::shared_ptr<PGENETLL_Session> lowLevelClient, std::shared_ptr<Packet> nextPacket)
{
    switch (static_cast<PacketID>(nextPacket->getPacketID())) {
    case PacketID::PGENET_PacketUserAuth: managePacketAuth(lowLevelClient, dynamic_cast<PacketUserAuth*>(nextPacket.get())); break;
    default:
        break;
    }
}
