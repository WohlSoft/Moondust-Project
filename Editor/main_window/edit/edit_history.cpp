/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <mainwindow.h>

void MainWindow::refreshHistoryButtons()
{
    if(activeChildWindow()==1)
    {
        if(activeLvlEditWin()->sceneCreated)
        {
            ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
            ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
        }
    }else if(activeChildWindow()==3){
        if(activeWldEditWin()->sceneCreated)
        {
            ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
            ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
        }
    }

}

// //History Manager
void MainWindow::on_actionUndo_triggered()
{
    ui->ItemProperties->hide();
    ui->WLD_ItemProps->hide();
    if (activeChildWindow()==1)
    {
        //Here must be call
        activeLvlEditWin()->scene->historyBack();
        ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->historyBack();
        ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
    }
}


void MainWindow::on_actionRedo_triggered()
{
    ui->ItemProperties->hide();
    ui->WLD_ItemProps->hide();
    if (activeChildWindow()==1)
    {
        //Here must be call
        activeLvlEditWin()->scene->historyForward();
        ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->historyForward();
        ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
    }
}

