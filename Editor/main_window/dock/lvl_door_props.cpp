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
#include "../../editing/_scenes/level/lvl_item_placing.h"

#include "../../editing/_dialogs/levelfilelist.h"

#include "../../editing/_dialogs/wld_setpoint.h"

#include <QInputDialog>

bool lockSetSettings=false;


void MainWindow::on_DoorsToolbox_visibilityChanged(bool visible)
{
    ui->actionWarpsAndDoors->setChecked(visible);
}
void MainWindow::on_actionWarpsAndDoors_triggered(bool checked)
{
    ui->DoorsToolbox->setVisible(checked);
    if(checked) ui->DoorsToolbox->raise();
}


void MainWindow::setDoorsToolbox()
{
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->WarpList->clear();
        foreach(LevelDoors door, activeLvlEditWin()->LvlData.doors)
        {
            ui->WarpList->addItem(QString("%1: x%2y%3 <=> x%4y%5")
           .arg(door.array_id).arg(door.ix).arg(door.iy).arg(door.ox).arg(door.oy),
                                  door.array_id);
        }
        if(ui->WarpList->count()<=0)
            setDoorData(-1);
        else
            setDoorData( ui->WarpList->currentIndex() );

    }

}

void MainWindow::SwitchToDoor(long arrayID)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        ui->DoorsToolbox->show();
        ui->DoorsToolbox->raise();
        ui->WarpList->setCurrentIndex(ui->WarpList->findData(QString::number(arrayID)));
    }

}


