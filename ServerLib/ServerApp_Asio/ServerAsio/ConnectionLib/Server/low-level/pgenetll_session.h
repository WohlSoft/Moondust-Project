#ifndef PGENETLL_SESSION_H
#define PGENETLL_SESSION_H

#include <asio.hpp>


using asio::ip::tcp;

class PGENETLL_Session
  : public std::enable_shared_from_this<PGENETLL_Session>
{
public:
    PGENETLL_Session(tcp::socket socket);

    void start();


    void setIncomingTextFunc(const std::function<void (std::string)> &incomingTextFunc);

private:
    std::function<void(std::string)> m_incomingTextFunc;

    void listen();

    tcp::socket m_socket;
    enum { max_length = 1024 };
    char m_dataBuf[max_length];
};

#endif // PGENETLL_SESSION_H
