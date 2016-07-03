#include "historyelementlayerchanged.h"
#include <editing/_scenes/level/lvl_scene.h>

#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>
#include <editing/_scenes/level/items/item_playerpoint.h>

#include <editing/_components/history/itemsearcher.h>
#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_layers_box.h>

HistoryElementLayerChanged::HistoryElementLayerChanged(const LevelData &changedItems, const QString &newLayerName, QObject *parent) :
    QObject(parent),
    m_levelData(changedItems),
    m_newLayerName(newLayerName)
{}

HistoryElementLayerChanged::~HistoryElementLayerChanged()
{}

QString HistoryElementLayerChanged::getHistoryName()
{
    return tr("Change Layer");
}

void HistoryElementLayerChanged::undo()
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
    connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBlocks(LevelBlock,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerBGO(LevelBGO,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerNPC(LevelNPC,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerDoor(LevelDoor,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    levelSearcher.find(m_levelData, m_scene->items());

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);

}

void HistoryElementLayerChanged::redo()
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
    connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerDoor(LevelDoor,QGraphicsItem*)));
    connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerWater(LevelPhysEnv,QGraphicsItem*)));
    levelSearcher.find(m_levelData, m_scene->items());

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);

}

void HistoryElementLayerChanged::historyUndoChangeLayerBlocks(const LevelBlock &sourceBlock, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    QString oldLayer = sourceBlock.layer;
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyUndoChangeLayerBGO(const LevelBGO &sourceBGO, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    QString oldLayer = sourceBGO.layer;
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyUndoChangeLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    QString oldLayer = sourceNPC.layer;
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyUndoChangeLayerWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemPhysEnv* targetItem = (ItemPhysEnv*)item;
    QString oldLayer = sourcePhysEnv.layer;
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyUndoChangeLayerDoor(const LevelDoor &sourceDoor, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString oldLayer = sourceDoor.layer;
    targetItem->m_data.layer = oldLayer;
    foreach (LevelLayer lr, lvlScene->LvlData->layers) {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyRedoChangeLayerBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBlock* targetItem = (ItemBlock*)item;
    targetItem->m_data.layer = m_newLayerName;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyRedoChangeLayerBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemBGO* targetItem = (ItemBGO*)item;
    targetItem->m_data.layer = m_newLayerName;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyRedoChangeLayerNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemNPC* targetItem = (ItemNPC*)item;
    targetItem->m_data.layer = m_newLayerName;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyRedoChangeLayerWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemPhysEnv* targetItem = (ItemPhysEnv*)item;
    targetItem->m_data.layer = m_newLayerName;
    foreach(LevelLayer lr, lvlScene->LvlData->layers)
    {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementLayerChanged::historyRedoChangeLayerDoor(const LevelDoor &/*sourceDoor*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    targetItem->m_data.layer = m_newLayerName;
    foreach (LevelLayer lr, lvlScene->LvlData->layers) {
        if(lr.name == m_newLayerName)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}
