#ifndef HISTORYELEMENTPLACEDOOR_H
#define HISTORYELEMENTPLACEDOOR_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>


class HistoryElementPlaceDoor : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementPlaceDoor(const LevelDoors &door, bool isEntrance, QObject *parent = 0);
    virtual ~HistoryElementPlaceDoor();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();



public slots:
    void historyRemoveDoors(const LevelDoors &door, QGraphicsItem *item);
private:
    LevelDoors m_door;
};

#endif // HISTORYELEMENTPLACEDOOR_H
