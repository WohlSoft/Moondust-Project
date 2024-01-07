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

#include <editing/_scenes/world/wld_item_placing.h>
#include <editing/_scenes/level/lvl_item_placing.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionRectFill_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);
    ui->actionOverwriteMode->setVisible(true);
    ui->actionRectFill->setVisible(true);
    ui->actionCircleFill->setVisible(true);
    ui->actionLine->setVisible(true);
    ui->actionFill->setVisible(true);
    ui->actionFloodSectionOnly->setVisible(true);
    ui->actionFloodSectionOnly->setEnabled(false);

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_Rect
                                        : LvlPlacingItems::PMODE_Brush);

        ui->actionLine->setChecked(false);
        ui->actionFill->setChecked(false);
        ui->actionCircleFill->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case LvlScene::PLC_Block:
            edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id);
            LogDebug(QString("Block Square draw -> %1").arg(checked));

            break;
        case LvlScene::PLC_BGO:
            edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id);
            LogDebug(QString("BGO Square draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_Rect
                                        : WldPlacingItems::PMODE_Brush);

        ui->actionLine->setChecked(false);
        ui->actionFill->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case WldScene::PLC_Terrain:
            edit->scene->setItemPlacer(0, WldPlacingItems::terrainSet.id);
            LogDebug(QString("Tile Square draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Scene:
            edit->scene->setItemPlacer(1, WldPlacingItems::sceneSet.id);
            LogDebug(QString("Scenery Square draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Path:
            edit->scene->setItemPlacer(2, WldPlacingItems::pathSet.id);
            LogDebug(QString("Path Square draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Level:
            edit->scene->setItemPlacer(3, WldPlacingItems::levelSet.id);
            LogDebug(QString("Path Square draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
}


void MainWindow::on_actionCircleFill_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);
    ui->actionOverwriteMode->setVisible(true);
    ui->actionRectFill->setVisible(true);
    ui->actionCircleFill->setVisible(true);
    ui->actionLine->setVisible(true);
    ui->actionFill->setVisible(true);
    ui->actionFloodSectionOnly->setVisible(true);
    ui->actionFloodSectionOnly->setEnabled(false);

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_Circle
                                        : LvlPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionLine->setChecked(false);
        ui->actionFill->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case LvlScene::PLC_Block:
            edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id);
            LogDebug(QString("Block Circle draw -> %1").arg(checked));

            break;
        case LvlScene::PLC_BGO:
            edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id);
            LogDebug(QString("BGO Circle draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_Circle
                                        : WldPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionLine->setChecked(false);
        ui->actionFill->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case WldScene::PLC_Terrain:
            edit->scene->setItemPlacer(0, WldPlacingItems::terrainSet.id);
            LogDebug(QString("Tile Circle draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Scene:
            edit->scene->setItemPlacer(1, WldPlacingItems::sceneSet.id);
            LogDebug(QString("Scenery Circle draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Path:
            edit->scene->setItemPlacer(2, WldPlacingItems::pathSet.id);
            LogDebug(QString("Path Circle draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Level:
            edit->scene->setItemPlacer(3, WldPlacingItems::levelSet.id);
            LogDebug(QString("Path Circle draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
}


void MainWindow::on_actionLine_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);
    ui->actionOverwriteMode->setVisible(true);
    ui->actionRectFill->setVisible(true);
    ui->actionCircleFill->setVisible(true);
    ui->actionLine->setVisible(true);
    ui->actionFill->setVisible(true);
    ui->actionFloodSectionOnly->setVisible(true);
    ui->actionFloodSectionOnly->setEnabled(false);

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_Line
                                        : LvlPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionCircleFill->setChecked(false);
        ui->actionFill->setChecked(false);


        switch(edit->scene->m_placingItemType)
        {
        case LvlScene::PLC_Block:
            edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id);
            LogDebug(QString("Block Line draw -> %1").arg(checked));

            break;
        case LvlScene::PLC_BGO:
            edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id);
            LogDebug(QString("BGO Line draw -> %1").arg(checked));

            break;
        case LvlScene::PLC_NPC:
            edit->scene->setItemPlacer(2, LvlPlacingItems::npcSet.id);
            LogDebug(QString("NPC Line draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_Line
                                        : WldPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionCircleFill->setChecked(false);
        ui->actionFill->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case WldScene::PLC_Terrain:
            edit->scene->setItemPlacer(0, WldPlacingItems::terrainSet.id);
            LogDebug(QString("Tile Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Scene:
            edit->scene->setItemPlacer(1, WldPlacingItems::sceneSet.id);
            LogDebug(QString("Scenery Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Path:
            edit->scene->setItemPlacer(2, WldPlacingItems::pathSet.id);
            LogDebug(QString("Path Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Level:
            edit->scene->setItemPlacer(3, WldPlacingItems::levelSet.id);
            LogDebug(QString("Path Line draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
}



void MainWindow::on_actionFill_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);
    ui->actionOverwriteMode->setVisible(true);
    ui->actionRectFill->setVisible(true);
    ui->actionCircleFill->setVisible(true);
    ui->actionLine->setVisible(true);
    ui->actionFill->setVisible(true);
    ui->actionFloodSectionOnly->setVisible(true);
    ui->actionFloodSectionOnly->setEnabled(checked);
    ui->actionFloodSectionOnly->setChecked(LvlPlacingItems::noOutSectionFlood);

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_FloodFill
                                        : LvlPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionCircleFill->setChecked(false);
        ui->actionLine->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case LvlScene::PLC_Block:
            edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id);
            break;
        case LvlScene::PLC_BGO:
            edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id);
            break;
        default:
            break;
        }
        edit->setFocus();
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_FloodFill
                                        : WldPlacingItems::PMODE_Brush);

        ui->actionRectFill->setChecked(false);
        ui->actionCircleFill->setChecked(false);
        ui->actionLine->setChecked(false);

        switch(edit->scene->m_placingItemType)
        {
        case WldScene::PLC_Terrain:
            edit->scene->setItemPlacer(0, WldPlacingItems::terrainSet.id);
            LogDebug(QString("Tile Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Scene:
            edit->scene->setItemPlacer(1, WldPlacingItems::sceneSet.id);
            LogDebug(QString("Scenery Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Path:
            edit->scene->setItemPlacer(2, WldPlacingItems::pathSet.id);
            LogDebug(QString("Path Line draw -> %1").arg(checked));

            break;
        case WldScene::PLC_Level:
            edit->scene->setItemPlacer(3, WldPlacingItems::levelSet.id);
            LogDebug(QString("Path Line draw -> %1").arg(checked));

            break;

        default:
            break;
        }
        edit->setFocus();
    }
}


void MainWindow::on_actionFloodSectionOnly_triggered(bool checked)
{
    LvlPlacingItems::noOutSectionFlood = checked;
}

void MainWindow::on_actionOverwriteMode_triggered(bool checked)
{
    LvlPlacingItems::overwriteMode = checked;
    WldPlacingItems::overwriteMode = checked;
}

