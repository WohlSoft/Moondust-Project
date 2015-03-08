#ifndef HISTORYELEMENTMERGELAYER_H
#define HISTORYELEMENTMERGELAYER_H

#include <QObject>

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementMergeLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementMergeLayer(const LevelData &mergedData, const QString &newLayerName, QObject *parent = 0);
    virtual ~HistoryElementMergeLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

public slots:
    void historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem *item);
    void historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item);
    void historyUndoChangeLayerDoor(const LevelDoor &sourceDoor, QGraphicsItem *item);
    void historyRedoChangeLayerBlocks(const LevelBlock &, QGraphicsItem *item);
    void historyRedoChangeLayerBGO(const LevelBGO &, QGraphicsItem *item);
    void historyRedoChangeLayerNPC(const LevelNPC &, QGraphicsItem *item);
    void historyRedoChangeLayerWater(const LevelPhysEnv &, QGraphicsItem *item);
    void historyRedoChangeLayerDoor(const LevelDoor &, QGraphicsItem *item);
private:
    LevelData m_mergedData;
    QString m_newLayerName;
};


#endif // HISTORYELEMENTMERGELAYER_H
