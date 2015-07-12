#ifndef PGENETLL_SESSION_H
#define PGENETLL_SESSION_H

#include <asio.hpp>
#include <ConnectionLib/Shared/util/ThreadedQueue.h>

class PGENETLL_Session;

using asio::ip::tcp;
using ThreadedQueue_RawData = ThreadedQueue<std::pair<std::shared_ptr<PGENETLL_Session>, std::string> >;

class PGENETLL_Session
  : public std::enable_shared_from_this<PGENETLL_Session>
{
public:
    PGENETLL_Session(tcp::socket socket);

    void start();
    void setRawPacketToPush(const std::shared_ptr<ThreadedQueue_RawData> &packetToPush);

private:
    std::shared_ptr<ThreadedQueue_RawData> m_rawPacketToPush;

    void listen();

    tcp::socket m_socket;
    enum { max_length = 4 };
    char m_dataBuf[max_length];
};

#endif // PGENETLL_SESSION_H
