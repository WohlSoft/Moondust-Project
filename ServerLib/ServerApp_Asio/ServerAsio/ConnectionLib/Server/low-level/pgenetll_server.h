#ifndef PGENETLL_SERVER_H
#define PGENETLL_SERVER_H

#include <asio.hpp>
#include <functional>

using asio::ip::tcp;

class PGENETLL_Server
{
public:
    PGENETLL_Server(asio::io_service& io_service, short port);

    void startAccepting();



    void setIncomingTextFunc(const std::function<void (std::string)> &incomingTextFunc);

private:
    std::function<void(std::string)> m_incomingTextFunc;
    tcp::acceptor m_pgenetll_acceptor;
    tcp::socket m_pgenetll_nextsocket;
};

#endif // PGENETLL_SERVER_H
