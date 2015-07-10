#include "pgenetll_session.h"
#include <iostream>
#include <memory>


PGENETLL_Session::PGENETLL_Session(tcp::socket socket) :
    m_socket(std::move(socket))
{

}

void PGENETLL_Session::start()
{
    listen();
}

void PGENETLL_Session::setRawPacketToPush(const std::shared_ptr<ThreadedQueue_RawData> &packetToPush)
{
    m_rawPacketToPush = packetToPush;
}

void PGENETLL_Session::listen()
{
    auto self(shared_from_this());

    m_socket.async_read_some(asio::buffer(m_dataBuf, max_length),
                             [this, self](std::error_code ec, std::size_t length)
    {
        if (ec == asio::error::connection_reset || ec == asio::error::eof){
            std::cout << "Connection closed!" << std::endl;
            return;
        }
        if (!ec)
        {
            int size = 0;
            memcpy((void*)&size, (void*)&m_dataBuf, 4);

            std::cout << "[Receive Size] size: " << size << std::endl;

            std::string buf(size, '\0');
            asio::read(m_socket, asio::buffer(&buf[0], size));

            m_rawPacketToPush->push(make_pair(self, buf));

            // Listen more
            listen();
        }else{
            std::cout << "Error happened: " << ec.message() << std::endl;
        }

    });
}

