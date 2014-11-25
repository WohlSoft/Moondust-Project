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

#include "../../mainwindow.h"
#include <ui_mainwindow.h>



void MainWindow::on_debuggerBox_visibilityChanged(bool visible)
{
    ui->actionDebugger->setChecked(visible);
}

void MainWindow::on_actionDebugger_triggered(bool checked)
{
    ui->debuggerBox->setVisible(checked);
    if(checked) ui->debuggerBox->raise();
}

void MainWindow::Debugger_UpdateMousePosition(QPoint p, bool isOffScreen)
{
    if(!ui->actionDebugger->isVisible()) return; //SpeedUp

    if(isOffScreen)
    {
        ui->DEBUG_MouseX->setText("");
        ui->DEBUG_MouseY->setText("");
    }
    else
    {
        ui->DEBUG_MouseX->setText(QString::number(p.x()));
        ui->DEBUG_MouseY->setText(QString::number(p.y()));
    }
}

void MainWindow::Debugger_UpdateItemList(QString list)
{
    if(!ui->actionDebugger->isVisible()) return; //SpeedUp

    ui->DEBUG_Items->setText(list);
}


void MainWindow::on_DEBUG_GotoPoint_clicked()
{
    if(ui->DEBUG_GotoX->text().isEmpty()) return;
    if(ui->DEBUG_GotoY->text().isEmpty()) return;

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);

    }

    ui->DEBUG_GotoX->setText("");
    ui->DEBUG_GotoY->setText("");
}
