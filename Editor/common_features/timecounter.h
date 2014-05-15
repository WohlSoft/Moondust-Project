#ifndef TIMECOUNTER_H
#define TIMECOUNTER_H
#include <QTime>
#include <QString>
#include "logger.h"

class TimeCounter
{
public:
    TimeCounter();
    void start();
    void stop(QString msg="",int items=0);

private:
    QTime TickTack;
    QString msg;

};

#endif // TIMECOUNTER_H
