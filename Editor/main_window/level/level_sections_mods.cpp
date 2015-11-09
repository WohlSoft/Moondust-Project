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

#include <editing/edit_level/lvl_clone_section.h>
#include <PGE_File_Formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionCloneSectionTo_triggered()
{
    LvlCloneSection box(this);
    box.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    box.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, box.size(), qApp->desktop()->availableGeometry()));


    //Creating of level files list
    QList<LevelEdit *> openedLeves;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
        {
            openedLeves.push_back(qobject_cast<LevelEdit *>(window->widget()));
        }
    }

    LevelEdit* activeLvlWin=NULL;
    if(activeChildWindow()==1)
        activeLvlWin = activeLvlEditWin();

    if(!activeLvlWin) return;
    box.addLevelList(openedLeves, activeLvlWin);
    if(box.exec()==QDialog::Accepted)
    {
        long x=0;
        long y=0;
        long w=0;
        long h=0;

        QProgressDialog progress(tr("Clonning of section..."), tr("Abort"), 0, 5, this);
        progress.setWindowTitle(tr("Please wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
        progress.setCancelButton(0);
        progress.setMinimumDuration(0);

        LevelEdit * src = box.clone_source;
        int s_id = box.clone_source_id;
        LevelEdit * dst = box.clone_target;
        int d_id = box.clone_target_id;

        //Init target section
        dst->scene->InitSection(d_id);

        x = dst->LvlData.sections[d_id].size_left;
        y = dst->LvlData.sections[d_id].size_top;

        w = labs(src->LvlData.sections[s_id].size_left-
                src->LvlData.sections[s_id].size_right);
        h = labs(src->LvlData.sections[s_id].size_top-
                src->LvlData.sections[s_id].size_bottom);

        if(h!=600)
        {
            QPoint aligned;
            aligned = dst->scene->applyGrid(QPoint(x,y), configs.default_grid);
            x = aligned.x();
            y = aligned.y();
        }

        //copy settings
        dst->LvlData.sections[d_id] = src->LvlData.sections[s_id];
        dst->LvlData.sections[d_id].id = d_id;

        dst->LvlData.sections[d_id].size_left = x;
        dst->LvlData.sections[d_id].size_top = y;
        dst->LvlData.sections[d_id].size_right = x+w;
        dst->LvlData.sections[d_id].size_bottom = y+h;

        dst->LvlData.sections[d_id].PositionX = x-10;
        dst->LvlData.sections[d_id].PositionY = y-10;

        dst->scene->ChangeSectionBG(
                    dst->LvlData.sections[d_id].background,
                    d_id
                );

        if(!progress.wasCanceled()) progress.setValue(1);
        qApp->processEvents();

        //copy items
        QRectF zone;
        zone.setLeft(src->LvlData.sections[s_id].size_left-box.clone_margin);
        zone.setTop(src->LvlData.sections[s_id].size_top-box.clone_margin);
        zone.setRight(src->LvlData.sections[s_id].size_right+box.clone_margin);
        zone.setBottom(src->LvlData.sections[s_id].size_bottom+box.clone_margin);

        src->scene->clearSelection();

        foreach(QGraphicsItem *x, src->scene->items(zone))
        {
            if(x->data(0)=="Block")
                x->setSelected(true);
            else
            if(x->data(0)=="BGO")
                x->setSelected(true);
            if(x->data(0)=="NPC")
                x->setSelected(true);
            if(x->data(0)=="Water")
                x->setSelected(true);
        }

        if(!progress.wasCanceled()) progress.setValue(2);
        qApp->processEvents();

        LevelData buffer = src->scene->copy();

        src->scene->clearSelection();
        //paste into target

        if(!progress.wasCanceled()) progress.setValue(3);
        qApp->processEvents();

        long baseX, baseY;
        bool doCloneItems=true;
        //set first base
        if(!buffer.blocks.isEmpty()){
            baseX = buffer.blocks[0].x;
            baseY = buffer.blocks[0].y;
        }else if(!buffer.bgo.isEmpty()){
            baseX = buffer.bgo[0].x;
            baseY = buffer.bgo[0].y;
        }else if(!buffer.npc.isEmpty()){
            baseX = buffer.npc[0].x;
            baseY = buffer.npc[0].y;
        }else if(!buffer.physez.isEmpty()){
            baseX = buffer.physez[0].x;
            baseY = buffer.physez[0].y;
        }else{
            //nothing to clone
            doCloneItems=false;
        }

        if(!progress.wasCanceled()) progress.setValue(4);
        qApp->processEvents();

        if(doCloneItems)
        {
            foreach (LevelBlock block, buffer.blocks)
            {
                if(block.x<baseX) {
                    baseX = block.x;
                }
                if(block.y<baseY) {
                    baseY = block.y;
                }
            }
            foreach (LevelBGO bgo, buffer.bgo){
                if(bgo.x<baseX){
                    baseX = bgo.x;
                }
                if(bgo.y<baseY){
                    baseY = bgo.y;
                }
            }
            foreach (LevelNPC npc, buffer.npc){
                if(npc.x<baseX){
                    baseX = npc.x;
                }
                if(npc.y<baseY){
                    baseY = npc.y;
                }
            }
            foreach (LevelPhysEnv water, buffer.physez){
                if(water.x<baseX){
                    baseX = water.x;
                }
                if(water.y<baseY){
                    baseY = water.y;
                }
            }

            long targetX;// = baseX;

            if(baseX<src->LvlData.sections[s_id].size_left)
                targetX = x-labs(baseX-src->LvlData.sections[s_id].size_left);
            else
                targetX = x+labs(src->LvlData.sections[s_id].size_left-baseX);
            long targetY;// = baseY;

            if(baseY<src->LvlData.sections[s_id].size_top)
                targetY = y-labs(baseY-src->LvlData.sections[s_id].size_top);
            else
                targetY = y+labs(src->LvlData.sections[s_id].size_top-baseY);

            dst->scene->paste(buffer, QPoint(targetX, targetY));
        }

        if(!progress.wasCanceled()) progress.setValue(5);
        qApp->processEvents();

        progress.close();

        QMessageBox::information(this, tr("Section has been clonned"),
                             tr("Section has been successfully clonned!"));

    }
}


void MainWindow::on_actionSCT_Delete_triggered()
{
    qApp->setActiveWindow(this);
    if(activeChildWindow()==1)
    {
        int id=0;

        if(QMessageBox::question(this, tr("Remove section"),
                                 tr("Do you want to remove all objects of this section?"),
                                 QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
        {
            LevelEdit* edit = activeLvlEditWin();
            QRectF zone;
            bool ok=false;
            long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                           tr("Please select, how far items out of section should be removed too (in pixels)"),
                           32, 0, 214948, 1, &ok);

            if(!ok) return;

            zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-outOfSectionMargin);
            zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-outOfSectionMargin);
            zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+outOfSectionMargin);
            zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+outOfSectionMargin);

            QList<QGraphicsItem *> itemsToRemove;
            foreach(QGraphicsItem *x, edit->scene->items(zone))
            {
                if(!x->data(ITEM_IS_ITEM).isNull())
                    itemsToRemove.push_back(x);
//                else
//                if(x->data(0)=="BGO")
//                    itemsToRemove.push_back(x);
//                if(x->data(0)=="NPC")
//                    itemsToRemove.push_back(x);
//                if(x->data(0)=="Water")
//                    itemsToRemove.push_back(x);
//                if(x->data(0)=="Door_enter")
//                    itemsToRemove.push_back(x);
//                if(x->data(0)=="Door_exit")
//                    itemsToRemove.push_back(x);
//                if(x->data(0)=="playerPoint")
//                    itemsToRemove.push_back(x);
            }

            //remove all items in the section
            edit->scene->removeLvlItems(itemsToRemove);

            id = edit->LvlData.sections[edit->LvlData.CurSection].id;
            edit->LvlData.sections[edit->LvlData.CurSection] = FileFormats::CreateLvlSection();
            edit->LvlData.sections[edit->LvlData.CurSection].id = id;

            edit->scene->ChangeSectionBG(0, edit->LvlData.CurSection);
            edit->scene->drawSpace();
            edit->LvlData.modified=true;

            bool switched=false;
            //find nearly not empty section
            for(int i=0;i<edit->LvlData.sections.size();i++)
            {
                if(
                     (edit->LvlData.sections[i].size_left !=0 )&&
                     (edit->LvlData.sections[i].size_right !=0 )&&
                     (edit->LvlData.sections[i].size_bottom !=0 )&&
                     (edit->LvlData.sections[i].size_top !=0 )
                  )
                {
                    SetCurrentLevelSection(i);
                    switched=true;
                    break;
                }
            }

            if(!switched)
                SetCurrentLevelSection(0);

            edit->scene->Debugger_updateItemList();

            QMessageBox::information(this, tr("Section has been removed"),
                                 tr("Section %1 has been successfully deleted!").arg(id+1));
        }

    }

}


