#include "pgenetll_session.h"

#include <iostream>

PGENETLL_Session::PGENETLL_Session(tcp::socket socket) :
    m_socket(std::move(socket))
{

}

void PGENETLL_Session::start()
{
    listen();
}

void PGENETLL_Session::listen()
{
    auto self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_dataBuf, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                // Do stuff
                std::cout << m_dataBuf << std::endl;
                // Listen more
                listen();
            }
        });
}

