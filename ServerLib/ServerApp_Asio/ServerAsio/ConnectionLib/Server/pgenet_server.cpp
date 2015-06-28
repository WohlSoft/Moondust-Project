#include "pgenet_server.h"

#include "../Shared/pgenet_global.h"
#include <iostream>

PGENET_Server::PGENET_Server(QObject *parent) :
    QObject(parent),

    m_currentState(PGENET_ServerState::Closed),

    _bgWorker_buf(""),
    _bgWorker_shouldQuit(false),

    m_service(new asio::io_service()),
    m_llserver(*m_service, PGENET_Global::Port)
{
    m_llserver.setIncomingTextFunc([this](std::string message){_bgWorker_NewMessage(message);});
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



void PGENET_Server::_bgWorker_NewMessage(std::string message)
{
    std::unique_lock<std::mutex> locker(_bgWorker_mutex);
    _bgWorker_buf = message;
    _bgWorker_alert.notify_all();
}

void PGENET_Server::_bgWorker_quit()
{
    std::unique_lock<std::mutex> locker(_bgWorker_mutex);
    _bgWorker_shouldQuit.store(true);
    _bgWorker_alert.notify_all();
}

void PGENET_Server::_bgWorker_WaitForIncoming()
{
    std::unique_lock<std::mutex> locker(_bgWorker_mutex);
    for (;;) {
        _bgWorker_alert.wait(locker);
        if(_bgWorker_shouldQuit.load())
            return;

        if(_bgWorker_buf != ""){
            QString message = QString(_bgWorker_buf.c_str());
            emit incomingText(message);
        }
    }
}

