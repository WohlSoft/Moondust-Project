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

#include <QDesktopServices>
#include <common_features/app_path.h>
#include <common_features/util.h>
#include <main_window/about_dialog/aboutdialog.h>
#include <main_window/updater/check_updates.h>
#include <main_window/tip_of_day/tip_of_day.h>
#include <main_window/global_settings.h>

#include <mainwindow.h>
#include <ui_mainwindow.h>


void MainWindow::on_actionContents_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( ApplicationPath + "/help/manual_editor.html" ) );
}

void MainWindow::showTipOfDay()
{
    if(GlobalSettings::ShowTipOfDay)
        on_actionTipOfDay_triggered();
}

void MainWindow::on_actionTipOfDay_triggered()
{
    TipOfDay tod;
    util::DialogToCenter(&tod, true);
    tod.exec();
}


void MainWindow::on_actionSMBX_like_GUI_triggered()
{
    setSubView();

    if(activeChildWindow()==1)
        on_actionLVLToolBox_triggered(false);
    else
    if(activeChildWindow()==3)
        on_actionWLDToolBox_triggered(false);

    on_actionTilesetBox_triggered(true);
}



void MainWindow::on_actionChange_log_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( ApplicationPath + "/changelog.editor.txt" ) );
}



void MainWindow::on_actionCheckUpdates_triggered()
{
    UpdateChecker updater;
    util::DialogToCenter(&updater, true);
    updater.exec();
}



//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about;
    util::DialogToCenter(&about, true);
    about.exec();
}
