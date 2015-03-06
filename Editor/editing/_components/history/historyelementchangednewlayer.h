#ifndef HISTORYELEMENTCHANGEDNEWLAYER_H
#define HISTORYELEMENTCHANGEDNEWLAYER_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementChangedNewLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementChangedNewLayer(const LevelData &changedItems, LevelLayers &newLayer, QObject *parent = 0);
    virtual ~HistoryElementChangedNewLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

public slots:
    void historyUndoChangeLayerBlocks(const LevelBlock &block, QGraphicsItem* item);
    void historyUndoChangeLayerBGO(const LevelBGO &bgo, QGraphicsItem* item);
    void historyUndoChangeLayerNPC(const LevelNPC &npc, QGraphicsItem* item);
    void historyUndoChangeLayerWater(const LevelPhysEnv &physEnv, QGraphicsItem* item);
    void historyUndoChangeLayerDoor(const LevelDoors &door, QGraphicsItem* item);
    void historyRedoChangeLayerBlocks(const LevelBlock &block, QGraphicsItem* item);
    void historyRedoChangeLayerBGO(const LevelBGO &bgo, QGraphicsItem* item);
    void historyRedoChangeLayerNPC(const LevelNPC &npc, QGraphicsItem* item);
    void historyRedoChangeLayerWater(const LevelPhysEnv &physEnv, QGraphicsItem* item);
    void historyRedoChangeLayerDoor(const LevelDoors &door, QGraphicsItem* item);

private:
    LevelData m_changedItems;
    LevelLayers m_newLayer;
};

#endif // HISTORYELEMENTCHANGEDNEWLAYER_H
