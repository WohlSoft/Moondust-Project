/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMdiSubWindow>
#include <QInputDialog>

#include <editing/edit_level/lvl_clone_section.h>
#include <PGE_File_Formats/file_formats.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_water.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>


void MainWindow::on_actionCloneSectionTo_triggered()
{
    while(1)
    {
        LvlCloneSection box(this);
        box.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        box.setGeometry(util::alignToScreenCenter(box.size()));


        //Creating of level files list
        QList<LevelEdit *> openedLeves;

        foreach(QMdiSubWindow *window, ui->centralWidget->subWindowList())
        {
            if(QString(window->widget()->metaObject()->className()) == LEVEL_EDIT_CLASS)
                openedLeves.push_back(qobject_cast<LevelEdit *>(window->widget()));
        }

        LevelEdit *activeLvlWin = nullptr;
        if(activeChildWindow() != WND_Level)
            return;

        activeLvlWin = activeLvlEditWin();
        Q_ASSERT(activeLvlWin);
        box.addLevelList(openedLeves, activeLvlWin);

        if(box.exec() == QDialog::Accepted)
        {
            long x = 0;
            long y = 0;
            long w = 0;
            long h = 0;

            QProgressDialog progress(tr("Clonning of section..."), tr("Abort"), 0, 5, this);
            progress.setWindowTitle(tr("Please wait..."));
            progress.setWindowModality(Qt::WindowModal);
            progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            progress.setFixedSize(progress.size());
            progress.setGeometry(util::alignToScreenCenter(progress.size()));
            progress.setCancelButton(0);
            progress.setMinimumDuration(0);

            LevelEdit *src = box.clone_source;
            int s_id = box.clone_source_id;
            LevelEdit *dst = box.clone_target;
            int d_id = box.clone_target_id;

            if(box.override_destinition)
            {
                // Delete destinition section
                deleteLevelSection(dst, d_id, box.clone_margin);
            }

            //Init target section
            dst->scene->InitSection(d_id);

            x = dst->LvlData.sections[d_id].size_left;
            y = dst->LvlData.sections[d_id].size_top;

            w = labs(src->LvlData.sections[s_id].size_left -
                     src->LvlData.sections[s_id].size_right);
            h = labs(src->LvlData.sections[s_id].size_top -
                     src->LvlData.sections[s_id].size_bottom);

            if(h != 600)
            {
                QPoint aligned;
                aligned = dst->scene->applyGrid(QPoint(x, y), configs.defaultGrid.general);
                x = aligned.x();
                y = aligned.y();
            }

            //copy settingsPEZ
            dst->LvlData.sections[d_id] = src->LvlData.sections[s_id];
            dst->LvlData.sections[d_id].id = d_id;

            dst->LvlData.sections[d_id].size_left = x;
            dst->LvlData.sections[d_id].size_top = y;
            dst->LvlData.sections[d_id].size_right = x + w;
            dst->LvlData.sections[d_id].size_bottom = y + h;

            dst->LvlData.sections[d_id].PositionX = x - 10;
            dst->LvlData.sections[d_id].PositionY = y - 10;

            dst->scene->ChangeSectionBG(
                dst->LvlData.sections[d_id].background,
                d_id
            );

            if(!progress.wasCanceled()) progress.setValue(1);
            qApp->processEvents();

            //copy items
            QRectF zone;
            zone.setLeft(src->LvlData.sections[s_id].size_left - box.clone_margin);
            zone.setTop(src->LvlData.sections[s_id].size_top - box.clone_margin);
            zone.setRight(src->LvlData.sections[s_id].size_right + box.clone_margin);
            zone.setBottom(src->LvlData.sections[s_id].size_bottom + box.clone_margin);

            src->scene->clearSelection();

            LvlScene::PGE_ItemList itemsToClone;
            src->scene->queryItems(zone, &itemsToClone);

            LevelData buffer;

            for(QGraphicsItem *x : itemsToClone)
            {
                QString t = x->data(ITEM_TYPE).toString();
                if(t == "Block")
                {
                    ItemBlock *sourceBlock = (ItemBlock *)(x);
                    buffer.blocks.push_back(sourceBlock->m_data);
                }
                else if(t == "BGO")
                {
                    ItemBGO *sourceBGO = (ItemBGO *)(x);
                    buffer.bgo.push_back(sourceBGO->m_data);
                }
                else if(t == "NPC")
                {
                    ItemNPC *sourceNPC = (ItemNPC *)(x);
                    buffer.npc.push_back(sourceNPC->m_data);
                }
                else if(t == "Water")
                {
                    ItemPhysEnv *sourcePEZ = (ItemPhysEnv *)(x);
                    buffer.physez.push_back(sourcePEZ->m_data);
                }
            }

            if(!progress.wasCanceled())
                progress.setValue(2);
            qApp->processEvents();

            //paste into target

            if(!progress.wasCanceled())
                progress.setValue(3);
            qApp->processEvents();

            long baseX = 0, baseY = 0;
            bool doCloneItems = true;
            //set first base
            if(!buffer.blocks.isEmpty())
            {
                baseX = buffer.blocks[0].x;
                baseY = buffer.blocks[0].y;
            }
            else if(!buffer.bgo.isEmpty())
            {
                baseX = buffer.bgo[0].x;
                baseY = buffer.bgo[0].y;
            }
            else if(!buffer.npc.isEmpty())
            {
                baseX = buffer.npc[0].x;
                baseY = buffer.npc[0].y;
            }
            else if(!buffer.physez.isEmpty())
            {
                baseX = buffer.physez[0].x;
                baseY = buffer.physez[0].y;
            }
            else
            {
                //nothing to clone
                doCloneItems = false;
            }

            if(!progress.wasCanceled()) progress.setValue(4);
            qApp->processEvents();

            if(doCloneItems)
            {
                for(LevelBlock block : buffer.blocks)
                {
                    if(block.x < baseX)
                        baseX = block.x;
                    if(block.y < baseY)
                        baseY = block.y;
                }

                for(LevelBGO bgo : buffer.bgo)
                {
                    if(bgo.x < baseX)
                        baseX = bgo.x;
                    if(bgo.y < baseY)
                        baseY = bgo.y;
                }

                for(LevelNPC npc : buffer.npc)
                {
                    if(npc.x < baseX)
                        baseX = npc.x;
                    if(npc.y < baseY)
                        baseY = npc.y;
                }

                for(LevelPhysEnv water : buffer.physez)
                {
                    if(water.x < baseX)
                        baseX = water.x;
                    if(water.y < baseY)
                        baseY = water.y;
                }

                long targetX;// = baseX;

                if(baseX < src->LvlData.sections[s_id].size_left)
                    targetX = x - labs(baseX - src->LvlData.sections[s_id].size_left);
                else
                    targetX = x + labs(src->LvlData.sections[s_id].size_left - baseX);
                long targetY;// = baseY;

                if(baseY < src->LvlData.sections[s_id].size_top)
                    targetY = y - labs(baseY - src->LvlData.sections[s_id].size_top);
                else
                    targetY = y + labs(src->LvlData.sections[s_id].size_top - baseY);

                dst->scene->paste(buffer, QPoint(targetX, targetY));
            }

            if(!progress.wasCanceled())
                progress.setValue(5);
            qApp->processEvents();

            progress.close();

            // Toggle destinition section if destinition level is same as currently toggled
            if(activeLvlWin == dst)
                setCurrentLevelSection(d_id);
            else // Or just change current section of that level without refreshing of current view
                dst->setCurrentSection(d_id);

            // Refresh layer visibility after clonning items
            dst->scene->applyLayersVisible();

            QMessageBox::StandardButton reply =  QMessageBox::information(this,
                                                 tr("Section has been clonned"),
                                                 tr("Section has been successfully cloned!\n"
                                                    "Do you want to clone another section?"),
                                                 QMessageBox::Yes | QMessageBox::No);
            if(reply != QMessageBox::Yes)
                return;
        }
        else
            return;
    }
}


