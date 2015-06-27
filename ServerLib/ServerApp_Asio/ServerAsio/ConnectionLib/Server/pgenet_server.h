#ifndef PGENET_SERVER_H
#define PGENET_SERVER_H

#include <asio.hpp>
#include <asio/io_service.hpp>

#include "low-level/pgenetll_server.h"

using asio::ip::tcp;

class PGENET_Server
{
public:
    PGENET_Server();
    void start();

private:
    asio::io_service m_service;
    PGENETLL_Server m_llserver;
};

#endif // PGENET_SERVER_H
