#include "pgenet_server.h"

#include "../Shared/pgenet_global.h"
#include <iostream>

PGENET_Server::PGENET_Server(QObject *parent) :
    QObject(parent),

    m_currentState(PGENET_ServerState::Closed),

    m_pckDecoder(getPacketRegister(), &m_userManager),

    m_globalSession(this),

    m_service(new asio::io_service()),
    m_llserver(*m_service, PGENET_Global::Port)
{
    m_llserver.setRawPacketToPush(m_pckDecoder.incomingPacketsQueue());
    m_fullPackets = m_pckDecoder.fullPacketsQueue();
    m_fullPacketsUnindentified = m_pckDecoder.fullPacketsUnindentified();

    //New connection should be handled by the user manager.
    m_llserver.setIncomingConnectionHandler(m_userManager.getNewIncomingConnectionHandler());
}

PGENET_Server::~PGENET_Server()
{
    if(m_currentState == PGENET_ServerState::Running){
        _bgWorker_quit();
        m_service->stop();
        _ioServiceState.waitForFinished();
        _bgWorkerState_FullPackets.waitForFinished();
        _bgWorkerState_FullPacketsUnindentified.waitForFinished();
    }
}

void PGENET_Server::start()
{
    if(m_currentState == PGENET_ServerState::Running){
        qWarning() << "Trying to start a running server";
        return;
    }
    m_llserver.startAccepting();
    _ioServiceState = QtConcurrent::run([this](){ _ioService_run(); });
    _bgWorkerState_FullPackets = QtConcurrent::run([this](){ _bgWorker_WaitForIncomingFullPackets(); });
    _bgWorkerState_FullPacketsUnindentified = QtConcurrent::run([this](){ _bgWorker_WaitForIncomingFullPacketsUnindentified(); });
    m_currentState = PGENET_ServerState::Running;
}


PGENET_Server::PGENET_ServerState PGENET_Server::currentState() const
{
    return m_currentState;
}

PGENET_UserManager *PGENET_Server::getUserManager()
{
    return &m_userManager;
}

void PGENET_Server::_ioService_run()
{
    try {
        m_service->run();
    } catch(std::exception& e) {
        std::cout << "Io Service Exception: " << e.what() << std::endl;
    }
}

void PGENET_Server::_bgWorker_quit()
{
    m_fullPackets->doExit();
    m_fullPacketsUnindentified->doExit();
}

void PGENET_Server::_bgWorker_WaitForIncomingFullPackets()
{
    for (;;) {
        // TODO: Spread the packet to the different sessions.
        Packet* nextFullPacket = m_fullPackets->pop();
        if(m_fullPackets->shouldExit())
            return;

        int sessionID = nextFullPacket->getSessionID();
        if(sessionID == 0){
            m_globalSession.receiveNextPacket(nextFullPacket);
        }else if(m_registeredSessions.contains(sessionID)){
            m_registeredSessions[sessionID]->receiveNextPacket(nextFullPacket);
        }
    }
}

void PGENET_Server::_bgWorker_WaitForIncomingFullPacketsUnindentified()
{
    for (;;) {

        std::pair<std::shared_ptr<PGENETLL_Session>, Packet*> nextFullPacket = m_fullPacketsUnindentified->pop();
        if(m_fullPacketsUnindentified->shouldExit())
            return;

        int sessionID = nextFullPacket.second->getSessionID();
        if(sessionID == 0){
            m_globalSession.receiveNextPacketUnindentified(nextFullPacket);
        }else if(m_registeredSessions.contains(sessionID)){
            m_registeredSessions[sessionID]->receiveNextPacketUnindentified(nextFullPacket);
        }else{
            delete nextFullPacket.second;
        }
    }
}

