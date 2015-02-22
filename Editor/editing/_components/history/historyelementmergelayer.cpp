#include "historyelementmergelayer.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>


#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_warp_props.h>
#include "itemsearcher.h"

HistoryElementMergeLayer::HistoryElementMergeLayer(const LevelData &mergedData, const QString &newLayerName, QObject *parent) :
    QObject(parent),
    m_mergedData(mergedData),
    m_newLayerName(newLayerName)
{}

HistoryElementMergeLayer::~HistoryElementMergeLayer()
{}

QString HistoryElementMergeLayer::getHistoryName()
{
    return tr("Merge Layer");
}

void HistoryElementMergeLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->layers.push_back(m_mergedData.layers[0]);
    ItemSearcher levelSearcher(static_cast<ItemTypes::itemTypesMultiSelectable>(
                                   ItemTypes::LVL_S_Block |
                                   ItemTypes::LVL_S_BGO |
                                   ItemTypes::LVL_S_NPC |
                                   ItemTypes::LVL_S_Door |
                                   ItemTypes::LVL_S_PhysEnv
                                   ));
    connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBlocks(LevelBlock,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBGO(LevelBGO,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerNPC(LevelNPC,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundDoor(LevelDoors,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerDoor(LevelDoors,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    levelSearcher.find(m_mergedData, m_scene->items());

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->setLayersBox();
    MainWinConnect::pMainWin->setLayerToolsLocked(false);
}

void HistoryElementMergeLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher levelSearcher(static_cast<ItemTypes::itemTypesMultiSelectable>(
                                   ItemTypes::LVL_S_Block |
                                   ItemTypes::LVL_S_BGO |
                                   ItemTypes::LVL_S_NPC |
                                   ItemTypes::LVL_S_Door |
                                   ItemTypes::LVL_S_PhysEnv
                                   ));
    connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerBlocks(LevelBlock,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerBGO(LevelBGO,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerNPC(LevelNPC,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundDoor(LevelDoors,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerDoor(LevelDoors,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    levelSearcher.find(m_mergedData, m_scene->items());


    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == m_mergedData.layers[0].array_id){
            lvlScene->LvlData->layers.removeAt(i);
        }
    }
    MainWinConnect::pMainWin->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->setLayersBox();
    MainWinConnect::pMainWin->setLayerToolsLocked(false);
}


void HistoryElementMergeLayer::historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    QString oldLayer = sourceBlock.layer;
    targetItem->blockData.layer = oldLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    QString oldLayer = sourceBGO.layer;
    targetItem->bgoData.layer = oldLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    QString oldLayer = sourceNPC.layer;
    targetItem->npcData.layer = oldLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemWater* targetItem = (ItemWater*)item;
    QString oldLayer = sourcePhysEnv.layer;
    targetItem->waterData.layer = oldLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyUndoChangeLayerDoor(const LevelDoors &sourceDoor, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString oldLayer = sourceDoor.layer;
    targetItem->doorData.layer = oldLayer;
    foreach (LevelLayers lr, lvlScene->LvlData->layers) {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyRedoChangeLayerBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    targetItem->blockData.layer = m_newLayerName;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyRedoChangeLayerBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    targetItem->bgoData.layer = m_newLayerName;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyRedoChangeLayerNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    targetItem->npcData.layer = m_newLayerName;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyRedoChangeLayerWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemWater* targetItem = (ItemWater*)item;
    targetItem->waterData.layer = m_newLayerName;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementMergeLayer::historyRedoChangeLayerDoor(const LevelDoors &/*sourceDoor*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    targetItem->doorData.layer = m_newLayerName;
    foreach (LevelLayers lr, lvlScene->LvlData->layers) {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}
