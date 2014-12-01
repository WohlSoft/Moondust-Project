#ifndef EVENTCOMMAND_H
#define EVENTCOMMAND_H

#include "basiccommand.h"

class EventCommand : public BasicCommand
{
    Q_OBJECT
public:
    explicit EventCommand(QObject *parent = 0);

signals:

public slots:

};

#endif // EVENTCOMMAND_H
