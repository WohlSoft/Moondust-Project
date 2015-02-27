#include "historyelementremovelayerandsave.h"

#include <common_features/mainwinconnect.h>

#include "itemsearcher.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>

#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_layers_box.h>

HistoryElementRemoveLayerAndSave::HistoryElementRemoveLayerAndSave(LevelData modData, QObject *parent) :
    QObject(parent),
    m_modData(modData)
{}

HistoryElementRemoveLayerAndSave::~HistoryElementRemoveLayerAndSave()
{}

QString HistoryElementRemoveLayerAndSave::getHistoryName()
{
    return tr("Remove layer and save items");
}

void HistoryElementRemoveLayerAndSave::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->layers.push_back(m_modData.layers[0]);
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
    levelSearcher.find(m_modData, m_scene->items());

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

void HistoryElementRemoveLayerAndSave::redo()
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
    levelSearcher.find(m_modData, m_scene->items());

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == m_modData.layers[0].array_id){
            lvlScene->LvlData->layers.removeAt(i);
        }
    }
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}


void HistoryElementRemoveLayerAndSave::historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem* item)
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

void HistoryElementRemoveLayerAndSave::historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem* item)
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

void HistoryElementRemoveLayerAndSave::historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem* item)
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

void HistoryElementRemoveLayerAndSave::historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item)
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

void HistoryElementRemoveLayerAndSave::historyUndoChangeLayerDoor(const LevelDoors &sourceDoor, QGraphicsItem* item)
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

void HistoryElementRemoveLayerAndSave::historyRedoChangeLayerBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    targetItem->blockData.layer = QString("Default");
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == QString("Default"))
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementRemoveLayerAndSave::historyRedoChangeLayerBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    targetItem->bgoData.layer = QString("Default");
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == QString("Default"))
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementRemoveLayerAndSave::historyRedoChangeLayerNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    targetItem->npcData.layer = QString("Default");
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == QString("Default"))
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementRemoveLayerAndSave::historyRedoChangeLayerWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemWater* targetItem = (ItemWater*)item;
    targetItem->waterData.layer = QString("Default");
    foreach(LevelLayers lr, lvlScene->LvlData->layers)
    {
        if(lr.name == QString("Default"))
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementRemoveLayerAndSave::historyRedoChangeLayerDoor(const LevelDoors &/*sourceDoor*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    targetItem->doorData.layer = QString("Default");
    foreach (LevelLayers lr, lvlScene->LvlData->layers) {
        if(lr.name == QString("Default"))
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}
