/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QList>
#include <QMessageBox>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <PGE_File_Formats/lvl_filedata.h>

void MainWindow::on_actionClear_NPC_garbadge_triggered()
{
    if(activeChildWindow()!=1) return;
    LevelEdit * box = activeLvlEditWin();
    if(!box) return;
    if(!box->sceneCreated) return;

    QList<LevelNPC> _fine_npcs;
    QList<LevelNPC> _found_garbage;
    for(int i=0; i<box->LvlData.npc.size(); i++)
    {
        LevelNPC npc=box->LvlData.npc[i];
        bool fine=true;
        for(int j=0;j<_fine_npcs.size(); j++)
        {
            if(npc.id!=_fine_npcs[j].id) continue;
            if(npc.x!=_fine_npcs[j].x) continue;
            if(npc.y!=_fine_npcs[j].y) continue;
            if(npc.layer!=_fine_npcs[j].layer) continue;
            if(npc.generator!=_fine_npcs[j].generator) continue;
            if(npc.msg!=_fine_npcs[j].msg) continue;
            if(npc.event_activate!=_fine_npcs[j].event_activate) continue;
            if(npc.event_die!=_fine_npcs[j].event_die) continue;
            if(npc.event_talk!=_fine_npcs[j].event_talk) continue;
            if(npc.event_nomore!=_fine_npcs[j].event_nomore) continue;

            fine=false;
            _found_garbage.push_back(npc);
            break;
        }
        if(fine) _fine_npcs.push_back(npc);
    }

    if(_found_garbage.isEmpty())
        QMessageBox::information(this, tr("NPC garbage clean-up"), tr("Everything is fine, level has no NPC gargabe!"), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("NPC garbage clean-up"), tr("Found %1 junk NPC's").arg(_found_garbage.size()), QMessageBox::Ok);

}

