#ifndef LOGGER_SETS_H
#define LOGGER_SETS_H

#include <QString>
#include <QtMsgHandler>

namespace LogWriter
{
    QString DebugLogFile="PGE_debug_log.txt";
    QtMsgType logLevel = QtDebugMsg;
    void WriteToLog(QtMsgType type, QString msg);
}


#endif // LOGGER_SETS_H
