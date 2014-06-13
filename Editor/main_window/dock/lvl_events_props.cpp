/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"


#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_block.h"
#include "../../level_scene/item_npc.h"
#include "../../level_scene/item_water.h"


void MainWindow::EventListsSync()
{
    int WinType = activeChildWindow();

    ui->PROPS_BlkEventDestroy->clear();
    ui->PROPS_BlkEventHited->clear();
    ui->PROPS_BlkEventLayerEmpty->clear();

    ui->PROPS_NpcEventActivate->clear();
    ui->PROPS_NpcEventDeath->clear();
    ui->PROPS_NpcEventTalk->clear();
    ui->PROPS_NpcEventEmptyLayer->clear();
    ui->LVLEvent_TriggerEvent->clear();

    QString noEvent = tr("[None]");
    ui->PROPS_BlkEventDestroy->addItem(noEvent);
    ui->PROPS_BlkEventHited->addItem(noEvent);
    ui->PROPS_BlkEventLayerEmpty->addItem(noEvent);

    ui->PROPS_NpcEventActivate->addItem(noEvent);
    ui->PROPS_NpcEventDeath->addItem(noEvent);
    ui->PROPS_NpcEventTalk->addItem(noEvent);
    ui->PROPS_NpcEventEmptyLayer->addItem(noEvent);
    ui->LVLEvent_TriggerEvent->addItem(noEvent);


    if (WinType==1)
    {
        foreach(LevelEvents event, activeLvlEditWin()->LvlData.events)
        {
            ui->PROPS_BlkEventDestroy->addItem(event.name);
            ui->PROPS_BlkEventHited->addItem(event.name);
            ui->PROPS_BlkEventLayerEmpty->addItem(event.name);

            ui->PROPS_NpcEventActivate->addItem(event.name);
            ui->PROPS_NpcEventDeath->addItem(event.name);
            ui->PROPS_NpcEventTalk->addItem(event.name);
            ui->PROPS_NpcEventEmptyLayer->addItem(event.name);
            ui->LVLEvent_TriggerEvent->addItem(event.name);
        }
    }

}
