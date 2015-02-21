#include "historyelementchangednewlayer.h"

#include "itemsearcher.h"
#include "common_features/mainwinconnect.h"


#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

HistoryElementChangedNewLayer::HistoryElementChangedNewLayer(const LevelData &changedItems, LevelLayers &newLayer, QObject *parent) :
    QObject(parent),
    m_changedItems(changedItems),
    m_newLayer(newLayer)
{}

HistoryElementChangedNewLayer::~HistoryElementChangedNewLayer()
{}

QString HistoryElementChangedNewLayer::getHistoryName()
{
    return tr("New Layer");
}

void HistoryElementChangedNewLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelData modifiedSourceData = m_changedItems;

    ItemSearcher* searcher = new ItemSearcher(static_cast<ItemTypes::itemTypesMultiSelectable>(
                                                  ItemTypes::LVL_S_Block |
                                                  ItemTypes::LVL_S_BGO |
                                                  ItemTypes::LVL_S_NPC |
                                                  ItemTypes::LVL_S_PhysEnv |
                                                  ItemTypes::LVL_S_Door
                                                  ));

    connect(searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBlocks(LevelBlock,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBGO(LevelBGO,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerNPC(LevelNPC,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundDoor(LevelDoors,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerDoor(LevelDoors,QGraphicsItem*)));
    searcher->find(modifiedSourceData, m_scene->items());
    delete searcher;

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == m_newLayer.array_id){
            lvlScene->LvlData->layers.removeAt(i);
        }
    }
    MainWinConnect::pMainWin->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->setLayersBox();
    MainWinConnect::pMainWin->setLayerToolsLocked(false);
}

void HistoryElementChangedNewLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelData modifiedSourceData = m_changedItems;

    lvlScene->LvlData->layers.push_back(m_newLayer);

    ItemSearcher* searcher = new ItemSearcher(static_cast<ItemTypes::itemTypesMultiSelectable>(
                                                  ItemTypes::LVL_S_Block |
                                                  ItemTypes::LVL_S_BGO |
                                                  ItemTypes::LVL_S_NPC |
                                                  ItemTypes::LVL_S_PhysEnv |
                                                  ItemTypes::LVL_S_Door
                                                  ));

    connect(searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerBlocks(LevelBlock,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerBGO(LevelBGO,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerNPC(LevelNPC,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundDoor(LevelDoors,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerDoor(LevelDoors,QGraphicsItem*)));
    searcher->find(modifiedSourceData, m_scene->items());
    delete searcher;

    MainWinConnect::pMainWin->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->setLayersBox();
    MainWinConnect::pMainWin->setLayerToolsLocked(false);
}



void HistoryElementChangedNewLayer::historyUndoChangeLayerBlocks(const LevelBlock &block, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    QString oldLayer = block.layer;
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

void HistoryElementChangedNewLayer::historyUndoChangeLayerBGO(const LevelBGO &bgo, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    QString oldLayer = bgo.layer;
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

void HistoryElementChangedNewLayer::historyUndoChangeLayerNPC(const LevelNPC &npc, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    QString oldLayer = npc.layer;
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

void HistoryElementChangedNewLayer::historyUndoChangeLayerWater(const LevelPhysEnv &physEnv, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemWater* targetItem = (ItemWater*)item;
    QString oldLayer = physEnv.layer;
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

void HistoryElementChangedNewLayer::historyUndoChangeLayerDoor(const LevelDoors &door, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString oldLayer = door.layer;
    targetItem->doorData.layer = oldLayer;
    foreach (LevelLayers lr, lvlScene->LvlData->layers) {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerBlocks(const LevelBlock &/*block*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    QString newLayer = m_newLayer.name;
    targetItem->blockData.layer = newLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerBGO(const LevelBGO &/*bgo*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    QString newLayer = m_newLayer.name;
    targetItem->bgoData.layer = newLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerNPC(const LevelNPC &/*npc*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    QString newLayer = m_newLayer.name;
    targetItem->npcData.layer = newLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerWater(const LevelPhysEnv &/*physEnv*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemWater* targetItem = (ItemWater*)item;
    QString newLayer = m_newLayer.name;
    targetItem->waterData.layer = newLayer;
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerDoor(const LevelDoors &/*door*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString newLayer = m_newLayer.name;
    targetItem->doorData.layer = newLayer;
    foreach (LevelLayers lr, lvlScene->LvlData->layers) {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}











