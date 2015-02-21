#ifndef HISTORYELEMENTRENAMEEVENT_H
#define HISTORYELEMENTRENAMEEVENT_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementRenameEvent : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRenameEvent(const int array_id, const QString &oldName, const QString &newName, QObject *parent = 0);
    virtual ~HistoryElementRenameEvent();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    QString m_oldName;
    QString m_newName;
};

#endif // HISTORYELEMENTRENAMEEVENT_H
