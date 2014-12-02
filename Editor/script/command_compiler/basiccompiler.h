#ifndef BASICCOMPILER_H
#define BASICCOMPILER_H

#include <QObject>

class BasicCompiler : public QObject
{
    Q_OBJECT
public:
    explicit BasicCompiler(QObject *parent = 0);
    ~BasicCompiler();

signals:

public slots:

};

#endif // BASICCOMPILER_H
