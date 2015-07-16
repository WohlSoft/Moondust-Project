#include "threadedlogger.h"

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
    queuedMsgWorkerState = QtConcurrent::run([this](){queuedMsgWorker();});
}

ThreadedLogger::~ThreadedLogger()
{
    quitQueuedMsgWorker();
    queuedMsgWorkerState.waitForFinished();
}

