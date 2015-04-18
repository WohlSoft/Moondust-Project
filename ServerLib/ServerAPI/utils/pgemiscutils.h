#ifndef PGEMISCUTILS_H
#define PGEMISCUTILS_H

#include <qlogging.h>
#include <QApplication>

static inline bool isQAppRunning(){return (qobject_cast<QApplication*>(QCoreApplication::instance())!=0);}

static inline void warnIfQAppNotRunning(){
    if(!isQAppRunning())
        qWarning() << "QApplication is not running!";
}

#endif // PGEMISCUTILS_H
