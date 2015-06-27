#ifndef PGENETLL_SERVER_H
#define PGENETLL_SERVER_H

#include <asio.hpp>

using asio::ip::tcp;

class PGENETLL_Server
{
public:
    PGENETLL_Server(asio::io_service& io_service, short port);

    void startAccepting();

private:
    tcp::acceptor m_pgenetll_acceptor;
    tcp::socket m_pgenetll_nextsocket;
};

#endif // PGENETLL_SERVER_H
