#ifndef HISTORYELEMENTREMOVELAYER_H
#define HISTORYELEMENTREMOVELAYER_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementRemoveLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveLayer(LevelData modData, QObject *parent = 0);
    virtual ~HistoryElementRemoveLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();



public slots:
    void removeBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void removeBGO(const LevelBGO &sourceBGO, QGraphicsItem *item);
    void removeNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void removePhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item);
    void removeDoor(const LevelDoor &sourceDoor, QGraphicsItem *item);
    void historyUpdateVisibleBlocks(const LevelBlock &, QGraphicsItem *item);
    void historyUpdateVisibleBGO(const LevelBGO &, QGraphicsItem *item);
    void historyUpdateVisibleNPC(const LevelNPC &, QGraphicsItem *item);
    void historyUpdateVisibleWater(const LevelPhysEnv &, QGraphicsItem *item);
    void historyUpdateVisibleDoor(const LevelDoor &, QGraphicsItem *item);


private:
    LevelData m_modData;
};

#endif // HISTORYELEMENTREMOVELAYER_H
