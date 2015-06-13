#include "historyelementremovewarp.h"
#include <common_features/mainwinconnect.h>

#include <main_window/dock/lvl_warp_props.h>


HistoryElementRemoveWarp::HistoryElementRemoveWarp(const LevelDoor &door, QObject *parent) :
    QObject(parent),
    m_removedDoor(door)
{}

HistoryElementRemoveWarp::~HistoryElementRemoveWarp()
{}

QString HistoryElementRemoveWarp::getHistoryName()
{
    return tr("Remove Warp");
}

void HistoryElementRemoveWarp::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->doors.insert(m_removedDoor.index, m_removedDoor);

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    warplist->insertItem(m_removedDoor.index, QString("%1: x%2y%3 <=> x%4y%5")
                         .arg(m_removedDoor.array_id).arg(m_removedDoor.ix).arg(m_removedDoor.iy).arg(m_removedDoor.ox).arg(m_removedDoor.oy),
                         m_removedDoor.array_id);
    if(warplist->count() > (int)m_removedDoor.index)
    {
        warplist->setCurrentIndex( m_removedDoor.index );
    }
    else
    {
        warplist->setCurrentIndex( warplist->count()-1 );
    }

    if(m_removedDoor.isSetOut){
        lvlScene->placeDoorExit(m_removedDoor);
    }
    if(m_removedDoor.isSetIn){
        lvlScene->placeDoorEnter(m_removedDoor);
    }


    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}

void HistoryElementRemoveWarp::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


    lvlScene->doorPointsSync( m_removedDoor.array_id, true);

    for(int i=0;i<lvlScene->LvlData->doors.size();i++)
    {
        if(lvlScene->LvlData->doors[i].array_id==m_removedDoor.array_id)
        {
            lvlScene->LvlData->doors.removeAt(i);
            break;
        }
    }

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    for(int i = 0; i < warplist->count(); i++){
        if((unsigned int)warplist->itemData(i).toInt() == m_removedDoor.array_id){
            warplist->removeItem(i);
            break;
        }
    }

    if(warplist->count()<=0) MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(false);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}