void MainWindow::on_actionSCT_RotateLeft_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        QRectF zone;
        bool ok=false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                       tr("Please select, how far items out of section should be rotated too (in pixels)"),
                       32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+outOfSectionMargin);
        QList<QGraphicsItem *> itemsToModify;
        foreach(QGraphicsItem *x, edit->scene->items(zone))
        {
            if(!x->data(ITEM_IS_ITEM).isNull())
                itemsToModify.push_back(x);
        }
        edit->scene->rotateGroup(itemsToModify, false, true, true);
    }
}

void MainWindow::on_actionSCT_RotateRight_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        QRectF zone;
        bool ok=false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                       tr("Please select, how far items out of section should be rotated too (in pixels)"),
                       32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+outOfSectionMargin);
        QList<QGraphicsItem *> itemsToModify;
        foreach(QGraphicsItem *x, edit->scene->items(zone))
        {
            if(!x->data(ITEM_IS_ITEM).isNull())
                itemsToModify.push_back(x);
        }
        edit->scene->rotateGroup(itemsToModify, true, true, true);
    }
}

void MainWindow::on_actionSCT_FlipHorizontal_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        QRectF zone;
        bool ok=false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                       tr("Please select, how far items out of section should be rotated too (in pixels)"),
                       32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+outOfSectionMargin);
        QList<QGraphicsItem *> itemsToModify;
        foreach(QGraphicsItem *x, edit->scene->items(zone))
        {
            if(!x->data(ITEM_IS_ITEM).isNull())
                itemsToModify.push_back(x);
        }
        edit->scene->flipGroup(itemsToModify, false, true, true);
    }
}

void MainWindow::on_actionSCT_FlipVertical_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        QRectF zone;
        bool ok=false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                       tr("Please select, how far items out of section should be rotated too (in pixels)"),
                       32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+outOfSectionMargin);
        QList<QGraphicsItem *> itemsToModify;
        foreach(QGraphicsItem *x, edit->scene->items(zone))
        {
            if(!x->data(ITEM_IS_ITEM).isNull())
                itemsToModify.push_back(x);
        }
        edit->scene->flipGroup(itemsToModify, true, true, true);
    }
}

