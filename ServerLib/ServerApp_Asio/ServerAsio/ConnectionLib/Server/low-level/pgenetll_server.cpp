#include "pgenetll_server.h"

#include "pgenetll_session.h"

PGENETLL_Server::PGENETLL_Server(asio::io_service& io_service, short port) :
    m_pgenetll_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    m_pgenetll_nextsocket(io_service)
{

}



void PGENETLL_Server::startAccepting()
{
    m_pgenetll_acceptor.async_accept(m_pgenetll_nextsocket,
        [this](std::error_code ec)
        {
            if (!ec)
            {
                std::shared_ptr<PGENETLL_Session> newSession = std::make_shared<PGENETLL_Session>(std::move(m_pgenetll_nextsocket));
                newSession->start();

            }
            startAccepting();
        });
}