void MainWindow::setDoorData(long index)
{
    long cIndex;
    if(index==-2)
        cIndex=ui->WarpList->currentIndex();
    else
        cIndex = index;

    lockSetSettings=true;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        if( (activeLvlEditWin()->LvlData.doors.size() > 0) && (cIndex < activeLvlEditWin()->LvlData.doors.size()) )
        {
            foreach(LevelDoors door, activeLvlEditWin()->LvlData.doors)
            {
                if(door.array_id == (unsigned long)ui->WarpList->currentData().toInt() )
                {
                    ui->WarpRemove->setEnabled(true);

                    ui->WarpAllowNPC->setEnabled(true);
                    ui->WarpAllowNPC->setChecked(door.allownpc);

                    ui->WarpLock->setEnabled(true);
                    ui->WarpLock->setChecked(door.locked);

                    ui->WarpNoYoshi->setEnabled(true);
                    ui->WarpNoYoshi->setChecked(door.novehicles);

                    ui->WarpType->setEnabled(true);
                    ui->WarpType->setCurrentIndex(door.type);

                    ui->WarpEntrancePlaced->setChecked(door.isSetIn);
                    ui->WarpExitPlaced->setChecked(door.isSetOut);

                    ui->WarpNeedAStars->setEnabled(true);
                    ui->WarpNeedAStars->setValue(door.stars);

                    ui->WarpEntranceGrp->setEnabled(  door.type==1 );
                    ui->WarpExitGrp->setEnabled( door.type==1 );

                    switch(door.idirect)
                    { //Entrance direction: [3] down, [1] up, [2] left, [4] right
                    case 1:
                        ui->Entr_Up->setChecked(true);
                        break;
                    case 2:
                        ui->Entr_Left->setChecked(true);
                        break;
                    case 3:
                        ui->Entr_Down->setChecked(true);
                        break;
                    case 4:
                        ui->Entr_Right->setChecked(true);
                        break;
                    default:
                        ui->Entr_Down->setChecked(true);
                        break;
                    }


                    switch(door.odirect)
                    { //Exit direction: [1] down [3] up [4] left [2] right
                    case 1:
                        ui->Exit_Down->setChecked(true);
                        break;
                    case 2:
                        ui->Exit_Right->setChecked(true);
                        break;
                    case 3:
                        ui->Exit_Up->setChecked(true);
                        break;
                    case 4:
                        ui->Exit_Left->setChecked(true);
                        break;
                    default:
                        ui->Exit_Up->setChecked(true);
                        break;
                    }

                    ui->WarpToMapX->setEnabled(true);
                    ui->WarpToMapX->setText((door.world_x!=-1)?QString::number(door.world_x):"");

                    ui->WarpToMapY->setEnabled(true);
                    ui->WarpToMapY->setText((door.world_y!=-1)?QString::number(door.world_y):"");
                    ui->WarpGetXYFromWorldMap->setEnabled(true);


                    ui->WarpLevelExit->setEnabled(true);
                    ui->WarpLevelExit->setChecked( door.lvl_o );

                    ui->WarpLevelEntrance->setEnabled(true);
                    ui->WarpLevelEntrance->setChecked( door.lvl_i );

                    ui->WarpSetEntrance->setEnabled( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) );
                    ui->WarpSetExit->setEnabled( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) );

                    ui->WarpLevelFile->setEnabled(true);
                    ui->WarpLevelFile->setText( door.lname );
                    ui->WarpBrowseLevels->setEnabled(true);
                    ui->WarpToExitNu->setValue(door.warpto);
                    ui->WarpToExitNu->setEnabled(true);

                    break;
                }
            }
        }
        else
        {
            ui->WarpRemove->setEnabled(false);

            ui->WarpSetEntrance->setEnabled(false);
            ui->WarpSetExit->setEnabled(false);

            ui->WarpNoYoshi->setEnabled(false);
            ui->WarpNoYoshi->setChecked(false);
            ui->WarpAllowNPC->setEnabled(false);
            ui->WarpAllowNPC->setChecked(false);
            ui->WarpLock->setEnabled(false);
            ui->WarpLock->setChecked(false);

            ui->WarpType->setEnabled(false);
            ui->WarpType->setCurrentIndex(0);
            ui->WarpNeedAStars->setEnabled(false);
            ui->WarpNeedAStars->setValue(0);

            ui->WarpEntranceGrp->setEnabled(false);
            ui->WarpExitGrp->setEnabled(false);

            ui->WarpToMapX->setEnabled(false);
            ui->WarpToMapX->setText("");
            ui->WarpToMapY->setEnabled(false);
            ui->WarpToMapY->setText("");
            ui->WarpGetXYFromWorldMap->setEnabled(false);

            ui->WarpLevelExit->setEnabled(false);
            ui->WarpLevelExit->setChecked(false);
            ui->WarpLevelEntrance->setEnabled(false);
            ui->WarpLevelEntrance->setChecked(false);
            ui->WarpLevelFile->setEnabled(false);
            ui->WarpLevelFile->setText("");
            ui->WarpBrowseLevels->setEnabled(false);
            ui->WarpToExitNu->setEnabled(false);

        }
    }
    lockSetSettings=false;
}



///////////////////////////////////////////////////////////////
///////////////////// Door Control/////////////////////////////
///////////////////////////////////////////////////////////////
void MainWindow::on_WarpList_currentIndexChanged(int index)
{
    setDoorData(index);
}

/*
void MainWindow::on_goToWarpDoor_clicked()
{
    int WinType = activeChildWindow();
    if(WinType==1)
    {
        leveledit* edit = activeLvlEditWin();
        unsigned int doorID = ui->WarpList->currentData().toInt();
        foreach (LevelDoors door, edit->LvlData.doors) {
            if(doorID == door.array_id){
                if(door.isSetIn) //If enter point was placed
                    edit->goTo(door.ix, door.iy, true, QPoint(-300, -300)); //Goto entrance point
            }
        }
    }
}
*/


