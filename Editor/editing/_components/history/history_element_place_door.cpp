#include "history_element_place_door.h"

#include <editing/_components/history/item_searcher.h>
#include <editing/_scenes/level/items/item_door.h>

#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_warp_props.h>

HistoryElementPlaceDoor::HistoryElementPlaceDoor(const LevelDoor &door, bool isEntrance, QObject *parent) :
    QObject(parent),
    m_door(door),
    m_isEntrance(isEntrance)
{}

HistoryElementPlaceDoor::~HistoryElementPlaceDoor()
{}

QString HistoryElementPlaceDoor::getHistoryName()
{
    return tr("Place Door");
}

void HistoryElementPlaceDoor::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher* searcher = new ItemSearcher(m_isEntrance ? ItemTypes::LVL_S_DoorEnter : ItemTypes::LVL_S_DoorExit);

    LevelData data;
    data.doors << m_door;

    QObject::connect(searcher, &ItemSearcher::foundDoor, this, &HistoryElementPlaceDoor::historyRemoveDoors);
    searcher->find(data, lvlScene);
    delete searcher;
}

void HistoryElementPlaceDoor::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene = qobject_cast<LvlScene*>(m_scene);

    if(!lvlScene)
        return;

    bool found = false;
    LevelDoor door;

    foreach(const LevelDoor &findDoor, lvlScene->m_data->doors)
    {
        if(m_door.meta.array_id == findDoor.meta.array_id)
        {
            door = findDoor;
            found = true;
            break;
        }
    }

    if(!found)
        return;

    if(m_isEntrance)
    {
        door.ix = m_door.ix;
        door.iy = m_door.iy;
        door.isSetIn = true;
        lvlScene->placeDoorEnter(door, false, false);
    }
    else
    {
        door.ox = m_door.ox;
        door.oy = m_door.oy;
        door.isSetOut = true;
        lvlScene->placeDoorExit(door, false, false);
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}

void HistoryElementPlaceDoor::historyRemoveDoors(const LevelDoor &/*door*/, QGraphicsItem* item)
{
    ItemDoor *sceneItem = qgraphicsitem_cast<ItemDoor*>(item);

    if(sceneItem)
        sceneItem->removeFromArray();

    if(item)
        delete (item);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}
