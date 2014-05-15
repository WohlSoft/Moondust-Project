#include "timecounter.h"

TimeCounter::TimeCounter()
{
}

void TimeCounter::start()
{
    TickTack.start();
    TickTack.restart();
}

void TimeCounter::stop(QString msg, int items)
{
    int counter = TickTack.elapsed();
    WriteToLog(QtDebugMsg, QString("Performance test-> %1 %2 in %3 ms").arg(items).arg(msg).arg(counter));
}
