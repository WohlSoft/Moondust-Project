#ifndef BASICCOMMAND_H
#define BASICCOMMAND_H

#include <QObject>

class BasicCommand : public QObject
{
    Q_OBJECT
public:
    explicit BasicCommand(QObject *parent = 0);

signals:

public slots:

};

#endif // BASICCOMMAND_H
