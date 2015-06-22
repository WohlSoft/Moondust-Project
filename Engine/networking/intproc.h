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
    enum ExternalCommands
    {
        MessageBox=0,
        Cheat=1,
        PlaceItem=2
    };
    static ExternalCommands command;

    static    bool cmd_accepted;
    static bool hasCommand();
    static ExternalCommands  commandType();
    static QString getCMD();
    static QString cmd;

    static EditorPipe * editor;
    static bool enabled;
    static bool isEnabled();

signals:

public slots:

};

#endif // INTPROC_H
