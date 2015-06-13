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

#include <common_features/util.h>
#include <tools/tilesets/tilesetconfiguredialog.h>
#include <tools/tilesets/tilesetgroupeditor.h>
#include <tools/external_tools/lazyfixtool_gui.h>
#include <tools/external_tools/gifs2png_gui.h>
#include <tools/external_tools/png2gifs_gui.h>
#include <tools/external_tools/audiocvt_sox_gui.h>
#include <editing/_dialogs/npcdialog.h>
#include <dev_console/devconsole.h>

#include <main_window/dock/lvl_sctc_props.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetConfigureDialog* tilesetConfDia = new TilesetConfigureDialog(&configs, NULL);
    util::DialogToCenter(tilesetConfDia);
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

    util::DialogToCenter(groupDialog);
    groupDialog->exec();
    delete groupDialog;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionShow_Development_Console_triggered()
{
    DevConsole::show();
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
    util::DialogToCenter(lazyfixGUI, true);
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
    util::DialogToCenter(gifToPngGUI, true);
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
    util::DialogToCenter(pngToGifGUI, true);
    pngToGifGUI->show();
}



void MainWindow::on_actionAudioCvt_triggered()
{
    AudioCvt_Sox_gui sox_cvt(this);
    util::DialogToCenter(&sox_cvt, true);
    sox_cvt.exec();

    if(activeChildWindow()==1)
        dock_LvlSectionProps->setLevelSectionData();
}




void MainWindow::on_actionSprite_editor_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}













