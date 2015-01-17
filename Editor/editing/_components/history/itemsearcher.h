#ifndef ITEMSEARCHER_H
#define ITEMSEARCHER_H

#include <QObject>
#include <file_formats/file_formats.h>
#include <QGraphicsItem>
#include <defines.h>

class ItemSearcher : public QObject
{
    Q_OBJECT
public:
    explicit ItemSearcher(QObject *parent = 0);
    explicit ItemSearcher(ItemTypes::itemTypesMultiSelectable typesToFind, QObject *parent = 0);

    void find(const LevelData &dataToFind, const QList<QGraphicsItem *> &allItems);
    void find(const WorldData &dataToFind, const QList<QGraphicsItem *> &allItems);

    ItemTypes::itemTypesMultiSelectable findFilter() const;
    void setFindFilter(const ItemTypes::itemTypesMultiSelectable &findFilter);

signals:
    //Level
    void foundBlock(const LevelBlock &sourceBlock, QGraphicsItem* item);
    void foundBGO(const LevelBGO &sourceBGO, QGraphicsItem* item);
    void foundNPC(const LevelNPC &sourceNPC, QGraphicsItem* item);
    void foundPhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item);
    void foundDoor(const LevelDoors &sourceDoor, QGraphicsItem* item);
    void foundPlayerPoint(const PlayerPoint &sourcePayerPoint, QGraphicsItem* item);

    //Overworld
    void foundTile(const WorldTiles &sourceTile, QGraphicsItem* item);
    void foundScenery(const WorldScenery &sourceScenery, QGraphicsItem* item);
    void foundPath(const WorldPaths &sourcePath, QGraphicsItem* item);
    void foundLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void foundMusicbox(const WorldMusic &sourceMusic, QGraphicsItem* item);

public slots:

private:
    ItemTypes::itemTypesMultiSelectable m_findFilter;
};

#endif // ITEMSEARCHER_H
