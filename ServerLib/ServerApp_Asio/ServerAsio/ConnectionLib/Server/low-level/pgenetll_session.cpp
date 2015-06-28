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

void PGENETLL_Session::setIncomingTextFunc(const std::function<void (std::string)> &incomingTextFunc)
{
    m_incomingTextFunc = incomingTextFunc;
}
void PGENETLL_Session::setPacketToPush(const std::shared_ptr<ThreadedQueue<std::string> > &packetToPush)
{
    m_packetToPush = packetToPush;
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
            std::cout << std::string(m_dataBuf, length);
            // Do stuff
            if(m_incomingTextFunc)
                m_incomingTextFunc(std::string(m_dataBuf, length));

            if(m_packetToPush)
                m_packetToPush->push(std::string(m_dataBuf, length));

            // Listen more
            listen();

        }else{
            std::cout << "Error happened: " << ec.message() << std::endl;
        }

    });
}

