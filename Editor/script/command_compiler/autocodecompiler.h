#ifndef AUTOCODECOMPILER_H
#define AUTOCODECOMPILER_H

#include "basiccompiler.h"

class AutocodeCompiler : public BasicCompiler
{
    Q_OBJECT
public:
    explicit AutocodeCompiler(QList<EventCommand *> commands, QObject *parent = 0);
    ~AutocodeCompiler();

    virtual QString compileCode();

signals:

public slots:

};

#endif // AUTOCODECOMPILER_H
