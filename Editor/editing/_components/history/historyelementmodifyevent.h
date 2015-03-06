#ifndef HISTORYELEMENTMODIFYEVENT_H
#define HISTORYELEMENTMODIFYEVENT_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementModifyEvent : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementModifyEvent(const LevelEvents &event, bool didRemove, QObject *parent = 0);
    virtual ~HistoryElementModifyEvent();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

    void doEventRemove();
    void doEventPlace();

private:
    LevelEvents m_event;
    bool m_didRemove;
};

#endif // HISTORYELEMENTMODIFYEVENT_H
