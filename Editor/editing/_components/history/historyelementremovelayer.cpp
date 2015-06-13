#include "historyelementremovelayer.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>

#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_layers_box.h>
#include "itemsearcher.h"

HistoryElementRemoveLayer::HistoryElementRemoveLayer(LevelData modData, QObject *parent) :
    QObject(parent),
    m_modData(modData)
{}

HistoryElementRemoveLayer::~HistoryElementRemoveLayer()
{}

QString HistoryElementRemoveLayer::getHistoryName()
{
    return tr("Remove Layer");
}

void HistoryElementRemoveLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->layers.push_back(m_modData.layers[0]);
    lvlScene->placeAll(m_modData);


    ItemSearcher* searcher = new ItemSearcher(ItemTypes::itemTypesMultiSelectable(
                                                  ItemTypes::LVL_S_Block |
                                                  ItemTypes::LVL_S_BGO |
                                                  ItemTypes::LVL_S_NPC |
                                                  ItemTypes::LVL_S_Door |
                                                  ItemTypes::LVL_S_PhysEnv
                                                  ));
    connect(searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUpdateVisibleBlocks(LevelBlock,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUpdateVisibleBGO(LevelBGO,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUpdateVisibleNPC(LevelNPC,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyUpdateVisibleWater(LevelPhysEnv,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUpdateVisibleDoor(LevelDoor,QGraphicsItem*)));
    searcher->find(m_modData, m_scene->items());
    delete searcher;

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

void HistoryElementRemoveLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher* searcher = new ItemSearcher(ItemTypes::itemTypesMultiSelectable(
                                                  ItemTypes::LVL_S_Block |
                                                  ItemTypes::LVL_S_BGO |
                                                  ItemTypes::LVL_S_NPC |
                                                  ItemTypes::LVL_S_Door |
                                                  ItemTypes::LVL_S_PhysEnv
                                                  ));
    connect(searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(removeBlock(LevelBlock,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(removeBGO(LevelBGO,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(removeNPC(LevelNPC,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(removePhysEnv(LevelPhysEnv,QGraphicsItem*)));
    connect(searcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(removeDoor(LevelDoor,QGraphicsItem*)));
    searcher->find(m_modData, m_scene->items());
    delete searcher;

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == m_modData.layers[0].array_id){
            lvlScene->LvlData->layers.removeAt(i);
        }
    }

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}


void HistoryElementRemoveLayer::removeBlock(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    Q_UNUSED(sourceBlock)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene){
        ((ItemBlock*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removeBGO(const LevelBGO &sourceBGO, QGraphicsItem *item)
{
    Q_UNUSED(sourceBGO)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene){
        ((ItemBGO*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removeNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    Q_UNUSED(sourceNPC)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene){
        ((ItemNPC*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removePhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item)
{
    Q_UNUSED(sourcePhysEnv)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene){
        ((ItemWater*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removeDoor(const LevelDoor &sourceDoor, QGraphicsItem *item)
{
    Q_UNUSED(sourceDoor)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene){
        ((ItemDoor*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}


void HistoryElementRemoveLayer::historyUpdateVisibleBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    item->setVisible(!m_modData.layers[0].hidden);
}

void HistoryElementRemoveLayer::historyUpdateVisibleBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem *item)
{
    item->setVisible(!m_modData.layers[0].hidden);
}

void HistoryElementRemoveLayer::historyUpdateVisibleNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    item->setVisible(!m_modData.layers[0].hidden);
}

void HistoryElementRemoveLayer::historyUpdateVisibleWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem *item)
{
    item->setVisible(!m_modData.layers[0].hidden);
}

void HistoryElementRemoveLayer::historyUpdateVisibleDoor(const LevelDoor &/*sourceDoor*/, QGraphicsItem *item)
{
    item->setVisible(!m_modData.layers[0].hidden);
}
