/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <editing/_scenes/level/lvl_item_placing.h>
#include <editing/_scenes/world/wld_item_placing.h>
#include <PGE_File_Formats/file_formats.h>

#include <defines.h>
#include <main_window/global_settings.h>
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/wld_item_props.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <networking/engine_intproc.h>


void MainWindow::on_action_Placing_ShowProperties_triggered(bool checked)
{
    if(activeChildWindow() == WND_Level) // Level editing window
    {
        switch(Placing_ShowProperties_lastType)
        {
        case ItemTypes::LVL_Block:
        case ItemTypes::LVL_BGO:
        case ItemTypes::LVL_NPC:
        {
            if(checked)
            {
                dock_LvlItemProps->show();
                dock_LvlItemProps->raise();
                setFocus(Qt::ActiveWindowFocusReason);
            }
            else
                dock_LvlItemProps->hide();
            break;
        }
        }
    }
    else if(activeChildWindow() == WND_World) // World editing window
    {
        switch(Placing_ShowProperties_lastType)
        {
        case ItemTypes::WLD_Level:
        {
            if(checked)
            {
                dock_WldItemProps->show();
                dock_WldItemProps->raise();
                setFocus(Qt::ActiveWindowFocusReason);
            }
            else
                dock_WldItemProps->hide();
            break;
        }
        }
    }
}

void MainWindow::on_actionSetFirstPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetFirstPlayer->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level))
    {
        activeLvlEditWin()->scene->setItemPlacer(5, 0);
    }

}

void MainWindow::on_actionSetSecondPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetSecondPlayer->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level))
    {
        activeLvlEditWin()->scene->setItemPlacer(5, 1);
    }

}

void MainWindow::on_actionDrawWater_triggered()
{
    resetEditmodeButtons();
    ui->actionDrawWater->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level))
    {
        activeLvlEditWin()->scene->setItemPlacer(3, 0);
    }
}

void MainWindow::on_actionDrawSand_triggered()
{
    resetEditmodeButtons();
    ui->actionDrawSand->setChecked(1);

    dock_LvlItemProps->hide();

    if((activeChildWindow() == WND_Level))
    {
        activeLvlEditWin()->scene->setItemPlacer(3, 1);
    }
}


