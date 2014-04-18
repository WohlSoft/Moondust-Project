#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QtMsgHandler>

void WriteToLog(QtMsgType type, QString msg);

#endif // LOGGER_H
