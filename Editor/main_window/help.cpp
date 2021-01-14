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

#include <QDesktopServices>
#include <common_features/app_path.h>
#include <common_features/util.h>
#include <common_features/themes.h>
#include <main_window/about_dialog/aboutdialog.h>
#include <main_window/updater/check_updates.h>
#include <main_window/tip_of_day/tip_of_day.h>
#include <main_window/greeting_dialog/greeting_dialog.h>
#include <main_window/global_settings.h>

#include <mainwindow.h>
#include <ui_mainwindow.h>

#include "dock/lvl_item_toolbox.h"
#include "dock/wld_item_toolbox.h"
#include "dock/tileset_item_box.h"


void MainWindow::on_actionContents_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( ApplicationPath + "/help/manual_editor.html" ) );
}

void MainWindow::showWelcomeDialog()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    setup.beginGroup("message-boxes");
    bool showNotice = setup.value("uidesign-editor-greeting", true).toBool();
    setup.endGroup();
    if(showNotice)
    {
        on_actionWelcome_triggered();
    }
}

void MainWindow::on_actionWelcome_triggered()
{
    GreetingDialog grtn(this);
    grtn.connect(&grtn, &GreetingDialog::switchClassic, this, &MainWindow::on_actionSMBX_like_GUI_triggered);
    grtn.connect(&grtn, &GreetingDialog::switchModern, this, &MainWindow::on_actionModern_GUI_triggered);
    grtn.exec();
    grtn.disconnect(&grtn, &GreetingDialog::switchClassic, this, &MainWindow::on_actionSMBX_like_GUI_triggered);
    grtn.disconnect(&grtn, &GreetingDialog::switchModern, this, &MainWindow::on_actionModern_GUI_triggered);
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

    int win = activeChildWindow();
    if(win == WND_Level)
        on_actionLVLToolBox_triggered(false);
    if(win == WND_World)
        on_actionWLDToolBox_triggered(false);
    if(((win == WND_Level) || (win == WND_World)) && configs.editor.default_visibility.tilesets_box)
        on_actionTilesetBox_triggered(true);

    dock_LvlItemBox->m_lastVisibilityState = false;
    dock_WldItemBox->m_lastVisibilityState = false;
    if(configs.editor.default_visibility.tilesets_box)
        dock_TilesetBox->m_lastVisibilityState = true;
    m_toolbarVanilla->setVisible(true);
}


void MainWindow::on_actionModern_GUI_triggered()
{
    setTabView();

    int win = activeChildWindow();
    if((win == WND_Level) && configs.editor.default_visibility.lvl_itembox)
        on_actionLVLToolBox_triggered(true);
    if((win == WND_World) && configs.editor.default_visibility.wld_itembox)
        on_actionWLDToolBox_triggered(true);
    //if((win==WND_Level) || (win==WND_World))
    //    on_actionTilesetBox_triggered(false);

    if(configs.editor.default_visibility.lvl_itembox)
        dock_LvlItemBox->m_lastVisibilityState = true;
    if(configs.editor.default_visibility.wld_itembox)
        dock_WldItemBox->m_lastVisibilityState = true;
    //dock_TilesetBox->m_lastVisibilityState = false;
    m_toolbarVanilla->setVisible(false);
}


void MainWindow::on_actionChange_log_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( ApplicationPath + "/changelog.editor.txt" ) );
}



void MainWindow::on_actionCheckUpdates_triggered()
{
    UpdateChecker updater(this);
    util::DialogToCenter(&updater, true);
    updater.exec();
}



//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about(this);
    util::DialogToCenter(&about, true);
    about.exec();
}
