#include "historyelementchangednewlayer.h"

#include "itemsearcher.h"
#include "common_features/main_window_ptr.h"


#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

#include <main_window/dock/lvl_layers_box.h>

HistoryElementChangedNewLayer::HistoryElementChangedNewLayer(const LevelData &changedItems, LevelLayer &newLayer, QObject *parent) :
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
    connect(searcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoChangeLayerDoor(LevelDoor,QGraphicsItem*)));
    searcher->find(modifiedSourceData, m_scene->items());
    delete searcher;

    for(int i = 0; i < lvlScene->m_data->layers.size(); i++){
        if(lvlScene->m_data->layers[i].meta.array_id == m_newLayer.meta.array_id){
            lvlScene->m_data->layers.removeAt(i);
        }
    }
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

void HistoryElementChangedNewLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelData modifiedSourceData = m_changedItems;

    lvlScene->m_data->layers.push_back(m_newLayer);

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
    connect(searcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoChangeLayerDoor(LevelDoor,QGraphicsItem*)));
    searcher->find(modifiedSourceData, m_scene->items());
    delete searcher;

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
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
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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

    ItemPhysEnv* targetItem = (ItemPhysEnv*)item;
    QString oldLayer = physEnv.layer;
    targetItem->m_data.layer = oldLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyUndoChangeLayerDoor(const LevelDoor &door, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString oldLayer = door.layer;
    targetItem->m_data.layer = oldLayer;
    foreach (LevelLayer lr, lvlScene->m_data->layers) {
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
    targetItem->m_data.layer = newLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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
    targetItem->m_data.layer = newLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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
    targetItem->m_data.layer = newLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
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

    ItemPhysEnv* targetItem = (ItemPhysEnv*)item;
    QString newLayer = m_newLayer.name;
    targetItem->m_data.layer = newLayer;
    foreach(LevelLayer lr, lvlScene->m_data->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void HistoryElementChangedNewLayer::historyRedoChangeLayerDoor(const LevelDoor &/*door*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemDoor* targetItem = (ItemDoor*)item;
    QString newLayer = m_newLayer.name;
    targetItem->m_data.layer = newLayer;
    foreach (LevelLayer lr, lvlScene->m_data->layers) {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}











