/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"

#include "../../file_formats/file_formats.h"


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



void MainWindow::setDoorData(long index)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        if( (activeLvlEditWin()->LvlData.doors.size() > 0) && (index < activeLvlEditWin()->LvlData.doors.size()) )
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
                    ui->WarpNoYoshi->setChecked(door.noyoshi);

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
}



///////////////////////////////////////////////////////////////
///////////////////// Door Control/////////////////////////////
///////////////////////////////////////////////////////////////
void MainWindow::on_WarpList_currentIndexChanged(int index)
{
    setDoorData(index);
}


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


void MainWindow::on_WarpAdd_clicked()
{
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();
        LevelDoors newDoor = FileFormats::dummyLvlDoor();
        newDoor.array_id = edit->LvlData.doors_array_id++;
        newDoor.index = edit->LvlData.doors.size();
        edit->LvlData.doors.push_back(newDoor);

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
        leveledit* edit = activeLvlEditWin();
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

        if(ui->WarpList->count()<=0) ui->WarpRemove->setEnabled(false);
    }

}



void MainWindow::on_WarpSetEntrance_clicked()
{

}

void MainWindow::on_WarpSetExit_clicked()
{

}


//////////// Flags///////////
void MainWindow::on_WarpNoYoshi_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].noyoshi = checked; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

    }
}
void MainWindow::on_WarpAllowNPC_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].allownpc = checked; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );

    }
}

void MainWindow::on_WarpLock_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].locked = checked; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

/////Door props

void MainWindow::on_WarpType_currentIndexChanged(int index)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].type = index; break;
            }
        }

        ui->WarpEntranceGrp->setEnabled(  index==1 );
        ui->WarpExitGrp->setEnabled( index==1 );

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpNeedAStars_valueChanged(int arg1)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].stars = arg1; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}


/////////Entrance Direction/////////////////
void MainWindow::on_Entr_Down_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].idirect = 3; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}
void MainWindow::on_Entr_Right_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].idirect = 4; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Entr_Up_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].idirect = 1; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}
void MainWindow::on_Entr_Left_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].idirect = 2; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

/////////Exit Direction/////////////////
void MainWindow::on_Exit_Up_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].odirect = 3; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Left_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].odirect = 4; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Down_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].odirect = 1; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_Exit_Right_clicked()
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].odirect = 2; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}



void MainWindow::on_WarpToMapX_textEdited(const QString &arg1)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].world_x = arg1.toInt(); break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpToMapY_textEdited(const QString &arg1)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].world_y = arg1.toInt(); break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}

void MainWindow::on_WarpGetXYFromWorldMap_clicked()
{
    QMessageBox::information(this, "Comming soon", "Selecting point from world map comming with WorldMap Editor in next versions of this programm", QMessageBox::Ok);
}


/////Door mode (Level Entrance / Level Exit)
void MainWindow::on_WarpLevelExit_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();
        bool exists=false;
        int i=0;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {   exists=true;
                edit->LvlData.doors[i].lvl_o = checked; break;
            }
        }

        if(!exists) return;

        //Disable placing door point, if it not avaliable
        ui->WarpSetEntrance->setEnabled( ((!checked) && (!ui->WarpLevelEntrance->isChecked())) || ((checked) && (!ui->WarpLevelEntrance->isChecked())) );
        //Disable placing door point, if it not avaliable
        ui->WarpSetExit->setEnabled( ((!checked) && (!ui->WarpLevelEntrance->isChecked())) || ((ui->WarpLevelEntrance->isChecked())) );

        //Unset placed point, if not it avaliable
        if(! (((!checked) && (!ui->WarpLevelEntrance->isChecked())) || ((ui->WarpLevelEntrance->isChecked()))) )
        {
            ui->WarpExitPlaced->setChecked(false);
            edit->LvlData.doors[i].ox = edit->LvlData.doors[i].ix;
            edit->LvlData.doors[i].oy = edit->LvlData.doors[i].iy;
        }

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}

void MainWindow::on_WarpLevelEntrance_clicked(bool checked)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();
        int i=0;
        bool exists=false;
        for(i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].lvl_i = checked; break;
            }
        }

        if(!exists) return;

        //Disable placing door point, if it not avaliable
        ui->WarpSetEntrance->setEnabled( ((!ui->WarpLevelExit->isChecked()) && (!checked)) || ((ui->WarpLevelExit->isChecked()) && (!checked)) );
         //Unset placed point, if not it avaliable
        if(! (((!ui->WarpLevelExit->isChecked()) && (!checked)) || ((ui->WarpLevelExit->isChecked()) && (!checked))) )
        {
            ui->WarpEntrancePlaced->setChecked(false);
            edit->LvlData.doors[i].ix = edit->LvlData.doors[i].ox;
            edit->LvlData.doors[i].iy = edit->LvlData.doors[i].oy;
        }

        //Disable placing door point, if it not avaliable
        ui->WarpSetExit->setEnabled( ((!ui->WarpLevelExit->isChecked()) && (!checked)) || ((checked)) );

        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }

}

void MainWindow::on_WarpLevelFile_textChanged(const QString &arg1)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].lname = arg1; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}
void MainWindow::on_WarpToExitNu_valueChanged(int arg1)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        leveledit* edit = activeLvlEditWin();

        for(int i=0;i<edit->LvlData.doors.size();i++)
        {
            if(edit->LvlData.doors[i].array_id==(unsigned int)ui->WarpList->currentData().toInt())
            {
                edit->LvlData.doors[i].warpto = arg1; break;
            }
        }
        edit->scene->doorPointsSync( (unsigned int)ui->WarpList->currentData().toInt() );
    }
}
