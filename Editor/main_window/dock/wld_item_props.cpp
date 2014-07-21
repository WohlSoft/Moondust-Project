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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"
#include "../../world_scene/wld_item_placing.h"
#include "../../item_select_dialog/itemselectdialog.h"

#include "../../world_scene/item_level.h"
#include "../../common_features/levelfilelist.h"

static bool wld_tools_lock=false;


void MainWindow::WldItemProps(int Type, WorldLevels level, bool newItem)
{

}


void MainWindow::WldLvlExitTypeListReset()
{
    QStringList LevelExitType;
    int backup_index;

    LevelExitType.push_back(tr("* - Any"));
    LevelExitType.push_back(tr("0 - None"));
    LevelExitType.push_back(tr("1 - SMB3 Standard Exit"));
    LevelExitType.push_back(tr("2 - SMB3 Boss Exit"));
    LevelExitType.push_back(tr("3 - Walked Offscreen"));
    LevelExitType.push_back(tr("4 - Secret Exit"));
    LevelExitType.push_back(tr("5 - SMB2 Boss Exit"));
    LevelExitType.push_back(tr("6 - Warp Exit"));
    LevelExitType.push_back(tr("7 - Star Exit"));
    LevelExitType.push_back(tr("8 - SMW Exit"));

    wld_tools_lock=true;

    backup_index = ui->WLD_PROPS_ExitTop->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitTop->clear();
    ui->WLD_PROPS_ExitTop->addItems(LevelExitType);
    ui->WLD_PROPS_ExitTop->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitLeft->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitLeft->clear();
    ui->WLD_PROPS_ExitLeft->addItems(LevelExitType);
    ui->WLD_PROPS_ExitLeft->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitRight->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitRight->clear();
    ui->WLD_PROPS_ExitRight->addItems(LevelExitType);
    ui->WLD_PROPS_ExitRight->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitBottom->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitBottom->clear();
    ui->WLD_PROPS_ExitBottom->addItems(LevelExitType);
    ui->WLD_PROPS_ExitBottom->setCurrentIndex(backup_index);

    wld_tools_lock=false;

}


//ITemProps
void MainWindow::on_WLD_PROPS_PathBG_clicked(bool checked)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_BigPathBG_clicked(bool checked)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_AlwaysVis_clicked(bool checked)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_GameStart_clicked(bool checked)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_LVLFile_textEdited(const QString &arg1)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_LVLTitle_textEdited(const QString &arg1)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_EnterTo_valueChanged(int arg1)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_LVLBrowse_clicked()
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_ExitTop_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_ExitLeft_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_ExitRight_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_ExitBottom_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_GotoX_textEdited(const QString &arg1)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_GotoY_textEdited(const QString &arg1)
{
    if(wld_tools_lock) return;

}

void MainWindow::on_WLD_PROPS_GetPoint_clicked()
{
    if(wld_tools_lock) return;

}
