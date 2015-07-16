#include "pgenetll_server.h"

#include <iostream>

PGENETLL_Server::PGENETLL_Server(asio::io_service& io_service, short port) :
    m_pgenetll_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    m_pgenetll_nextsocket(io_service)
{}


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
                newSession->setRawPacketToPush(m_rawPacketToPush);
                std::cout << "Incoming connection!" << std::endl;

                if(m_incomingConnectionHandler)
                    m_incomingConnectionHandler(newSession);

            }else{
                std::cout << "Error happened: " << ec.message() << std::endl;
            }
            startAccepting();
        });

}


void PGENETLL_Server::setRawPacketToPush(const std::shared_ptr<ThreadedQueue_RawData> &packetToPush)
{
    m_rawPacketToPush = packetToPush;
}
void PGENETLL_Server::setIncomingConnectionHandler(const std::function<void (std::shared_ptr<PGENETLL_Session>)> &value)
{
    m_incomingConnectionHandler = value;
}



