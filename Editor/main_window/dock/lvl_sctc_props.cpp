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


// ////////////////////////////////////////////////////////////////////////////////


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



// ////////////////Set LevelSection data//////////////////////////////////

void MainWindow::setLevelSectionData()
{
    int i;
        WriteToLog(QtDebugMsg, QString("Set level Section Data"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLPropsBackImage->addItem( tr("[No image]") );
    ui->LVLPropsMusicNumber->addItem( tr("[Silence]") );

    for(i=0; i< configs.main_bg.size();i++)
        ui->LVLPropsBackImage->addItem(configs.main_bg[i].name);

    for(i=0; i< configs.main_music_lvl.size();i++)
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name);

}
