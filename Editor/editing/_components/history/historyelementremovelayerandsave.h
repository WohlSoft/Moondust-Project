#ifndef HISTORYELEMENTREMOVELAYERANDSAVE_H
#define HISTORYELEMENTREMOVELAYERANDSAVE_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementRemoveLayerAndSave : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveLayerAndSave(LevelData modData, QObject *parent = 0);
    virtual ~HistoryElementRemoveLayerAndSave();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

public slots:
    void historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem *item);
    void historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item);
    void historyUndoChangeLayerDoor(const LevelDoors &sourceDoor, QGraphicsItem *item);
    void historyRedoChangeLayerBlocks(const LevelBlock &, QGraphicsItem *item);
    void historyRedoChangeLayerBGO(const LevelBGO &, QGraphicsItem *item);
    void historyRedoChangeLayerNPC(const LevelNPC &, QGraphicsItem *item);
    void historyRedoChangeLayerWater(const LevelPhysEnv &, QGraphicsItem *item);
    void historyRedoChangeLayerDoor(const LevelDoors &, QGraphicsItem *item);
private:
    LevelData m_modData;
};

#endif // HISTORYELEMENTREMOVELAYERANDSAVE_H
