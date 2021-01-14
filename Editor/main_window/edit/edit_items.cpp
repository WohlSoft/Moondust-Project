/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef Q_OS_MACX
#undef Q_OS_MACX//Workaround for AutoMOC, avoiding of re-definiton warnings
#endif

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionAlign_selected_triggered()
{

    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->scene->applyGridToEach(
            activeLvlEditWin()->scene->selectedItems());
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->scene->applyGridToEach(
            activeWldEditWin()->scene->selectedItems());
    }

}

void MainWindow::on_actionRotateLeft_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->scene->rotateGroup(
            activeLvlEditWin()->scene->selectedItems(), false);
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->scene->rotateGroup(
            activeWldEditWin()->scene->selectedItems(), false);
    }

}

void MainWindow::on_actionRotateRight_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->scene->rotateGroup(
            activeLvlEditWin()->scene->selectedItems(), true);
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->scene->rotateGroup(
            activeWldEditWin()->scene->selectedItems(), true);
    }
}

void MainWindow::on_actionFlipHorizontal_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->scene->flipGroup(
            activeLvlEditWin()->scene->selectedItems(), false);
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->scene->flipGroup(
            activeWldEditWin()->scene->selectedItems(), false);
    }

}

void MainWindow::on_actionFlipVertical_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->scene->flipGroup(
            activeLvlEditWin()->scene->selectedItems(), true);
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->scene->flipGroup(
            activeWldEditWin()->scene->selectedItems(), true);
    }
}

