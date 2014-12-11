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
#include <mainwindow.h>

void MainWindow::on_actionAlign_selected_triggered()
{

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->applyGridToEach(
                    activeLvlEditWin()->scene->selectedItems()   );
    }
    else
    if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->applyGridToEach(
                    activeWldEditWin()->scene->selectedItems()   );
    }

}

void MainWindow::on_actionRotateLeft_triggered()
{

}

void MainWindow::on_actionRotateRight_triggered()
{

}

void MainWindow::on_actionFlipHorizontal_triggered()
{

}

void MainWindow::on_actionFlipVertical_triggered()
{

}

