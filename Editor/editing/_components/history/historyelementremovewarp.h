#ifndef HISTORYELEMETNREMOVEWARP_H
#define HISTORYELEMETNREMOVEWARP_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementRemoveWarp : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveWarp(const LevelDoors &door, QObject *parent = 0);
    virtual ~HistoryElementRemoveWarp();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    LevelDoors m_removedDoor;
};

#endif // HISTORYELEMETNREMOVEWARP_H