void MainWindow::on_WarpAdd_clicked()
{
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        LevelDoors newDoor = FileFormats::dummyLvlDoor();
        newDoor.array_id = edit->LvlData.doors_array_id++;
        newDoor.index = edit->LvlData.doors.size();
        edit->LvlData.doors.push_back(newDoor);

        edit->scene->addAddWarpHistory(newDoor.array_id, ui->WarpList->count(), newDoor.index);

        ui->WarpList->addItem(QString("%1: x%2y%3 <=> x%4y%5")
       .arg(newDoor.array_id).arg(newDoor.ix).arg(newDoor.iy).arg(newDoor.ox).arg(newDoor.oy),
                              newDoor.array_id);
        ui->WarpList->setCurrentIndex( ui->WarpList->count()-1 );
        ui->WarpRemove->setEnabled(true);
    }

}
void MainWindow::on_WarpRemove_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->scene->addRemoveWarpHistory(edit->LvlData.doors[i]);
                break;
            }
        }

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt(), true);

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors.remove(i);
                break;
            }
        }

        ui->WarpList->removeItem( ui->WarpList->currentIndex() );

        //if(ui->WarpList->count()<=0) ui->WarpRemove->setEnabled(false);
        if(ui->WarpList->count()<=0) setWarpRemoveButtonEnabled(false);
    }

}

void MainWindow::on_WarpSetEntrance_clicked()
{
    //placeDoorEntrance
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        bool placed=false;
        int i=0;
        int array_id = 0;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                placed = edit->LvlData.doors[i].isSetIn;
                array_id = edit->LvlData.doors[i].array_id;
                break;
            }
        }

        if(placed)
        {
               edit->goTo(edit->LvlData.doors[i].ix, edit->LvlData.doors[i].iy, true, QPoint(0, 0), true);
               //deselect all and select placed one
               foreach (QGraphicsItem* i, edit->scene->selectedItems())
               {
                   i->setSelected(false);
               }
               foreach (QGraphicsItem* item, edit->scene->items())
               {
                   if(item->data(0).toString()=="Door_enter")
                   {
                       if(item->data(2).toInt()==array_id)
                       {
                           item->setSelected(true);
                           break;
                       }
                   }
               }

               return;
        }

       resetEditmodeButtons();

       edit->scene->clearSelection();
       edit->changeCursor(LevelEdit::MODE_PlaceItem);
       edit->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
       edit->scene->setItemPlacer(4, ui->WarpList->currentData().toInt(), LvlPlacingItems::DOOR_Entrance);

       ui->ItemProperties->hide();

       edit->setFocus();
    }
}

void MainWindow::on_WarpSetExit_clicked()
{
    //placeDoorEntrance
    if(activeChildWindow()==1)
    {
        LevelEdit* edit = activeLvlEditWin();
        bool placed=false;
        int i=0;
        int array_id = 0;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                placed = edit->LvlData.doors[i].isSetOut;
                array_id = edit->LvlData.doors[i].array_id;
                break;
            }
        }

        if(placed)
        {
               edit->goTo(edit->LvlData.doors[i].ox, edit->LvlData.doors[i].oy, true, QPoint(0, 0), true);
               //deselect all and select placed one
               foreach (QGraphicsItem* i, edit->scene->selectedItems())
               {
                   i->setSelected(false);
               }
               foreach (QGraphicsItem* item, edit->scene->items())
               {
                   if(item->data(0).toString()=="Door_exit")
                   {
                       if(item->data(2).toInt()==array_id)
                       {
                           item->setSelected(true);
                           break;
                       }
                   }
               }
               return;
        }

        resetEditmodeButtons();

        edit->scene->clearSelection();
        edit->changeCursor(LevelEdit::MODE_PlaceItem);
        edit->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
        edit->scene->setItemPlacer(4, ui->WarpList->currentData().toInt(), LvlPlacingItems::DOOR_Exit);

       ui->ItemProperties->hide();

       activeLvlEditWin()->setFocus();
    }
}


//////////// Flags///////////
void MainWindow::on_WarpNoYoshi_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].novehicles = checked; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory(ui->WarpList->currentData().toInt(), LvlScene::SETTING_NOYOSHI, QVariant(checked));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

    }
}
void MainWindow::on_WarpAllowNPC_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].allownpc = checked; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory(ui->WarpList->currentData().toInt(), LvlScene::SETTING_ALLOWNPC, QVariant(checked));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

    }
}

