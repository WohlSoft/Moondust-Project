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
#include <editing/_scenes/level/lvl_scene.h>

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
            if(npc.event_emptylayer!=_fine_npcs[j].event_emptylayer) continue;

            fine=false;
            _found_garbage.push_back(npc);
            break;
        }
        if(fine) _fine_npcs.push_back(npc);
    }

    if(_found_garbage.isEmpty())
        QMessageBox::information(this, tr("NPC garbage clean-up"), tr("Everything is fine, level has no NPC gargabe!"), QMessageBox::Ok);
    else
    {
        QMessageBox::StandardButton x=QMessageBox::question(this, tr("NPC garbage clean-up"), tr("Found %1 junk NPC's. Do you want to remove them?\nPress to \"Help\" button to show up some found junk NPC's").arg(_found_garbage.size()), QMessageBox::Yes|QMessageBox::No|QMessageBox::Help);
        if((x!=QMessageBox::Yes)&&(x!=QMessageBox::Help)) return;
        bool help=(x==QMessageBox::Help);
        LvlScene *sc = box->scene;
        sc->clearSelection();
        LvlScene::PGE_ItemList items=sc->items();
        LevelData removedItems;
        QPointF jumpTo;

        for(int i=0; i<items.size(); i++)
        {
            if(items[i]->data(ITEM_TYPE).toString()=="NPC")
            {
                ItemNPC*npc= qgraphicsitem_cast<ItemNPC*>(items[i]);
                if(!npc) continue;
                for(int j=0; j<_found_garbage.size();j++)
                {
                    if(npc->npcData.array_id==_found_garbage[j].array_id)
                    {
                        if(help) //Select & jump
                        {
                            npc->setSelected(true);
                            jumpTo=npc->scenePos();
                        } else { //Delete actual NPC
                            removedItems.npc.push_back(npc->npcData);
                            npc->removeFromArray();
                            delete npc;
                        }
                        break;
                    }
                }
            }
        }

        if(!help)
        {
            if(!removedItems.npc.isEmpty())
                sc->addRemoveHistory(removedItems);
            QMessageBox::information(this, tr("NPC garbage clean-up"), tr("NPC gargabe has been removed!\nThis operation can be undone with Ctrl+Z or Edit/Undo action."), QMessageBox::Ok);
        }
        else
        {
            box->goTo(jumpTo.x(), jumpTo.y(), true, QPoint(0,0), true);
        }
    }
}

