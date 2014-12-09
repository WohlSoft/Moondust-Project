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

#include "../tools/tilesets/tilesetconfiguredialog.h"
#include "../tools/tilesets/tilesetgroupeditor.h"
#include "../editing/_dialogs/npcdialog.h"
#include <QDesktopServices>
#include "../dev_console/devconsole.h"

#include "../tools/external_tools/lazyfixtool_gui.h"
#include "../tools/external_tools/gifs2png_gui.h"
#include "../tools/external_tools/png2gifs_gui.h"


void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetConfigureDialog* tilesetConfDia = new TilesetConfigureDialog(&configs, NULL);
    tilesetConfDia->exec();
    delete tilesetConfDia;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionTileset_groups_editor_triggered()
{
    TilesetGroupEditor * groupDialog;
    if(activeChildWindow()==1)
        groupDialog = new TilesetGroupEditor(activeLvlEditWin()->scene, this);
    else if(activeChildWindow()==3)
        groupDialog = new TilesetGroupEditor(activeWldEditWin()->scene, this);
    else
        groupDialog = new TilesetGroupEditor(NULL, this);
    groupDialog->exec();
    delete groupDialog;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionShow_Development_Console_triggered()
{
    DevConsole::show();
    DevConsole::log("Showing DevConsole!","View");
}

LazyFixTool_gui * lazyfixGUI;
void MainWindow::on_actionLazyFixTool_triggered()
{
    if(lazyfixGUI)
    {
        lazyfixGUI->show();
        lazyfixGUI->raise();
        lazyfixGUI->setFocus();
        return;
    }
    lazyfixGUI = new LazyFixTool_gui;
    lazyfixGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    lazyfixGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, lazyfixGUI->size(), qApp->desktop()->availableGeometry()));
    lazyfixGUI->show();
}

gifs2png_gui * gifToPngGUI;
void MainWindow::on_actionGIFs2PNG_triggered()
{
    if(gifToPngGUI)
    {
        gifToPngGUI->show();
        gifToPngGUI->raise();
        gifToPngGUI->setFocus();
        return;
    }
    gifToPngGUI = new gifs2png_gui;
    gifToPngGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    gifToPngGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, gifToPngGUI->size(), qApp->desktop()->availableGeometry()));
    gifToPngGUI->show();
}

png2gifs_gui * pngToGifGUI;
void MainWindow::on_actionPNG2GIFs_triggered()
{
    if(pngToGifGUI)
    {
        pngToGifGUI->show();
        pngToGifGUI->raise();
        pngToGifGUI->setFocus();
        return;
    }
    pngToGifGUI = new png2gifs_gui;
    pngToGifGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    pngToGifGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pngToGifGUI->size(), qApp->desktop()->availableGeometry()));
    pngToGifGUI->show();
}



void MainWindow::on_actionSprite_editor_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}













