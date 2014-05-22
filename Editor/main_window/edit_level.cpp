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


#include "../ui_mainwindow.h"
#include "../mainwindow.h"


//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelProps LevProps(activeLvlEditWin()->LvlData);
        if(LevProps.exec()==QDialog::Accepted)
        {
            activeLvlEditWin()->LvlData.playmusic = LevProps.AutoPlayMusic;
            activeLvlEditWin()->LvlData.LevelName = LevProps.LevelTitle;
            autoPlayMusic = LevProps.AutoPlayMusic;
        }
    }

}


// Level tool box show/hide
void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
{
    ui->actionLVLToolBox->setChecked(visible);
}

void MainWindow::on_actionLVLToolBox_triggered()
{
    if(ui->actionLVLToolBox->isChecked())
    {
        ui->LevelToolBox->setVisible(true);
        ui->LevelToolBox->raise();
    }
    else
        ui->LevelToolBox->setVisible(false);
}


// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
        ui->actionSection_Settings->setChecked(visible);
}

void MainWindow::on_actionSection_Settings_triggered()
{
    if(ui->actionSection_Settings->isChecked())
    {
        ui->LevelSectionSettings->setVisible(true);
        ui->LevelSectionSettings->raise();
    }
    else
        ui->LevelSectionSettings->setVisible(false);
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




void MainWindow::on_BGOUniform_clicked(bool checked)
{
    ui->BGOItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
}

void MainWindow::on_BlockUniform_clicked(bool checked)
{
    ui->BlockItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
}


void MainWindow::on_NPCUniform_clicked(bool checked)
{
    ui->NPCItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
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
// //////////////////////////////////////////////////////////////

void MainWindow::on_actionGridEn_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->grid = checked;
    }
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
    }

}

void MainWindow::setDoorData(long index)
{
    int WinType = activeChildWindow();
    if (WinType==1)
    {
        if(index<activeLvlEditWin()->LvlData.doors.size())
        {
            foreach(LevelDoors door, activeLvlEditWin()->LvlData.doors)
            {
                if(door.array_id == (unsigned long)ui->WarpList->currentData().toInt() )
                {
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


                    ui->WarpLevelExit->setEnabled(true);
                    ui->WarpLevelExit->setChecked( door.lvl_o );

                    ui->WarpLevelEntrance->setEnabled(true);
                    ui->WarpLevelEntrance->setChecked( door.lvl_i );

                    ui->WarpSetEntrance->setEnabled( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) );
                    ui->WarpSetExit->setEnabled( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) );

                    ui->WarpLevelFile->setEnabled(true);

                    ui->WarpLevelFile->setText( door.lname );

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

            ui->WarpLevelExit->setEnabled(false);
            ui->WarpLevelExit->setChecked(false);
            ui->WarpLevelEntrance->setEnabled(false);
            ui->WarpLevelEntrance->setChecked(false);
            ui->WarpLevelFile->setEnabled(false);
            ui->WarpLevelFile->setText("");
        }
    }
}

void MainWindow::on_WarpList_currentIndexChanged(int index)
{
    setDoorData(index);
}




