#ifndef MEMORYCOMMAND_H
#define MEMORYCOMMAND_H

#include "basiccommand.h"

class MemoryCommand : public BasicCommand
{
    Q_OBJECT
public:
    explicit MemoryCommand(QObject *parent = 0);

signals:

public slots:

};

#endif // MEMORYCOMMAND_H
