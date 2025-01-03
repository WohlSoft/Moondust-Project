/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

void MainWindow::on_actionExport_to_image_triggered()
{
    on_actionSelect_triggered();

    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->ExportToImage_fn_piece();
    }
    else if(activeChildWindow() == WND_World)
    {
        activeWldEditWin()->ExportToImage_fn();
    }
}

void MainWindow::on_actionExport_to_image_section_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        activeLvlEditWin()->ExportToImage_fn();
    }
}