void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    int SectionId = SctId;
    int WinType = activeChildWindow();

    WriteToLog(QtDebugMsg, "Set Current Section");
    if ((open==1)&&(WinType==1)) // Only Set Checked section number without section select
    {
        WriteToLog(QtDebugMsg, "get Current Section");
        SectionId = activeLvlEditWin()->LvlData.CurSection;
    }

    WriteToLog(QtDebugMsg, "Set checkbox to");
    ui->actionSection_1->setChecked( (SectionId==0) );
    ui->actionSection_2->setChecked( (SectionId==1) );
    ui->actionSection_3->setChecked( (SectionId==2) );
    ui->actionSection_4->setChecked( (SectionId==3) );
    ui->actionSection_5->setChecked( (SectionId==4) );
    ui->actionSection_6->setChecked( (SectionId==5) );
    ui->actionSection_7->setChecked( (SectionId==6) );
    ui->actionSection_8->setChecked( (SectionId==7) );
    ui->actionSection_9->setChecked( (SectionId==8) );
    ui->actionSection_10->setChecked( (SectionId==9) );
    ui->actionSection_11->setChecked( (SectionId==10) );
    ui->actionSection_12->setChecked( (SectionId==11) );
    ui->actionSection_13->setChecked( (SectionId==12) );
    ui->actionSection_14->setChecked( (SectionId==13) );
    ui->actionSection_15->setChecked( (SectionId==14) );
    ui->actionSection_16->setChecked( (SectionId==15) );
    ui->actionSection_17->setChecked( (SectionId==16) );
    ui->actionSection_18->setChecked( (SectionId==17) );
    ui->actionSection_19->setChecked( (SectionId==18) );
    ui->actionSection_20->setChecked( (SectionId==19) );
    ui->actionSection_21->setChecked( (SectionId==20) );

    if ((WinType==1) && (open==0))
    {
       WriteToLog(QtDebugMsg, "Call to setCurrentSection()");
       activeLvlEditWin()->setCurrentSection(SectionId);
    }

    if(WinType==1)
    {
        WriteToLog(QtDebugMsg, "Set Section Data in menu");
        //Set Section Data in menu
        ui->actionLevNoBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->actionLevOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->actionLevUnderW->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->actionLevWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text label");
        //set data in Section Settings Widget
        ui->LVLProp_CurSect->setText(QString::number(SectionId+1));

        WriteToLog(QtDebugMsg, "Set ToolBar data");
        ui->LVLPropsNoTBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->LVLPropsOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->LVLPropsUnderWater->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->LVLPropsLevelWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text to custom music file");
        ui->LVLPropsMusicCustom->setText(activeLvlEditWin()->LvlData.sections[SectionId].music_file);

        WriteToLog(QtDebugMsg, "Set standart Music index");
        ui->LVLPropsMusicNumber->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].music_id );

        WriteToLog(QtDebugMsg, "Set Custom music checkbox");
        ui->LVLPropsMusicCustomEn->setChecked( (activeLvlEditWin()->LvlData.sections[SectionId].music_id == configs.music_custom_id) );

        WriteToLog(QtDebugMsg, "Set background index");
        if(activeLvlEditWin()->LvlData.sections[SectionId].background < (unsigned int)ui->LVLPropsBackImage->count() )
            ui->LVLPropsBackImage->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].background );
        else
            ui->LVLPropsBackImage->setCurrentIndex( ui->LVLPropsBackImage->count()-1 );
    }
}




void MainWindow::on_LVLPropsBackImage_currentIndexChanged(int index)
{
    if(configs.main_bg.size()==0)
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for backgrounds not loaded"));
        return;
    }

    if(ui->LVLPropsBackImage->hasFocus())
    {
        ui->LVLPropsBackImage->setEnabled(false);
        WriteToLog(QtDebugMsg, "Change BG to "+QString::number(index));
        if (activeChildWindow()==1)
        {
           activeLvlEditWin()->scene->ChangeSectionBG(index);
           activeLvlEditWin()->LvlData.modified = true;
        }
        ui->LVLPropsBackImage->setEnabled(true);
    }
    else
    {
        if (activeChildWindow()==1)
        {
           ui->LVLPropsBackImage->setCurrentIndex(
                       activeLvlEditWin()->LvlData.sections[
                       activeLvlEditWin()->LvlData.CurSection].background);
        }
    }
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






// Level Section Settings
void MainWindow::on_LVLPropsLevelWarp_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevWarp_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsLevelWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsOffScr_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevOffScr_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}



void MainWindow::on_LVLPropsNoTBack_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevNoBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevNoBack_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsNoTBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsUnderWater_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevUnderW->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevUnderW_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsUnderWater->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}




///////////////////////////////////////Resize section/////////////////////////////////////
void MainWindow::on_ResizeSection_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->setFocus();
        activeLvlEditWin()->scene->setSectionResizer(true);
        ui->ResizeSection->setVisible(false);
        ui->applyResize->setVisible(true);
        ui->cancelResize->setVisible(true);
    }
}

void MainWindow::on_applyResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, true);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
    }
}

void MainWindow::on_cancelResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, false);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
    }
}
