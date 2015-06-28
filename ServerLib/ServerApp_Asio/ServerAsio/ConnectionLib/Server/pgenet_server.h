#ifndef PGENET_SERVER_H
#define PGENET_SERVER_H

#include <mutex>
#include <condition_variable>
#include <atomic>

#include <asio.hpp>
#include <asio/io_service.hpp>

#include <QScopedPointer>


#include <QObject>


#include <QtConcurrent>

#include "low-level/pgenetll_server.h"
#include <ConnectionLib/Shared/util/rawpacketdecoder.h>

using asio::ip::tcp;

class PGENET_Server : public QObject
{
    Q_OBJECT

public:
    PGENET_Server(QObject* parent = 0);
    virtual ~PGENET_Server();

    void start();

    enum class PGENET_ServerState {
        Closed,
        Running
    };

    PGENET_ServerState currentState() const;

signals:
    void incomingText(QString text);

private:

    // ///////////// GENERAL STATE ////////////////////////

    PGENET_ServerState m_currentState;


    // ///////////// ASYC IO_SERVICE RUNNER ///////////////

    void _ioService_run();
    QFuture<void> _ioServiceState;

    // ///////////// ASYC TOOLS ///////////////////////////

    std::mutex _bgWorker_mutex;
    std::condition_variable _bgWorker_alert;


    // BACKGROUND WORKER MANAGING:
    // Unused right now
    void _bgWorker_NewMessage(std::string message);

    void _bgWorker_quit();

    // BACKGROUND WORKER:
    void _bgWorker_WaitForIncoming();
    QFuture<void> _bgWorkerState;

    // ///////////// SERVER TOOLS /////////////////////////

    RawPacketDecoder m_pckDecoder;
    std::shared_ptr<ThreadedQueue<std::string> > m_fullPackets;

    // SERVICE / SERVER
    QScopedPointer<asio::io_service> m_service;
    PGENETLL_Server m_llserver;


};

#endif // PGENET_SERVER_H
