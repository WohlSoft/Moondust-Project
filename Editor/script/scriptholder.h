#ifndef SCRIPTHOLDER_H
#define SCRIPTHOLDER_H

#include <QObject>
#include "command_compiler/basiccompiler.h"
#include "commands/basiccommand.h"
#include "commands/eventcommand.h"

class ScriptHolder : public QObject
{
    Q_OBJECT
public:
    explicit ScriptHolder(QObject *parent = 0);
    ~ScriptHolder();

signals:

public slots:

private:
    BasicCompiler* m_usingCompiler;
    QList<EventCommand> m_events;
};

#endif // SCRIPTHOLDER_H
