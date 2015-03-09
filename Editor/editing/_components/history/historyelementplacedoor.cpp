#include "historyelementplacedoor.h"

#include <editing/_components/history/itemsearcher.h>
#include <editing/_scenes/level/items/item_door.h>

#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_warp_props.h>

HistoryElementPlaceDoor::HistoryElementPlaceDoor(const LevelDoor &door, bool isEntrance, QObject *parent) :
    QObject(parent),
    m_door(door)
{
    m_door.isSetIn = isEntrance;
    m_door.isSetOut = !isEntrance;
}

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

    ItemSearcher* searcher = new ItemSearcher(ItemTypes::LVL_S_Door);

    LevelData data;
    data.doors << m_door;

    connect(searcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRemoveDoors(LevelDoor,QGraphicsItem*)));
    searcher->find(data, m_scene->items());
    delete searcher;
}

void HistoryElementPlaceDoor::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    bool found = false;
    LevelDoor door;

    foreach(LevelDoor findDoor, lvlScene->LvlData->doors){
        if(m_door.array_id == findDoor.array_id){
            door = findDoor;
            found = true;
            break;
        }
    }

    if(!found)
        return;

    bool isEntrance = m_door.isSetIn;

    if(isEntrance){
        door.ix = m_door.ix;
        door.iy = m_door.iy;
        door.isSetIn = true;
        lvlScene->placeDoorEnter(door, false, false);
    }else{
        door.ox = m_door.ox;
        door.oy = m_door.oy;
        door.isSetOut = true;
        lvlScene->placeDoorExit(door, false, false);
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

}

void HistoryElementPlaceDoor::historyRemoveDoors(const LevelDoor &/*door*/, QGraphicsItem* item)
{
    ((ItemDoor *)item)->removeFromArray();
    if(item) delete (item);
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}
