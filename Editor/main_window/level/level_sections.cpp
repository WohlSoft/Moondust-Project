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
#include "mainwindow.h"

#include "../../editing/_scenes/level/lvl_scene.h"
#include "../dock/lvl_sctc_props.h"

void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    lockSctSettingsProps=true;

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

        setMusic( ui->actionPlayMusic->isChecked() );
    }

    lockSctSettingsProps=false;
}


void MainWindow::on_actionReset_position_triggered()
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->ResetPosition();
    }
    else
    if (activeChildWindow()==3)
    {
       activeWldEditWin()->ResetPosition();
    }
}

void MainWindow::on_actionGo_to_Section_triggered()
{
    qApp->setActiveWindow(this);
    int WinType = activeChildWindow();
    if(WinType==1)
    {
            LevelEdit* edit = activeLvlEditWin();
            int SectionId = edit->LvlData.CurSection;
            int xb = edit->LvlData.sections[SectionId].size_left;
            int yb = edit->LvlData.sections[SectionId].size_top;
            edit->goTo(xb, yb, false, QPoint(-10, -10));
    }
}



bool MainWindow::getCurrentSceneCoordinates(qreal &x, qreal &y)
{
    if(activeChildWindow() == 1)
    {
        LevelEdit* edit = activeLvlEditWin();
        QPointF coor = edit->getGraphicsView()->mapToScene(0,0);
        x = coor.x();
        y = coor.y();
        return true;
    }
    else if(activeChildWindow() == 3)
    {
        WorldEdit* edit = activeWldEditWin();
        QPointF coor = edit->getGraphicsView()->mapToScene(0,0);
        x = coor.x();
        y = coor.y();
        return true;
    }
    return false;
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