void MainWindow::on_WarpLock_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].locked = checked; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory(ui->WarpList->currentData().toInt(), LvlScene::SETTING_LOCKED, QVariant(checked));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

/////Door props

void MainWindow::on_WarpType_currentIndexChanged(int index)
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> warpTypeData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                warpTypeData.push_back(edit->LvlData.doors[i].type);
                warpTypeData.push_back(index);
                edit->LvlData.doors[i].type = index; break;
            }
        }

        ui->WarpEntranceGrp->setEnabled(  index==1 );
        ui->WarpExitGrp->setEnabled( index==1 );

        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_WARPTYPE, QVariant(warpTypeData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpNeedAStars_valueChanged(int arg1)
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> starData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                starData.push_back(edit->LvlData.doors[i].stars);
                starData.push_back(arg1);
                edit->LvlData.doors[i].stars = arg1; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_NEEDASTAR, QVariant(starData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}


/////////Entrance Direction/////////////////
void MainWindow::on_Entr_Down_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].idirect);
                dirData.push_back(3);
                edit->LvlData.doors[i].idirect = 3; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_ENTRDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}
void MainWindow::on_Entr_Right_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].idirect);
                dirData.push_back(4);
                edit->LvlData.doors[i].idirect = 4; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_ENTRDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Entr_Up_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].idirect);
                dirData.push_back(1);
                edit->LvlData.doors[i].idirect = 1; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_ENTRDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}
void MainWindow::on_Entr_Left_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].idirect);
                dirData.push_back(2);
                edit->LvlData.doors[i].idirect = 2; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_ENTRDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

/////////Exit Direction/////////////////
void MainWindow::on_Exit_Up_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].odirect);
                dirData.push_back(3);
                edit->LvlData.doors[i].odirect = 3; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_EXITDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Left_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].odirect);
                dirData.push_back(4);
                edit->LvlData.doors[i].odirect = 4; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_EXITDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Down_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].odirect);
                dirData.push_back(1);
                edit->LvlData.doors[i].odirect = 1; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_EXITDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Right_clicked()
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> dirData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                dirData.push_back(edit->LvlData.doors[i].odirect);
                dirData.push_back(2);
                edit->LvlData.doors[i].odirect = 2; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_EXITDIR, QVariant(dirData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}