void MainWindow::SwitchPlacingItem(int itemType, unsigned long itemID, bool dont_reset_props)
{
    if(activeChildWindow() == WND_Level) // Level editing window
    {
        bool valid = false;
        switch(itemType)
        {
        case ItemTypes::LVL_Block:
        case ItemTypes::LVL_BGO:
        case ItemTypes::LVL_NPC:
            valid = true;

            resetEditmodeButtons();
            ui->PlacingToolbar->setVisible(true);
            ui->actionOverwriteMode->setVisible(true);
            ui->actionRectFill->setVisible(true);
            ui->actionCircleFill->setVisible(true);
            ui->actionLine->setVisible(true);
            ui->actionFill->setVisible(true);
            ui->actionFloodSectionOnly->setVisible(true);
            ui->actionFloodSectionOnly->setEnabled(ui->actionFill->isChecked());

            activeLvlEditWin()->scene->clearSelection();
            activeLvlEditWin()->changeCursor(LevelEdit::MODE_PlaceItem);
            activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);

            LvlPlacingItems::placingMode = LvlPlacingItems::PMODE_Brush;

            ui->actionRectFill->setChecked(false);
            ui->actionRectFill->setEnabled(true);

            ui->actionCircleFill->setChecked(false);
            ui->actionCircleFill->setEnabled(true);

            ui->actionLine->setChecked(false);
            ui->actionLine->setEnabled(true);

            ui->actionFill->setChecked(false);
            ui->actionFill->setEnabled(true);

        default:
            ;
        }

        Placing_ShowProperties_lastType = itemType;

        //Switch placing mode
        if(valid)
        {
            switch(itemType)
            {
            case ItemTypes::LVL_Block:
            {
                ui->action_Placing_ShowProperties->setEnabled(true);
                //Switch scene to placing mode:
                activeLvlEditWin()->scene->setItemPlacer(0, itemID);
                //Open block properties toolbox for define placing properties
                dock_LvlItemProps->OpenBlock(LvlPlacingItems::blockSet,
                                             true,
                                             dont_reset_props,
                                             GlobalSettings::Placing_dontShowPropertiesBox);

                if(IntEngine::isWorking())
                {
                    LevelData buffer;
                    FileFormats::CreateLevelData(buffer);
                    buffer.blocks.push_back(LvlPlacingItems::blockSet);
                    buffer.layers.clear();
                    buffer.events.clear();
                    QString encoded;
                    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
                        IntEngine::sendItemPlacing("BLOCK_PLACE\nBLOCK_PLACE_END\n" + encoded);
                }
                break;
            }
            case ItemTypes::LVL_BGO:
            {
                ui->action_Placing_ShowProperties->setEnabled(true);
                activeLvlEditWin()->scene->setItemPlacer(1, itemID);
                dock_LvlItemProps->OpenBGO(LvlPlacingItems::bgoSet,
                                           true,
                                           dont_reset_props,
                                           GlobalSettings::Placing_dontShowPropertiesBox);

                if(IntEngine::isWorking())
                {
                    LevelData buffer;
                    FileFormats::CreateLevelData(buffer);
                    buffer.bgo.push_back(LvlPlacingItems::bgoSet);
                    buffer.layers.clear();
                    buffer.events.clear();
                    QString encoded;
                    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
                        IntEngine::sendItemPlacing("BGO_PLACE\nBGO_PLACE_END\n" + encoded);
                }

                break;
            }
            case ItemTypes::LVL_NPC:
            {
                ui->action_Placing_ShowProperties->setEnabled(true);
                ui->actionRectFill->setEnabled(false);
                ui->actionCircleFill->setEnabled(false);
                ui->actionFill->setEnabled(false);

                activeLvlEditWin()->scene->setItemPlacer(2, itemID);

                dock_LvlItemProps->OpenNPC(LvlPlacingItems::npcSet,
                                           true,
                                           dont_reset_props,
                                           GlobalSettings::Placing_dontShowPropertiesBox);

                if(IntEngine::isWorking())
                {
                    LevelData buffer;
                    FileFormats::CreateLevelData(buffer);
                    buffer.npc.push_back(LvlPlacingItems::npcSet);
                    buffer.layers.clear();
                    buffer.events.clear();
                    QString encoded;
                    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
                        IntEngine::sendItemPlacing("NPC_PLACE\nNPC_PLACE_END\n" + encoded);
                }

                break;
            }
            }
            qApp->setActiveWindow(this);
            raise();
            activeLvlEditWin()->setFocus(Qt::MouseFocusReason);
        }
    }
    else if(activeChildWindow() == WND_World) // World editing window
    {
        bool valid = false;
        ui->action_Placing_ShowProperties->setEnabled(false);
        switch(itemType)
        {
        case ItemTypes::WLD_Tile:
        case ItemTypes::WLD_Scenery:
        case ItemTypes::WLD_Path:
        case ItemTypes::WLD_Level:
        case ItemTypes::WLD_MusicBox:
            valid = true;

            resetEditmodeButtons();

            ui->PlacingToolbar->setVisible(true);
            ui->actionOverwriteMode->setVisible(true);
            ui->actionRectFill->setVisible(true);
            ui->actionCircleFill->setVisible(true);
            ui->actionLine->setVisible(true);
            ui->actionFill->setVisible(true);
            ui->actionFloodSectionOnly->setVisible(true);
            ui->actionFloodSectionOnly->setEnabled(ui->actionFill->isChecked());

            activeWldEditWin()->scene->clearSelection();
            activeWldEditWin()->changeCursor(WorldEdit::MODE_PlaceItem);
            activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_PlacingNew);

            WldPlacingItems::placingMode = WldPlacingItems::PMODE_Brush;

            ui->actionRectFill->setChecked(false);
            ui->actionRectFill->setEnabled(true);

            ui->actionCircleFill->setChecked(false);
            ui->actionCircleFill->setEnabled(true);

            ui->actionLine->setChecked(false);
            ui->actionLine->setEnabled(true);

            ui->actionFill->setChecked(false);
            ui->actionFill->setEnabled(true);

        default:
            ;
        }

        Placing_ShowProperties_lastType = itemType;

        //Switch placing mode
        if(valid)
        {
            switch(itemType)
            {
            case ItemTypes::WLD_Tile:
            {
                activeWldEditWin()->scene->setItemPlacer(0, itemID);
                dock_WldItemProps->WldItemProps(-1, FileFormats::CreateWldLevel(), true);
                break;
            }
            case ItemTypes::WLD_Scenery:
            {
                activeWldEditWin()->scene->setItemPlacer(1, itemID);
                dock_WldItemProps->WldItemProps(-1, FileFormats::CreateWldLevel(), true);
                break;
            }
            case ItemTypes::WLD_Path:
            {
                activeWldEditWin()->scene->setItemPlacer(2, itemID);
                dock_WldItemProps->WldItemProps(-1, FileFormats::CreateWldLevel(), true);
                break;
            }
            case ItemTypes::WLD_Level:
            {
                ui->action_Placing_ShowProperties->setEnabled(true);
                activeWldEditWin()->scene->setItemPlacer(3, itemID);
                dock_WldItemProps->WldItemProps(0,
                                                WldPlacingItems::LevelSet,
                                                true,
                                                GlobalSettings::Placing_dontShowPropertiesBox);
                break;
            }
            case ItemTypes::WLD_MusicBox:
            {
                ui->actionRectFill->setEnabled(false);
                ui->actionCircleFill->setEnabled(false);
                ui->actionLine->setEnabled(false);
                ui->actionFill->setEnabled(false);
                activeWldEditWin()->scene->setItemPlacer(4, itemID);
                dock_WldItemProps->WldItemProps(-1, FileFormats::CreateWldLevel(), true);
                break;
            }
            }
            qApp->setActiveWindow(this);
            raise();
            activeWldEditWin()->setFocus(Qt::MouseFocusReason);
        }
    }
}