void MainWindow::on_actionSCT_Delete_triggered()
{
    qApp->setActiveWindow(this);
    if(activeChildWindow() == WND_Level)
    {
        if(QMessageBox::question(this, tr("Remove section"),
                                 tr("Do you want to remove all objects of this section?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            bool ok = false;
            long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                                      tr("Please select how far items can travel beyond the section boundaries (in pixels) before they are removed."),
                                      32, 0, 214948, 1, &ok);

            if(!ok)
                return;

            LevelEdit *edit = activeLvlEditWin();
            int deletedSection = edit->LvlData.CurSection;
            deleteLevelSection(edit, deletedSection, outOfSectionMargin);

            bool switched = false;
            // find  a nearest non empty section
            for(int i = 0; i < edit->LvlData.sections.size(); i++)
            {
                if(
                    (edit->LvlData.sections[i].size_left != 0) &&
                    (edit->LvlData.sections[i].size_right != 0) &&
                    (edit->LvlData.sections[i].size_bottom != 0) &&
                    (edit->LvlData.sections[i].size_top != 0)
                )
                {
                    setCurrentLevelSection(i);
                    switched = true;
                    break;
                }
            }

            if(!switched)
                setCurrentLevelSection(0);

            //Pop last section entry if possible
            if(deletedSection > 20 && edit->LvlData.sections.size() > 21)
                edit->LvlData.sections.pop_back();
            edit->scene->Debugger_updateItemList();

            QMessageBox::information(this, tr("Section has been removed"),
                                     tr("Section %1 has been successfully deleted!").arg(deletedSection));
        }

    }

}

void MainWindow::deleteLevelSection(LevelEdit *edit, int section, long margin)
{
    int id = 0;
    QRectF zone;
    zone.setLeft(edit->LvlData.sections[section].size_left - margin);
    zone.setTop(edit->LvlData.sections[section].size_top - margin);
    zone.setRight(edit->LvlData.sections[section].size_right + margin);
    zone.setBottom(edit->LvlData.sections[section].size_bottom + margin);

    LvlScene::PGE_ItemList itemsToRemove;
    edit->scene->queryItems(zone, &itemsToRemove);

    for(auto x = itemsToRemove.begin(); x != itemsToRemove.end(); )
    {
        if((*x)->data(ITEM_IS_ITEM).isNull())
            x = itemsToRemove.erase(x);
        else
            ++x;
    }

    //remove all items in the section
    edit->scene->removeLvlItems(itemsToRemove, false, true);

    id = edit->LvlData.sections[section].id;
    edit->LvlData.sections[section] = FileFormats::CreateLvlSection();
    edit->LvlData.sections[section].id = id;

    edit->scene->ChangeSectionBG(0, section);
    edit->scene->drawSpace();
    edit->LvlData.meta.modified = true;
}



void MainWindow::on_actionSCT_RotateLeft_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();
        QRectF zone;
        bool ok = false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                                  tr("Please select how far items can rotate beyond the section boundaries (in pixels) before they are removed."),
                                  32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left - outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top - outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right + outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom + outOfSectionMargin);

        LvlScene::PGE_ItemList itemsToModify;
        edit->scene->queryItems(zone, &itemsToModify);

        for(auto x = itemsToModify.begin(); x != itemsToModify.end(); )
        {
            if((*x)->data(ITEM_IS_ITEM).isNull())
                x = itemsToModify.erase(x);
            else
                ++x;
        }

        edit->scene->rotateGroup(itemsToModify, false, true, true);
        edit->LvlData.meta.modified = true;
    }
}

