#ifndef INTPROC_H
#define INTPROC_H

#include <QObject>
#include "editor_pipe.h"

class IntProc : public QObject
{
    Q_OBJECT
public:
    explicit IntProc(QObject *parent = 0);
    static void init();
    static void quit();
    static bool isWorking();
    static QString state;

    static EditorPipe * editor;
    static bool enabled;
    static bool isEnabled();

signals:

public slots:

};

#endif // INTPROC_H
