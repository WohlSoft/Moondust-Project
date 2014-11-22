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




// World tool box show/hide
void MainWindow::on_WorldToolBox_visibilityChanged(bool visible)
{
        ui->actionWLDToolBox->setChecked(visible);
}

void MainWindow::on_actionWLDToolBox_triggered(bool checked)
{
    ui->WorldToolBox->setVisible(checked);
    if(checked) ui->WorldToolBox->raise();
}


void MainWindow::on_WorldSettings_visibilityChanged(bool visible)
{
    ui->actionWorld_settings->setChecked(visible);
}


void MainWindow::on_actionWorld_settings_triggered(bool checked)
{
    ui->WorldSettings->setVisible(checked);
    if(checked) ui->WorldSettings->raise();
}


// //////////////////////// Locks Begin //////////////////////////////

void MainWindow::on_actionLockTiles_triggered(bool checked)
{
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->setLocked(1, checked);
    }

}

void MainWindow::on_actionLockScenes_triggered(bool checked)
{
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->setLocked(2, checked);
    }
}

void MainWindow::on_actionLockPaths_triggered(bool checked)
{
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->setLocked(3, checked);
    }
}

void MainWindow::on_actionLockLevels_triggered(bool checked)
{
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->setLocked(4, checked);
    }
}

void MainWindow::on_actionLockMusicBoxes_triggered(bool checked)
{
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->setLocked(5, checked);
    }
}



void MainWindow::on_actionSemi_transparent_paths_triggered(bool checked)
{
    GlobalSettings::LvlOpts.semiTransparentPaths = checked;
    if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.semiTransparentPaths = GlobalSettings::LvlOpts.semiTransparentPaths;
        activeWldEditWin()->scene->setSemiTransparentPaths(GlobalSettings::LvlOpts.semiTransparentPaths);
    }

}


// //////////////////////// Locks End //////////////////////////////
