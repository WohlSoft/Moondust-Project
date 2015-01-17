#ifndef IHISTORYELEMENT_H
#define IHISTORYELEMENT_H

#include <QObject>


class IHistoryElement
{
public:
    virtual ~IHistoryElement(){}
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();
signals:

public slots:

};

Q_DECLARE_INTERFACE(IHistoryElement, "IHistoryElement")

#endif // IHISTORYELEMENT_H
