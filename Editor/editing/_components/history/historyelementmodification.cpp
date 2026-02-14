#include "historyelementmodification.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

#include <editing/_scenes/world/wld_scene.h>
#include <editing/_scenes/world/items/item_tile.h>
#include <editing/_scenes/world/items/item_scene.h>
#include <editing/_scenes/world/items/item_path.h>
#include <editing/_scenes/world/items/item_level.h>
#include <editing/_scenes/world/items/item_music.h>

#include <editing/_components/history/itemsearcher.h>

HistoryElementModification::HistoryElementModification(const LevelData &oldData, const LevelData &newData, QObject *parent) :
    QObject(parent), m_oldLvlData(oldData), m_newLvlData(newData)
{}

HistoryElementModification::HistoryElementModification(const WorldData &oldData, const WorldData &newData, QObject *parent) :
    QObject(parent), m_oldWldData(oldData), m_newWldData(newData)
{}

HistoryElementModification::~HistoryElementModification()
{}

QString HistoryElementModification::getHistoryName()
{
    if(m_customHistoryName.isEmpty())
        return tr("Simple Modification History");

    return m_customHistoryName;
}

void HistoryElementModification::undo()
{
    if(!m_scene)
        return;

    if(qobject_cast<LvlScene*>(m_scene))
        processReplacement(m_newLvlData, m_oldLvlData);

    if(qobject_cast<WldScene*>(m_scene))
        processReplacement(m_newWldData, m_oldWldData);
}

void HistoryElementModification::redo()
{
    if(!m_scene)
        return;

    if(qobject_cast<LvlScene*>(m_scene))
        processReplacement(m_oldLvlData, m_newLvlData);

    if(qobject_cast<WldScene*>(m_scene))
        processReplacement(m_oldWldData, m_newWldData);    
}

void HistoryElementModification::processReplacement(const LevelData &toRemoveData, const LevelData &toPlaceData)
{
    LvlScene* lvlScene = 0;
    if(!m_scene)
        return;

    if((lvlScene = qobject_cast<LvlScene*>(m_scene)))
    {
        ItemSearcher lvlSearcher(static_cast<uint32_t>(ItemTypes::LVL_S_Block |
                                                       ItemTypes::LVL_S_BGO |
                                                       ItemTypes::LVL_S_NPC |
                                                       ItemTypes::LVL_S_Door |
                                                       ItemTypes::LVL_S_PhysEnv |
                                                       ItemTypes::LVL_S_Player));

        //connect to our remover functions
        QObject::connect(&lvlSearcher, &ItemSearcher::foundBlock,       this, &HistoryElementModification::processBlock);
        QObject::connect(&lvlSearcher, &ItemSearcher::foundBGO,         this, &HistoryElementModification::processBGO);
        QObject::connect(&lvlSearcher, &ItemSearcher::foundNPC,         this, &HistoryElementModification::processNPC);
        QObject::connect(&lvlSearcher, &ItemSearcher::foundPhysEnv,     this, &HistoryElementModification::processPhysEnv);
        QObject::connect(&lvlSearcher, &ItemSearcher::foundDoor,        this, &HistoryElementModification::processDoor);
        QObject::connect(&lvlSearcher, &ItemSearcher::foundPlayerPoint, this, &HistoryElementModification::processPlayerPoint);

        lvlSearcher.find(toRemoveData, m_scene->items()); //remove the new level Data

        //place the old lvl Data
        lvlScene->placeAll(toPlaceData);
    }
}

