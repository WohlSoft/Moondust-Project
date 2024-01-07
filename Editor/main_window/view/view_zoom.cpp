/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QLineEdit>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionZoomIn_triggered()
{
    if(activeChildWindow() == WND_Level)
        activeLvlEditWin()->zoomIn();
    else if(activeChildWindow() == WND_World)
        activeWldEditWin()->zoomIn();

}

void MainWindow::on_actionZoomOut_triggered()
{
    if(activeChildWindow() == WND_Level)
        activeLvlEditWin()->zoomOut();
    else if(activeChildWindow() == WND_World)
        activeWldEditWin()->zoomOut();
}

void MainWindow::on_actionZoomReset_triggered()
{
    if(activeChildWindow() == WND_Level)
        activeLvlEditWin()->zoomReset();
    else if(activeChildWindow() == WND_World)
        activeWldEditWin()->ResetZoom();
}


void MainWindow::applyTextZoom()
{
    bool ok = false;
    int zoomPercent = 100;
    zoomPercent = zoom->text().toInt(&ok);
    if(!ok)
        return;

    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->setZoom(zoomPercent);
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->setZoom(zoomPercent);
    }
}

