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
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/wld_item_props.h>

/////////////////Switch edit mode////////////////////////////////////////////

void MainWindow::resetEditmodeButtons()
{
    ui->actionSelect->setChecked(0);
    ui->actionSelectOnly->setChecked(0);
    ui->actionEriser->setChecked(0);
    ui->actionHandScroll->setChecked(0);

    ui->actionSetFirstPlayer->setChecked(0);
    ui->actionSetSecondPlayer->setChecked(0);
    ui->actionDrawWater->setChecked(0);
    ui->actionDrawSand->setChecked(0);

    ui->PlacingToolbar->setVisible(false);
    ui->actionOverwriteMode->setVisible(false);
    ui->actionRectFill->setVisible(false);
    ui->actionCircleFill->setVisible(false);
    ui->actionLine->setVisible(false);
    ui->actionFill->setVisible(false);
    ui->actionFloodSectionOnly->setVisible(false);

    ui->ResizingToolbar->setVisible(false);

    if(activeChildWindow() == WND_Level)
        activeLvlEditWin()->scene->setLabelBoxItem(false);
    else if(activeChildWindow() == WND_World)
        activeWldEditWin()->scene->setMessageBoxItem(false);

}



void MainWindow::on_actionSelect_triggered()
{
    resetEditmodeButtons();
    ui->actionSelect->setChecked(1);

    dock_LvlItemProps->hide();
    dock_WldItemProps->hide();

    if((activeChildWindow() == WND_Level) && (ui->actionSelect->isChecked()))
    {
        activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Selecting);
    }
    else if((activeChildWindow() == WND_World) && (ui->actionSelect->isChecked()))
    {
        activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Selecting);
    }
}


void MainWindow::on_actionSelectOnly_triggered()
{
    resetEditmodeButtons();
    ui->actionSelectOnly->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level) && (ui->actionSelectOnly->isChecked()))
    {
        activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_SelectingOnly);
    }
    else if((activeChildWindow() == WND_World) && (ui->actionSelectOnly->isChecked()))
    {
        activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_SelectingOnly);
    }
}


void MainWindow::on_actionEriser_triggered()
{
    resetEditmodeButtons();
    ui->actionEriser->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level) && (ui->actionEriser->isChecked()))
    {
        activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Erasing);
    }
    else if((activeChildWindow() == WND_World) && (ui->actionEriser->isChecked()))
    {
        activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Erasing);
    }
}


void MainWindow::on_actionHandScroll_triggered()
{
    resetEditmodeButtons();
    ui->actionHandScroll->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level) && (ui->actionHandScroll->isChecked()))
    {
        activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_HandScroll);
    }
    else if((activeChildWindow() == WND_World) && (ui->actionHandScroll->isChecked()))
    {
        activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_HandScroll);
    }
}
