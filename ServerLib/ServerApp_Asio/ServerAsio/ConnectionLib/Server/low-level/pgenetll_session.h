#ifndef PGENETLL_SESSION_H
#define PGENETLL_SESSION_H

#include <asio.hpp>
#include <ConnectionLib/Shared/util/ThreadedQueue.h>

using asio::ip::tcp;

class PGENETLL_Session
  : public std::enable_shared_from_this<PGENETLL_Session>
{
public:
    PGENETLL_Session(tcp::socket socket);

    void start();


    void setIncomingTextFunc(const std::function<void (std::string)> &incomingTextFunc);
    void setPacketToPush(const std::shared_ptr<ThreadedQueue<std::string> > &packetToPush);

private:
    std::function<void(std::string)> m_incomingTextFunc;
    std::shared_ptr<ThreadedQueue<std::string> > m_packetToPush;

    void listen();

    tcp::socket m_socket;
    enum { max_length = 4 };
    char m_dataBuf[max_length];
};

#endif // PGENETLL_SESSION_H
