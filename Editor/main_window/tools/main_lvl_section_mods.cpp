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

#include <ui_mainwindow.h>
#include "../../mainwindow.h"

#include "../../file_formats/file_formats.h"
#include "../../edit_level/lvl_clone_section.h"



void MainWindow::on_actionCloneSectionTo_triggered()
{
    LvlCloneSection box;
    box.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    box.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, box.size(), qApp->desktop()->availableGeometry()));


    //Creating of level files list
    QList<leveledit *> openedLeves;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
            openedLeves.push_back(qobject_cast<leveledit *>(window->widget()));
        }
    }

    leveledit* activeLvlWin=NULL;
    if(activeChildWindow()==1)
        activeLvlWin = activeLvlEditWin();

    box.addLevelList(openedLeves, activeLvlWin);
    box.exec();

}


void MainWindow::on_actionSCT_Delete_triggered()
{
    qApp->setActiveWindow(this);
    if(activeChildWindow()==1)
    {

        if(QMessageBox::question(this, tr("Remove section"),
                                 tr("Do you want to remove all objects of this section?"),
                                 QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
        {
            leveledit* edit = activeLvlEditWin();
            QRectF zone;
            bool ok=false;
            long padding = QInputDialog::getInt(this, tr("Section padding"),
                           tr("Please enter, how far items out of section should be removed (in pixels)"),
                           32, 0, 214948, 1, &ok);

            if(!ok) return;

            zone.setLeft(edit->LvlData.sections[edit->LvlData.CurSection].size_left-padding);
            zone.setTop(edit->LvlData.sections[edit->LvlData.CurSection].size_top-padding);
            zone.setRight(edit->LvlData.sections[edit->LvlData.CurSection].size_right+padding);
            zone.setBottom(edit->LvlData.sections[edit->LvlData.CurSection].size_bottom+padding);

            QList<QGraphicsItem *> itemsToRemove;
            foreach(QGraphicsItem *x, edit->scene->items(zone))
            {
                if(x->data(0)=="Block")
                    itemsToRemove.push_back(x);
                else
                if(x->data(0)=="BGO")
                    itemsToRemove.push_back(x);
                if(x->data(0)=="NPC")
                    itemsToRemove.push_back(x);
                if(x->data(0)=="Water")
                    itemsToRemove.push_back(x);
                if(x->data(0)=="Door_enter")
                    itemsToRemove.push_back(x);
                if(x->data(0)=="Door_exit")
                    itemsToRemove.push_back(x);
                if(x->data(0)=="playerPoint")
                    itemsToRemove.push_back(x);
            }

            //remove all items in the section
            edit->scene->removeLvlItems(itemsToRemove);

            int id = edit->LvlData.sections[edit->LvlData.CurSection].id;
            edit->LvlData.sections[edit->LvlData.CurSection] = FileFormats::dummyLvlSection();
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
        }
    }

}


void MainWindow::on_actionSCT_RotateLeft_triggered()
{

}

void MainWindow::on_actionSCT_RotateRight_triggered()
{

}

void MainWindow::on_actionSCT_FlipHorizontal_triggered()
{

}

void MainWindow::on_actionSCT_FlipVertical_triggered()
{

}

