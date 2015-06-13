#ifndef HISTORYELEMETNREMOVEWARP_H
#define HISTORYELEMETNREMOVEWARP_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementRemoveWarp : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveWarp(const LevelDoor &door, QObject *parent = 0);
    virtual ~HistoryElementRemoveWarp();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    LevelDoor m_removedDoor;
};

#endif // HISTORYELEMETNREMOVEWARP_H
