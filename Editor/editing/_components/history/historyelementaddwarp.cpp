#include "historyelementaddwarp.h"
#include <common_features/mainwinconnect.h>

#include <main_window/dock/lvl_warp_props.h>

HistoryElementAddWarp::HistoryElementAddWarp(int array_id, int listindex, int doorindex, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_listindex(listindex),
    m_doorindex(doorindex)
{}

HistoryElementAddWarp::~HistoryElementAddWarp()
{}

QString HistoryElementAddWarp::getHistoryName()
{
    return tr("Add Warp");
}

void HistoryElementAddWarp::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->doorPointsSync((unsigned int)m_array_id,true);

    for(int i = 0; i < lvlScene->LvlData->doors.size(); i++)
    {
        if(lvlScene->LvlData->doors[i].array_id==(unsigned int)m_array_id)
        {
            lvlScene->LvlData->doors.removeAt(i);
            break;
        }
    }

    bool found = false;

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();

    if((warplist->currentIndex()==m_listindex)&&(warplist->count()>2))
    {
        warplist->setCurrentIndex(warplist->currentIndex()-1);
    }


    WriteToLog(QtDebugMsg, "HistoryManager -> check index");

    if(m_listindex < warplist->count()){
        if(m_array_id == warplist->itemData(m_listindex).toInt()){
            found = true;
            warplist->removeItem(m_listindex);
        }
    }
    WriteToLog(QtDebugMsg, QString("HistoryManager -> found = %1").arg(found));


    if(!found)
    {
        found=false;
        for(int i = 0; i < warplist->count(); i++)
        {
            if(m_array_id == warplist->itemData(i).toInt())
            {
                warplist->removeItem(i);
                found=true;
                break;
            }
        }
    }
    WriteToLog(QtDebugMsg, QString("HistoryManager -> found and removed = %1").arg(found));


    if(warplist->count()<=0) MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(false);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    //warplist->update();
    //warplist->repaint();


}

void HistoryElementAddWarp::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelDoor newDoor = FileFormats::CreateLvlWarp();
    newDoor.array_id = m_array_id;
    newDoor.index = m_doorindex;

    lvlScene->LvlData->doors.insert(m_doorindex, newDoor);
    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    warplist->addItem(QString("%1: x%2y%3 <=> x%4y%5")
                      .arg(newDoor.array_id).arg(newDoor.ix).arg(newDoor.iy).arg(newDoor.ox).arg(newDoor.oy),
                      newDoor.array_id);
    if(warplist->count() < m_listindex)
    {
        warplist->setCurrentIndex( m_listindex );
    }
    else
    {
        warplist->setCurrentIndex( warplist->count()-1 );
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(true);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

}
