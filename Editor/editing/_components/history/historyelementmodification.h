#ifndef HISTORYELEMENTMODIFICATION_H
#define HISTORYELEMENTMODIFICATION_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementModification : public QObject, public IHistoryElement
{
    Q_OBJECT

    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementModification(const LevelData &oldData, const LevelData &newData, QObject *parent = 0);
    explicit HistoryElementModification(const WorldData &oldData, const WorldData &newData, QObject *parent = 0);
    virtual ~HistoryElementModification();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();


    void processReplacement(const LevelData &toRemoveData, const LevelData &toPlaceData);
    void processReplacement(const WorldData &toRemoveData, const WorldData &toPlaceData);

    QString customHistoryName() const;
    void setCustomHistoryName(const QString &customHistoryName);

    LevelData oldLvlData() const;
    void setOldLvlData(const LevelData &oldLvlData);

    LevelData newLvlData() const;
    void setNewLvlData(const LevelData &newLvlData);

    WorldData oldWldData() const;
    void setOldWldData(const WorldData &oldWldData);

    WorldData newWldData() const;
    void setNewWldData(const WorldData &newWldData);

signals:

public slots:

private slots:
    void processBlock(const LevelBlock &sourceBlock, QGraphicsItem* item);
    void processBGO(const LevelBGO &sourceBGO, QGraphicsItem* item);
    void processNPC(const LevelNPC &sourceNPC, QGraphicsItem* item);
    void processPhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item);
    void processDoor(const LevelDoors &sourceDoor, QGraphicsItem* item);
    void processPlayerPoint(const PlayerPoint &sourcePayerPoint, QGraphicsItem* item);

    //Overworld
    void processTile(const WorldTiles &sourceTile, QGraphicsItem* item);
    void processScenery(const WorldScenery &sourceScenery, QGraphicsItem* item);
    void processPath(const WorldPaths &sourcePath, QGraphicsItem* item);
    void processLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void processMusicbox(const WorldMusic &sourceMusic, QGraphicsItem* item);


private:
    LevelData m_oldLvlData;
    LevelData m_newLvlData;
    WorldData m_oldWldData;
    WorldData m_newWldData;
    QString m_customHistoryName;
};

#endif // HISTORYELEMENTMODIFICATION_H
