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

#include "ui_mainwindow.h"
#include "../mainwindow.h"

#include "../tilesets/tilesetconfiguredialog.h"
#include "../tilesets/tilesetgroupeditor.h"
#include "../npc_dialog/npcdialog.h"
#include <QDesktopServices>
#include "../dev_console/devconsole.h"

#include "../external_tools/lazyfixtool_gui.h"
#include "../external_tools/gifs2png_gui.h"

void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetConfigureDialog* tilesetConfDia = new TilesetConfigureDialog(&configs);
    tilesetConfDia->exec();
    delete tilesetConfDia;
}


void MainWindow::on_actionTileset_groups_editor_triggered()
{
    TilesetGroupEditor * groupDialog = new TilesetGroupEditor;
    groupDialog->exec();
    delete groupDialog;
}


void MainWindow::on_actionShow_Development_Console_triggered()
{
    DevConsole::show();
    DevConsole::log("Showing DevConsole!","View");
}


void MainWindow::on_actionLazyFixTool_triggered()
{
    LazyFixTool_gui * lazyfixGUI = new LazyFixTool_gui;
    lazyfixGUI->exec();
    delete lazyfixGUI;
}

void MainWindow::on_actionGIFs2PNG_triggered()
{
    gifs2png_gui * gifToPngGUI = new gifs2png_gui;
    gifToPngGUI->exec();
    delete gifToPngGUI;

}

void MainWindow::on_actionPNG2GIFs_triggered()
{

}

