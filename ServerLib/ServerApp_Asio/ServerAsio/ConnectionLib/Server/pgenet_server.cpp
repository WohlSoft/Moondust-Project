#include "pgenet_server.h"

#include "../Shared/pgenet_global.h"
#include <iostream>

PGENET_Server::PGENET_Server(QObject *parent) :
    QObject(parent),

    m_currentState(PGENET_ServerState::Closed),

    m_pckDecoder(getPacketRegister(), &m_userManager),

    m_globalSession(&m_userManager),

    m_service(new asio::io_service()),
    m_llserver(*m_service, PGENET_Global::Port)
{
    //m_llserver.setIncomingTextFunc([this](std::string message){_bgWorker_NewMessage(message);});
    m_llserver.setRawPacketToPush(m_pckDecoder.incomingPacketsQueue());
    m_fullPackets = m_pckDecoder.fullPacketsQueue();
}

PGENET_Server::~PGENET_Server()
{
    if(m_currentState == PGENET_ServerState::Running){
        _bgWorker_quit();
        m_service->stop();
        _ioServiceState.waitForFinished();
        _bgWorkerState.waitForFinished();
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
    _bgWorkerState = QtConcurrent::run([this](){ _bgWorker_WaitForIncoming(); });
    m_currentState = PGENET_ServerState::Running;
}


PGENET_Server::PGENET_ServerState PGENET_Server::currentState() const
{
    return m_currentState;
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
}

void PGENET_Server::_bgWorker_WaitForIncoming()
{
    for (;;) {
        // TODO: Spread the packet to the diffrent sessions.
        std::shared_ptr<Packet> nextFullPacket = m_fullPackets->pop();
        if(m_fullPackets->shouldExit())
            return;

        int sessionID = nextFullPacket->getSessionID();
        if(sessionID == 0){

        }else if(m_registeredSessions.contains(sessionID)){

        }
    }
}

