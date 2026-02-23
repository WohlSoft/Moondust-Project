#include "history_element_remove_layer.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>

#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_layers_box.h>
#include "item_searcher.h"

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

    lvlScene->curViewPort()->setUpdatesEnabled(false);
    lvlScene->m_data->layers.push_back(m_modData.layers[0]);
    lvlScene->placeAll(m_modData);

    ItemSearcher* searcher = new ItemSearcher(ItemTypes::LVL_S_ALL_BASE);
    QObject::connect(searcher, &ItemSearcher::foundBlock,    this, &HistoryElementRemoveLayer::historyUpdateVisibleBlocks);
    QObject::connect(searcher, &ItemSearcher::foundBGO,      this, &HistoryElementRemoveLayer::historyUpdateVisibleBGO);
    QObject::connect(searcher, &ItemSearcher::foundNPC,      this, &HistoryElementRemoveLayer::historyUpdateVisibleNPC);
    QObject::connect(searcher, &ItemSearcher::foundPhysEnv,  this, &HistoryElementRemoveLayer::historyUpdateVisibleWater);
    QObject::connect(searcher, &ItemSearcher::foundDoor,     this, &HistoryElementRemoveLayer::historyUpdateVisibleDoor);
    searcher->find(m_modData, lvlScene);
    delete searcher;

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);

    lvlScene->curViewPort()->setUpdatesEnabled(true);
}

void HistoryElementRemoveLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->curViewPort()->setUpdatesEnabled(false);
    ItemSearcher* searcher = new ItemSearcher(ItemTypes::LVL_S_ALL_BASE);
    QObject::connect(searcher, &ItemSearcher::foundBlock,    this, &HistoryElementRemoveLayer::removeBlock);
    QObject::connect(searcher, &ItemSearcher::foundBGO,      this, &HistoryElementRemoveLayer::removeBGO);
    QObject::connect(searcher, &ItemSearcher::foundNPC,      this, &HistoryElementRemoveLayer::removeNPC);
    QObject::connect(searcher, &ItemSearcher::foundPhysEnv,  this, &HistoryElementRemoveLayer::removePhysEnv);
    QObject::connect(searcher, &ItemSearcher::foundDoor,     this, &HistoryElementRemoveLayer::removeDoor);
    searcher->find(m_modData, lvlScene);
    delete searcher;

    for(int i = 0; i < lvlScene->m_data->layers.size(); i++)
    {
        if(lvlScene->m_data->layers[i].meta.array_id == m_modData.layers[0].meta.array_id)
            lvlScene->m_data->layers.removeAt(i);
    }

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
    lvlScene->curViewPort()->setUpdatesEnabled(true);
}


void HistoryElementRemoveLayer::removeBlock(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    Q_UNUSED(sourceBlock)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemBlock*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removeBGO(const LevelBGO &sourceBGO, QGraphicsItem *item)
{
    Q_UNUSED(sourceBGO)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemBGO*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removeNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    Q_UNUSED(sourceNPC)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ((ItemNPC*)item)->removeFromArray();
        scene->removeItem(item);
        delete item;
    }
}

void HistoryElementRemoveLayer::removePhysEnv(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item)
{
    Q_UNUSED(sourcePhysEnv)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ItemPhysEnv *it = dynamic_cast<ItemPhysEnv*>(item);
        it->removeFromArray();
        scene->removeItem(it);
        delete it;
    }
}

void HistoryElementRemoveLayer::removeDoor(const LevelDoor &sourceDoor, QGraphicsItem *item)
{
    Q_UNUSED(sourceDoor)
    LvlScene* scene = qobject_cast<LvlScene*>(m_scene);
    if(scene)
    {
        ItemDoor *it = dynamic_cast<ItemDoor*>(item);
        it->removeFromArray();
        scene->removeItem(it);
        delete it;
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
