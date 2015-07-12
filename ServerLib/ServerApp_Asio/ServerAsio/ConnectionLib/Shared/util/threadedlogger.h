#ifndef THREADEDLOGGER_H
#define THREADEDLOGGER_H

#include <QObject>
#include "ThreadedQueue.h"

#include <QtConcurrent>

class ThreadedLogger;
static QScopedPointer<ThreadedLogger> gThreadedLogger;

class ThreadedLogger : public QObject
{
    Q_OBJECT
public:
    enum LoggerLevel {
        LOGGER_DEBUG,
        LOGGER_INFO,
        LOGGER_WARNING,
        LOGGER_ERROR
    };
    static inline QString LoggerLevelToString(LoggerLevel level){
        switch (level) {
        case LOGGER_DEBUG: return "Debug";
        case LOGGER_INFO: return "Info";
        case LOGGER_WARNING: return "Warning";
        case LOGGER_ERROR: return "Error";
        default:
            return "?";
        }
    }
    static inline void initStatic(){
        gThreadedLogger.reset(new ThreadedLogger());
        qRegisterMetaType<ThreadedLogger::LoggerLevel>("ThreadedLogger::LoggerLevel");
    }
    static inline void destoryStatic(){
        gThreadedLogger.reset();
    }

private:
    ThreadedQueue<QPair<LoggerLevel, QString>> queuedMsg;
    QFuture<void> queuedMsgWorkerState;
    void queuedMsgWorker();
    void quitQueuedMsgWorker();
public:
    explicit ThreadedLogger(QObject *parent = 0);
    ~ThreadedLogger();

    void logDebug(const QString& msg){queuedMsg.push(qMakePair(LOGGER_DEBUG, msg));}
    void logInfo(const QString& msg){queuedMsg.push(qMakePair(LOGGER_INFO, msg));}
    void logWarning(const QString& msg){queuedMsg.push(qMakePair(LOGGER_WARNING, msg));}
    void logError(const QString& msg){queuedMsg.push(qMakePair(LOGGER_ERROR, msg));}



signals:
    void newIncomingMsg(ThreadedLogger::LoggerLevel, QString);
public slots:
};


#endif // THREADEDLOGGER_H
