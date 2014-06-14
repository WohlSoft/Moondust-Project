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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"
#include "../music_player.h"

static bool lockSctSettingsProps=false;

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
        if(activeLvlEditWin()->scene->pResizer==NULL)
        {
            activeLvlEditWin()->scene->setSectionResizer(true);
            ui->ResizeSection->setVisible(false);
            ui->applyResize->setVisible(true);
            ui->cancelResize->setVisible(true);
        }
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
    if(lockSctSettingsProps) return;
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
    lockSctSettingsProps=true;

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

    lockSctSettingsProps=false;
}


void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    unsigned int test = index;
    if(ui->LVLPropsMusicNumber->hasFocus())
    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
    }

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) activeLvlEditWin()->LvlData.modified = true;
    }

    WriteToLog(QtDebugMsg, "Call to Set Music if playing");
    setMusic(ui->actionPlayMusic->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(lockSctSettingsProps) return;

    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                activeLvlEditWin()->LvlData.modified = true;
            }
        }
    }
}

void MainWindow::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    MusicFileList musicList(dirPath);
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText(musicList.SelectedFile);
    }

}

void MainWindow::on_LVLPropsMusicCustom_textChanged(const QString &arg1)
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1.simplified().remove('\"');
    }

    setMusic( ui->actionPlayMusic->isChecked() );
}