void MainWindow::on_actionSCT_RotateRight_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();
        QRectF zone;
        bool ok = false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                                  tr("Please select how far items can rotate beyond the section boundaries (in pixels) before they are removed."),
                                  32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left - outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top - outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right + outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom + outOfSectionMargin);

        LvlScene::PGE_ItemList itemsToModify;
        edit->scene->queryItems(zone, &itemsToModify);

        for(auto x = itemsToModify.begin(); x != itemsToModify.end(); )
        {
            if((*x)->data(ITEM_IS_ITEM).isNull())
                x = itemsToModify.erase(x);
            else
                ++x;
        }

        edit->scene->rotateGroup(itemsToModify, true, true, true);
        edit->LvlData.meta.modified = true;
    }
}

void MainWindow::on_actionSCT_FlipHorizontal_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();
        QRectF zone;
        bool ok = false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                                  tr("Please select how far items can rotate beyond the section boundaries (in pixels) before they are removed."),
                                  32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left - outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top - outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right + outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom + outOfSectionMargin);

        LvlScene::PGE_ItemList itemsToModify;
        edit->scene->queryItems(zone, &itemsToModify);

        for(auto x = itemsToModify.begin(); x != itemsToModify.end(); )
        {
            if((*x)->data(ITEM_IS_ITEM).isNull())
                x = itemsToModify.erase(x);
            else
                ++x;
        }

        edit->scene->flipGroup(itemsToModify, false, true, true);
        edit->LvlData.meta.modified = true;
    }
}

void MainWindow::on_actionSCT_FlipVertical_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();
        QRectF zone;
        bool ok = false;
        long outOfSectionMargin = QInputDialog::getInt(this, tr("Margin of section"),
                                  tr("Please select how far items can rotate beyond the section boundaries (in pixels) before they are removed."),
                                  32, 0, 214948, 1, &ok);

        if(!ok) return;

        zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left - outOfSectionMargin);
        zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top - outOfSectionMargin);
        zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right + outOfSectionMargin);
        zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom + outOfSectionMargin);

        LvlScene::PGE_ItemList itemsToModify;
        edit->scene->queryItems(zone, &itemsToModify);

        for(auto x = itemsToModify.begin(); x != itemsToModify.end(); )
        {
            if((*x)->data(ITEM_IS_ITEM).isNull())
                x = itemsToModify.erase(x);
            else
                ++x;
        }

        edit->scene->flipGroup(itemsToModify, true, true, true);
        edit->LvlData.meta.modified = true;
    }
}
