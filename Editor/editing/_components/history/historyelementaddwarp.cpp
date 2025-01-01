/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QComboBox>

#include "historyelementaddwarp.h"
#include <common_features/main_window_ptr.h>

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

    for(int i = 0; i < lvlScene->m_data->doors.size(); i++)
    {
        if(lvlScene->m_data->doors[i].meta.array_id==(unsigned int)m_array_id)
        {
            lvlScene->m_data->doors.removeAt(i);
            break;
        }
    }

    bool found = false;

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();

    if((warplist->currentIndex()==m_listindex)&&(warplist->count()>2))
    {
        warplist->setCurrentIndex(warplist->currentIndex()-1);
    }


    LogDebug("HistoryManager -> check index");

    if(m_listindex < warplist->count()){
        if(m_array_id == warplist->itemData(m_listindex).toInt()){
            found = true;
            warplist->removeItem(m_listindex);
        }
    }
    LogDebug(QString("HistoryManager -> found = %1").arg(found));


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
    LogDebug(QString("HistoryManager -> found and removed = %1").arg(found));


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
    newDoor.meta.array_id = m_array_id;
    newDoor.meta.index = m_doorindex;

    lvlScene->m_data->doors.insert(m_doorindex, newDoor);
    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    warplist->addItem(QString("%1: x%2y%3 <=> x%4y%5")
                      .arg(newDoor.meta.array_id).arg(newDoor.ix).arg(newDoor.iy).arg(newDoor.ox).arg(newDoor.oy),
                      newDoor.meta.array_id);
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
