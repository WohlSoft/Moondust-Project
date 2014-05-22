#ifndef LOGGER_SETS_H
#define LOGGER_SETS_H

#include <QString>
#include <QtMsgHandler>

class LogWriter
{
public:
    static QString DebugLogFile;
    static QtMsgType logLevel;
    static bool   enabled;

    static void WriteToLog(QtMsgType type, QString msg);
    static void LoadLogSettings();

};


#endif // LOGGER_SETS_H
