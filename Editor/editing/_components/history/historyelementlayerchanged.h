#ifndef HISTORYELEMENTLAYERCHANGED_H
#define HISTORYELEMENTLAYERCHANGED_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementLayerChanged : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)
public:
    explicit HistoryElementLayerChanged(const LevelData &changedItems, const QString &newLayerName, QObject *parent = 0);
    virtual ~HistoryElementLayerChanged();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

public slots:
    void historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem* item);
    void historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem* item);
    void historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem* item);
    void historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item);
    void historyUndoChangeLayerDoor(const LevelDoors &sourceDoor, QGraphicsItem* item);
    void historyRedoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem* item);
    void historyRedoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem* item);
    void historyRedoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem* item);
    void historyRedoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item);
    void historyRedoChangeLayerDoor(const LevelDoors &sourceDoor, QGraphicsItem* item);

signals:



private:
    LevelData m_levelData;
    QString m_newLayerName;
};

#endif // HISTORYELEMENTLAYERCHANGED_H
