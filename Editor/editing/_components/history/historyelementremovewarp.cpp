/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "historyelementremovewarp.h"
#include <common_features/main_window_ptr.h>

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

    lvlScene->m_data->doors.insert(m_removedDoor.meta.index, m_removedDoor);

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    warplist->insertItem(m_removedDoor.meta.index, QString("%1: x%2y%3 <=> x%4y%5")
                         .arg(m_removedDoor.meta.array_id).arg(m_removedDoor.ix).arg(m_removedDoor.iy).arg(m_removedDoor.ox).arg(m_removedDoor.oy),
                         m_removedDoor.meta.array_id);
    if(warplist->count() > (int)m_removedDoor.meta.index)
    {
        warplist->setCurrentIndex( m_removedDoor.meta.index );
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


    lvlScene->doorPointsSync( m_removedDoor.meta.array_id, true);

    for(int i=0;i<lvlScene->m_data->doors.size();i++)
    {
        if(lvlScene->m_data->doors[i].meta.array_id==m_removedDoor.meta.array_id)
        {
            lvlScene->m_data->doors.removeAt(i);
            break;
        }
    }

    QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
    for(int i = 0; i < warplist->count(); i++){
        if((unsigned int)warplist->itemData(i).toInt() == m_removedDoor.meta.array_id){
            warplist->removeItem(i);
            break;
        }
    }

    if(warplist->count()<=0) MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(false);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}
