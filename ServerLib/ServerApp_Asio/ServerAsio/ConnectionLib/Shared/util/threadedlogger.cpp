#include "threadedlogger.h"

QScopedPointer<ThreadedLogger> gThreadedLogger;

void ThreadedLogger::queuedMsgWorker()
{
    for(;;){
        QPair<LoggerLevel, QString> nextMsg = queuedMsg.pop();
        if(queuedMsg.shouldExit())
            return;
        emit newIncomingMsg(nextMsg.first, nextMsg.second);
    }
}

void ThreadedLogger::quitQueuedMsgWorker()
{
    queuedMsg.doExit();
}

ThreadedLogger::ThreadedLogger(QObject *parent) : QObject(parent)
{
    std::cout << "Threaded logger constructed!" << std::endl;
    queuedMsgWorkerState = QtConcurrent::run([this](){queuedMsgWorker();});
}

ThreadedLogger::~ThreadedLogger()
{
    std::cout << "Threaded logger destructed!" << std::endl;
    quitQueuedMsgWorker();
    queuedMsgWorkerState.waitForFinished();
}