void MainWindow::on_WarpToMapX_editingFinished()//_textEdited(const QString &arg1)
{
    if(lockSetSettings) return;

    if(!ui->WarpToMapX->isModified()) return;
    ui->WarpToMapX->setModified(false);

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        QString arg1 = ui->WarpToMapX->text();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                if(arg1.isEmpty())
                    edit->LvlData.doors[i].world_x = -1;
                else
                    edit->LvlData.doors[i].world_x = arg1.toInt();

                edit->LvlData.modified = true;
                break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpToMapY_editingFinished()//_textEdited(const QString &arg1)
{
    if(lockSetSettings) return;

    if(!ui->WarpToMapY->isModified()) return;
    ui->WarpToMapY->setModified(false);

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        QString arg1 = ui->WarpToMapY->text();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                if(arg1.isEmpty())
                    edit->LvlData.doors[i].world_y = -1;
                else
                    edit->LvlData.doors[i].world_y = arg1.toInt();

                edit->LvlData.modified = true;
                break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpGetXYFromWorldMap_clicked()
{
    if(lockSetSettings) return;

    // QMessageBox::information(this, "Comming soon", "Selecting point from world map comming with WorldMap Editor in next versions of this programm", QMessageBox::Ok);

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QString woldMaps_path;
        QString woldMaps_file;
        woldMaps_path = activeLvlEditWin()->LvlData.path;

        QStringList filters;
        QStringList files;
        QDir levelDir(woldMaps_path);
        filters << "*.wld" << "*.wldx";
        levelDir.setSorting(QDir::Name);
        levelDir.setNameFilters(filters);

        files = levelDir.entryList(filters);
        if(files.isEmpty())
        {
            QMessageBox::warning(this,
             tr("World map files not found"),
             tr("You haven't available world map files with this level file.\n"
                "Please, put this level file with a world map, "
                "or create new world map in the same fomder with this level file.\n"
                "File path: %1").arg(woldMaps_path), QMessageBox::Ok);
            return;
        }

        bool ok=true;
        if(files.count()==1)
            woldMaps_file = files.first();
        else
            woldMaps_file = QInputDialog::getItem(this, tr("Select world map file"),
               tr("Found more than one world map files.\n"
               "Please, select necessary world map in a list:"),
                files, 0, false, &ok);

        if(woldMaps_file.isEmpty() || !ok) return;

        QString wldPath = QString("%1/%2").arg(woldMaps_path).arg(woldMaps_file);


        QFile file(wldPath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
                return;
        }
        QFileInfo in_1(wldPath);

        WorldData FileData = FileFormats::ReadWorldFile(file);
        if( !FileData.ReadFileValid ) return;

        WLD_SetPoint * pointDialog = new WLD_SetPoint;

        pointDialog->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        pointDialog->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pointDialog->size(), qApp->desktop()->availableGeometry()));

        if ( (bool)(pointDialog->loadFile(wldPath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            pointDialog->ResetPosition();
            if(ui->WarpToMapX->text().isEmpty() || ui->WarpToMapY->text().isEmpty())
            {
                pointDialog->mapPointIsNull=true;
            }
            else
            {
                pointDialog->pointSelected(
                            QPoint(ui->WarpToMapX->text().toInt(),
                                   ui->WarpToMapY->text().toInt())    );
                pointDialog->goTo(ui->WarpToMapX->text().toInt()+16, ui->WarpToMapY->text().toInt()+16,
                                  false,
                                    QPoint(-qRound(qreal(pointDialog->gViewPort()->width())/2), -qRound(qreal(pointDialog->gViewPort()->height())/2))
                                  );
                pointDialog->scene->setPoint( pointDialog->mapPoint );
            }

            if( pointDialog->exec()==QDialog::Accepted )
            {
                ui->WarpToMapX->setText(QString::number(pointDialog->mapPoint.x()));
                ui->WarpToMapY->setText(QString::number(pointDialog->mapPoint.y()));
                ui->WarpToMapX->setModified(true);
                ui->WarpToMapY->setModified(true);
                on_WarpToMapX_editingFinished();
                on_WarpToMapY_editingFinished();

            }
        } else {
            //pointDialog->close();
        }
        delete pointDialog;

        //QMessageBox::information(this, "Gotten file", QString("%1/%2").arg(woldMaps_path).arg(woldMaps_file), QMessageBox::Ok);

        //ui->FileList->insertItems(levelDir.entryList().size(), levelDir.entryList(filters) );
    }


}


/////Door mode (Level Entrance / Level Exit)
void MainWindow::on_WarpLevelExit_clicked(bool checked)
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> extraData;
        LevelEdit* edit = activeLvlEditWin();
        bool exists=false;
        int i=0;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                exists=true;
                extraData.push_back(checked);
                if(checked){
                    extraData.push_back((int)edit->LvlData.doors[i].ox);
                    extraData.push_back((int)edit->LvlData.doors[i].oy);
                }
                edit->LvlData.doors[i].lvl_o = checked; break;
            }
        }

        if(!exists) return;

        //Disable placing door point, if it not avaliable
        ui->WarpSetEntrance->setEnabled(
                    ((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
                    ((edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i))
                    );
        //Disable placing door point, if it not avaliable
        ui->WarpSetExit->setEnabled(
                    ((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
                    (edit->LvlData.doors[i].lvl_i) );

        bool iPlaced = edit->LvlData.doors[i].isSetIn;
        bool oPlaced = edit->LvlData.doors[i].isSetOut;

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

        //Unset placed point, if not it avaliable
        if(! (((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
              (edit->LvlData.doors[i].lvl_i) ) )
        {
            oPlaced=false;
            ui->WarpExitPlaced->setChecked(false);
            edit->LvlData.doors[i].ox = edit->LvlData.doors[i].ix;
            edit->LvlData.doors[i].oy = edit->LvlData.doors[i].iy;
        }

        edit->LvlData.doors[i].isSetIn = iPlaced;
        edit->LvlData.doors[i].isSetOut = oPlaced;

        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_LEVELEXIT, QVariant(extraData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}

void MainWindow::on_WarpLevelEntrance_clicked(bool checked)
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> extraData;
        LevelEdit* edit = activeLvlEditWin();
        int i=0;
        bool exists=false;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                exists=true;
                extraData.push_back(checked);
                if(checked){
                    extraData.push_back((int)edit->LvlData.doors[i].ix);
                    extraData.push_back((int)edit->LvlData.doors[i].iy);
                }
                edit->LvlData.doors[i].lvl_i = checked; break;
            }
        }

        if(!exists) return;

        //Disable placing door point, if it not avaliable
        ui->WarpSetEntrance->setEnabled(
                    ((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
                    ((edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) );
        //Disable placing door point, if it not avaliable
        ui->WarpSetExit->setEnabled(
                    ((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
                    (edit->LvlData.doors[i].lvl_i) );

        bool iPlaced = edit->LvlData.doors[i].isSetIn;
        bool oPlaced = edit->LvlData.doors[i].isSetOut;

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

        //Unset placed point, if not it avaliable
        if(! (((!edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i)) ||
              ((edit->LvlData.doors[i].lvl_o) && (!edit->LvlData.doors[i].lvl_i))) )
        {
            iPlaced=false;
            ui->WarpEntrancePlaced->setChecked(false);
            edit->LvlData.doors[i].ix = edit->LvlData.doors[i].ox;
            edit->LvlData.doors[i].iy = edit->LvlData.doors[i].oy;
        }

        edit->LvlData.doors[i].isSetIn = iPlaced;
        edit->LvlData.doors[i].isSetOut = oPlaced;

        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_LEVELENTR, QVariant(extraData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}


void MainWindow::on_WarpBrowseLevels_clicked()
{
    QString dirPath;
    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    LevelFileList levelList(dirPath, ui->WarpLevelFile->text());
    if( levelList.exec() == QDialog::Accepted )
    {
        ui->WarpLevelFile->setText(levelList.SelectedFile);
        ui->WarpLevelFile->setModified(true);
        on_WarpLevelFile_editingFinished();
    }
}

void MainWindow::on_WarpLevelFile_editingFinished()//_textChanged(const QString &arg1)
{
    if(lockSetSettings) return;

    if(!ui->WarpLevelFile->isModified()) return;
    ui->WarpLevelFile->setModified(false);

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].lname = ui->WarpLevelFile->text(); break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}
void MainWindow::on_WarpToExitNu_valueChanged(int arg1)
{
    if(lockSetSettings) return;

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        QList<QVariant> warpToData;
        LevelEdit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                warpToData.push_back((int)edit->LvlData.doors[i].warpto);
                warpToData.push_back(arg1);
                edit->LvlData.doors[i].warpto = arg1; break;
            }
        }
        edit->scene->addChangeWarpSettingsHistory((unsigned int)ui->WarpList->currentData().toInt(), LvlScene::SETTING_LEVELWARPTO, QVariant(warpToData));
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}


QComboBox *MainWindow::getWarpList()
{
    return ui->WarpList;
}

void MainWindow::removeItemFromWarpList(int index)
{
    ui->WarpList->removeItem(index);
}


void MainWindow::setWarpRemoveButtonEnabled(bool isEnabled)
{
    ui->WarpRemove->setEnabled(isEnabled);
}
