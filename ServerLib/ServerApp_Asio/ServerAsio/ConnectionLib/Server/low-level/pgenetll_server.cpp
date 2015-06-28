#include "pgenetll_server.h"

#include "pgenetll_session.h"

PGENETLL_Server::PGENETLL_Server(asio::io_service& io_service, short port) :
    m_pgenetll_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    m_pgenetll_nextsocket(io_service)
{

}

#include <iostream>

void PGENETLL_Server::startAccepting()
{

    std::cout << "Start listening!" << std::endl;
    m_pgenetll_acceptor.async_accept(m_pgenetll_nextsocket,
        [this](std::error_code ec)
        {
            if (!ec)
            {
                std::shared_ptr<PGENETLL_Session> newSession = std::make_shared<PGENETLL_Session>(std::move(m_pgenetll_nextsocket));
                newSession->start();
                //newSession->setIncomingTextFunc(m_incomingTextFunc);
                newSession->setPacketToPush(m_packetToPush);
                std::cout << "Incoming connection!" << std::endl;
            }else{
                std::cout << "Error happened: " << ec.message() << std::endl;
            }
            startAccepting();
        });

}


void PGENETLL_Server::setIncomingTextFunc(const std::function<void (std::string)> &incomingTextFunc)
{
    m_incomingTextFunc = incomingTextFunc;
}
void PGENETLL_Server::setPacketToPush(const std::shared_ptr<ThreadedQueue<std::string> > &packetToPush)
{
    m_packetToPush = packetToPush;
}