void HistoryElementModification::processReplacement(const WorldData &toRemoveData, const WorldData &toPlaceData)
{
    WldScene* wldScene = nullptr;
    if(!m_scene)
        return;

    if((wldScene = qobject_cast<WldScene*>(m_scene)))
    {
        ItemSearcher wldSearcher(static_cast<uint32_t>(ItemTypes::WLD_S_Tile |
                                                       ItemTypes::WLD_S_Scenery |
                                                       ItemTypes::WLD_S_Path |
                                                       ItemTypes::WLD_S_Level |
                                                       ItemTypes::WLD_S_MusicBox));

        //connect to our remover functions
        QObject::connect(&wldSearcher, &ItemSearcher::foundTile,    this, &HistoryElementModification::processTile);
        QObject::connect(&wldSearcher, &ItemSearcher::foundScenery, this, &HistoryElementModification::processScenery);
        QObject::connect(&wldSearcher, &ItemSearcher::foundPath,    this, &HistoryElementModification::processPath);
        QObject::connect(&wldSearcher, &ItemSearcher::foundLevel,   this, &HistoryElementModification::processLevel);
        QObject::connect(&wldSearcher, &ItemSearcher::foundMusicbox,this, &HistoryElementModification::processMusicbox);

        wldSearcher.find(toRemoveData, m_scene->items()); //remove the new level Data

        //place the old lvl Data
        wldScene->placeAll(toPlaceData);
    }
}

QString HistoryElementModification::customHistoryName() const
{
    return m_customHistoryName;
}

void HistoryElementModification::setCustomHistoryName(const QString &customHistoryName)
{
    m_customHistoryName = customHistoryName;
}
LevelData HistoryElementModification::oldLvlData() const
{
    return m_oldLvlData;
}

void HistoryElementModification::setOldLvlData(const LevelData &oldLvlData)
{
    m_oldLvlData = oldLvlData;
}
LevelData HistoryElementModification::newLvlData() const
{
    return m_newLvlData;
}

void HistoryElementModification::setNewLvlData(const LevelData &newLvlData)
{
    m_newLvlData = newLvlData;
}
WorldData HistoryElementModification::oldWldData() const
{
    return m_oldWldData;
}

void HistoryElementModification::setOldWldData(const WorldData &oldWldData)
{
    m_oldWldData = oldWldData;
}
WorldData HistoryElementModification::newWldData() const
{
    return m_newWldData;
}

void HistoryElementModification::setNewWldData(const WorldData &newWldData)
{
    m_newWldData = newWldData;
}

void HistoryElementModification::processBlock(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    Q_UNUSED(sourceBlock)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemBlock*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processBGO(const LevelBGO &sourceBGO, QGraphicsItem *item)
{
    Q_UNUSED(sourceBGO)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemBGO*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    Q_UNUSED(sourceNPC)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemNPC*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processPhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item)
{
    Q_UNUSED(sourcePhysEnv)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemPhysEnv*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processDoor(const LevelDoor &sourceDoor, QGraphicsItem *item)
{
    Q_UNUSED(sourceDoor)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemDoor*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processPlayerPoint(const PlayerPoint &sourcePayerPoint, QGraphicsItem *item)
{
    Q_UNUSED(sourcePayerPoint)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemDoor*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processTile(const WorldTerrainTile &sourceTile, QGraphicsItem *item)
{
    Q_UNUSED(sourceTile)
    WldScene* scene = qobject_cast<WldScene*>(m_scene);
    if(scene)
    {
        ((ItemTile*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processScenery(const WorldScenery &sourceScenery, QGraphicsItem *item)
{
    Q_UNUSED(sourceScenery)
    WldScene* scene = qobject_cast<WldScene*>(m_scene);
    if(scene)
    {
        ((ItemScene*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processPath(const WorldPathTile &sourcePath, QGraphicsItem *item)
{
    Q_UNUSED(sourcePath)
    WldScene* scene = qobject_cast<WldScene*>(m_scene);
    if(scene)
    {
        ((ItemPath*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    Q_UNUSED(sourceLevel)
    WldScene* scene = qobject_cast<WldScene*>(m_scene);
    if(scene)
    {
        ((ItemLevel*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}

void HistoryElementModification::processMusicbox(const WorldMusicBox &sourceMusic, QGraphicsItem *item)
{
    Q_UNUSED(sourceMusic)
    WldScene* scene = qobject_cast<WldScene*>(m_scene);
    if(scene)
    {
        ((ItemMusic*)item)->removeFromArray();
        //scene->removeItem(item);
        delete item;
    }
}
