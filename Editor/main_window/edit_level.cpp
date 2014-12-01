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
#include "../mainwindow.h"

#include "../file_formats/file_formats.h"


//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelProps LevProps(activeLvlEditWin()->LvlData);
        LevProps.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        LevProps.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, LevProps.size(), qApp->desktop()->availableGeometry()));
        if(LevProps.exec()==QDialog::Accepted)
        {
            QList<QVariant> lvlsetData;
            lvlsetData.push_back(activeLvlEditWin()->LvlData.LevelName);
            lvlsetData.push_back(LevProps.LevelTitle);
            activeLvlEditWin()->scene->addChangeLevelSettingsHistory(LvlScene::SETTING_LEVELNAME, QVariant(lvlsetData));
            activeLvlEditWin()->LvlData.LevelName = LevProps.LevelTitle;
            activeLvlEditWin()->LvlData.modified = true;
            activeLvlEditWin()->setWindowTitle( QString(LevProps.LevelTitle.isEmpty() ? activeLvlEditWin()->userFriendlyCurrentFile() : LevProps.LevelTitle).replace("&", "&&&") );
            updateWindowMenu();
        }
    }

}


// Level tool box show/hide
void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
{
    ui->actionLVLToolBox->setChecked(visible);
}

void MainWindow::on_actionLVLToolBox_triggered(bool checked)
{
    ui->LevelToolBox->setVisible(checked);
    if(checked) ui->LevelToolBox->raise();
}


// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
        ui->actionSection_Settings->setChecked(visible);
}

void MainWindow::on_actionSection_Settings_triggered(bool checked)
{
    ui->LevelSectionSettings->setVisible(checked);
    if(checked) ui->LevelSectionSettings->raise();
}

void MainWindow::on_DoorsToolbox_visibilityChanged(bool visible)
{
    ui->actionWarpsAndDoors->setChecked(visible);
}
void MainWindow::on_actionWarpsAndDoors_triggered(bool checked)
{
    ui->DoorsToolbox->setVisible(checked);
    if(checked) ui->DoorsToolbox->raise();
}



void MainWindow::on_LevelEventsToolBox_visibilityChanged(bool visible)
{
    ui->actionLevelEvents->setChecked(visible);
}

void MainWindow::on_actionLevelEvents_triggered(bool checked)
{
    ui->LevelEventsToolBox->setVisible(checked);
    if(checked) ui->LevelEventsToolBox->raise();
}

void MainWindow::on_FindDock_visibilityChanged(bool visible)
{
    ui->actionLVLSearchBox->setChecked(visible);
}

void MainWindow::on_actionLVLSearchBox_triggered(bool checked)
{
    ui->FindDock->setVisible(checked);
    if(checked) ui->FindDock->raise();
}




// //////////////////////////////// GoTo Section  ////////////////////////////////
void MainWindow::on_actionSection_1_triggered()
{
    SetCurrentLevelSection(0);
}

void MainWindow::on_actionSection_2_triggered()
{
    SetCurrentLevelSection(1);
}

void MainWindow::on_actionSection_3_triggered()
{
    SetCurrentLevelSection(2);
}

void MainWindow::on_actionSection_4_triggered()
{
    SetCurrentLevelSection(3);
}

void MainWindow::on_actionSection_5_triggered()
{
    SetCurrentLevelSection(4);
}

void MainWindow::on_actionSection_6_triggered()
{
    SetCurrentLevelSection(5);
}

void MainWindow::on_actionSection_7_triggered()
{
    SetCurrentLevelSection(6);
}

void MainWindow::on_actionSection_8_triggered()
{
    SetCurrentLevelSection(7);
}

void MainWindow::on_actionSection_9_triggered()
{
    SetCurrentLevelSection(8);
}

void MainWindow::on_actionSection_10_triggered()
{
    SetCurrentLevelSection(9);
}

void MainWindow::on_actionSection_11_triggered()
{
    SetCurrentLevelSection(10);
}

void MainWindow::on_actionSection_12_triggered()
{
    SetCurrentLevelSection(11);
}


void MainWindow::on_actionSection_13_triggered()
{
    SetCurrentLevelSection(12);
}

void MainWindow::on_actionSection_14_triggered()
{
    SetCurrentLevelSection(13);
}

void MainWindow::on_actionSection_15_triggered()
{
    SetCurrentLevelSection(14);
}

void MainWindow::on_actionSection_16_triggered()
{
    SetCurrentLevelSection(15);
}

void MainWindow::on_actionSection_17_triggered()
{
    SetCurrentLevelSection(16);
}

void MainWindow::on_actionSection_18_triggered()
{
    SetCurrentLevelSection(17);
}

void MainWindow::on_actionSection_19_triggered()
{
    SetCurrentLevelSection(18);
}

void MainWindow::on_actionSection_20_triggered()
{
    SetCurrentLevelSection(19);
}

void MainWindow::on_actionSection_21_triggered()
{
    SetCurrentLevelSection(20);
}


void MainWindow::on_actionGo_to_Section_triggered()
{
    qApp->setActiveWindow(this);
    int WinType = activeChildWindow();
    if(WinType==1)
    {
            leveledit* edit = activeLvlEditWin();
            int SectionId = edit->LvlData.CurSection;
            int xb = edit->LvlData.sections[SectionId].size_left;
            int yb = edit->LvlData.sections[SectionId].size_top;
            edit->goTo(xb, yb, false, QPoint(-10, -10));
    }
}




void MainWindow::on_actionCloneSectionTo_triggered()
{

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





// //////////////////////// Locks Begin //////////////////////////////
void MainWindow::on_actionLockBlocks_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(1, checked);
    }
}

void MainWindow::on_actionLockBGO_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(2, checked);
    }
}

void MainWindow::on_actionLockNPC_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(3, checked);
    }
}

void MainWindow::on_actionLockWaters_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(4, checked);
    }
}

void MainWindow::on_actionLockDoors_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(5, checked);
    }
}
// //////////////////////// Locks  End //////////////////////////////



