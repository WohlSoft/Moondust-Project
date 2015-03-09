#ifndef HISTORYELEMENTPLACEDOOR_H
#define HISTORYELEMENTPLACEDOOR_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>


class HistoryElementPlaceDoor : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementPlaceDoor(const LevelDoor &door, bool isEntrance, QObject *parent = 0);
    virtual ~HistoryElementPlaceDoor();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();



public slots:
    void historyRemoveDoors(const LevelDoor &door, QGraphicsItem *item);
private:
    LevelDoor m_door;
};

#endif // HISTORYELEMENTPLACEDOOR_H
