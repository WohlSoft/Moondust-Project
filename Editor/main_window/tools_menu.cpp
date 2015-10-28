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
#include <dev_console/devconsole.h>

#include <main_window/dock/lvl_sctc_props.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetConfigureDialog* tilesetConfDia = new TilesetConfigureDialog(&configs, NULL, this);
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

LazyFixTool_gui * lazyfixGUI = nullptr;
gifs2png_gui * gifToPngGUI = nullptr;
png2gifs_gui * pngToGifGUI = nullptr;

template<typename T>
void defConstructObjAndExec(T*& dialogObj, QWidget* parent = 0){
    static_assert(std::is_base_of<QDialog, T>::value, "dialogObj must be base of QDialog!");
    if(!dialogObj){
        dialogObj = new T(parent);
    }
    util::DialogToCenter(dialogObj, true);
    dialogObj->show();
    dialogObj->raise();
    dialogObj->setFocus();
}

void MainWindow::on_actionLazyFixTool_triggered()
{    
    defConstructObjAndExec(lazyfixGUI, this);
}

void MainWindow::on_actionGIFs2PNG_triggered()
{
    defConstructObjAndExec(gifToPngGUI, this);
}

void MainWindow::on_actionPNG2GIFs_triggered()
{
    defConstructObjAndExec(pngToGifGUI, this);
}



void MainWindow::on_actionAudioCvt_triggered()
{
    AudioCvt_Sox_gui sox_cvt(this);
    util::DialogToCenter(&sox_cvt, true);
    sox_cvt.exec();

    if(activeChildWindow()==1)
        dock_LvlSectionProps->refreshFileData();
}




void MainWindow::on_actionSprite_editor_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}













