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

#include <editing/_scenes/level/lvl_item_placing.h>
#include <editing/_scenes/world/wld_item_placing.h>
#include <file_formats/file_formats.h>

#include <defines.h>
#include <main_window/global_settings.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>


void MainWindow::on_action_Placing_ShowProperties_triggered(bool checked)
{
    if(activeChildWindow()==1) // Level editing window
    {
        switch(Placing_ShowProperties_lastType)
        {
        case ItemTypes::LVL_Block:
        case ItemTypes::LVL_BGO:
        case ItemTypes::LVL_NPC:
            {
                if(checked)
                {
                    ui->ItemProperties->show();
                    ui->ItemProperties->raise();
                }
                else
                    ui->ItemProperties->hide();
                break;
            }
        }
    }
    else if(activeChildWindow()==3) // World editing window
    {
        switch(Placing_ShowProperties_lastType)
        {
            case ItemTypes::WLD_Level:
            {
                if(checked)
                {
                    ui->WLD_ItemProps->show();
                    ui->WLD_ItemProps->raise();
                }
                else
                    ui->WLD_ItemProps->hide();
                break;
            }
        }
    }
}

void MainWindow::on_actionSetFirstPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetFirstPlayer->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        activeLvlEditWin()->scene->setItemPlacer( 5, 0 );
    }

}

void MainWindow::on_actionSetSecondPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetSecondPlayer->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        activeLvlEditWin()->scene->setItemPlacer( 5, 1 );
    }

}

void MainWindow::on_actionDrawWater_triggered()
{
    resetEditmodeButtons();
    ui->actionDrawWater->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        activeLvlEditWin()->scene->setItemPlacer( 3, 0 );
    }
}

void MainWindow::on_actionDrawSand_triggered()
{
    resetEditmodeButtons();
    ui->actionDrawSand->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        activeLvlEditWin()->scene->setItemPlacer( 3, 1 );
    }
}


void MainWindow::SwitchPlacingItem(int itemType, unsigned long itemID)
{
    if(activeChildWindow()==1) // Level editing window
    {
       bool valid=false;
       switch(itemType)
       {
           case ItemTypes::LVL_Block:
           case ItemTypes::LVL_BGO:
           case ItemTypes::LVL_NPC:
                valid=true;

                resetEditmodeButtons();
                ui->PlacingToolbar->setVisible(true);
                    ui->actionOverwriteMode->setVisible(true);
                    ui->actionSquareFill->setVisible(true);
                    ui->actionLine->setVisible(true);
                    ui->actionFill->setVisible(true);
                    ui->actionFloodSectionOnly->setVisible(true);
                    ui->actionFloodSectionOnly->setEnabled(ui->actionFill->isChecked());

                activeLvlEditWin()->scene->clearSelection();
                activeLvlEditWin()->changeCursor(LevelEdit::MODE_PlaceItem);
                activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);


                LvlPlacingItems::placingMode = LvlPlacingItems::PMODE_Brush;

                //LvlPlacingItems::squareFillingMode = false;
                ui->actionSquareFill->setChecked(false);
                ui->actionSquareFill->setEnabled(true);

                //LvlPlacingItems::lineMode = false;
                ui->actionLine->setChecked(false);
                ui->actionLine->setEnabled(true);

                //LvlPlacingItems::floodFillingMode = false;
                ui->actionFill->setChecked(false);
                ui->actionFill->setEnabled(true);


                qApp->setActiveWindow(this);
                activeLvlEditWin()->setFocus();
           default:;
       }

       Placing_ShowProperties_lastType = itemType;

       //Switch placing mode
       if(valid)
           switch(itemType)
           {
           case ItemTypes::LVL_Block:
               {
                   ui->action_Placing_ShowProperties->setChecked(true);
                   ui->action_Placing_ShowProperties->setEnabled(true);
                   //Switch scene to placing mode:
                   activeLvlEditWin()->scene->setItemPlacer(0, itemID);

                   //Open block properties toolbox for define placing properties
                   LvlItemProps(0,LvlPlacingItems::blockSet,
                                             FileFormats::dummyLvlBgo(),
                                             FileFormats::dummyLvlNpc(), true);

                   if(GlobalSettings::Placing_dontShowPropertiesBox)
                        ui->ItemProperties->hide();

                   break;
               }
           case ItemTypes::LVL_BGO:
               {
                   ui->action_Placing_ShowProperties->setChecked(true);
                   ui->action_Placing_ShowProperties->setEnabled(true);

                   activeLvlEditWin()->scene->setItemPlacer(1, itemID );

                   LvlItemProps(1,FileFormats::dummyLvlBlock(),
                                             LvlPlacingItems::bgoSet,
                                             FileFormats::dummyLvlNpc(), true);

                   if(GlobalSettings::Placing_dontShowPropertiesBox)
                        ui->ItemProperties->hide();

                   break;
               }
           case ItemTypes::LVL_NPC:
               {
                   ui->action_Placing_ShowProperties->setChecked(true);
                   ui->action_Placing_ShowProperties->setEnabled(true);

                   ui->actionSquareFill->setEnabled(false);
                   ui->actionFill->setEnabled(false);

                   activeLvlEditWin()->scene->setItemPlacer(2, itemID );

                   LvlItemProps(2,FileFormats::dummyLvlBlock(),
                                             FileFormats::dummyLvlBgo(),
                                             LvlPlacingItems::npcSet, true);

                   if(GlobalSettings::Placing_dontShowPropertiesBox)
                        ui->ItemProperties->hide();

                   break;
               }
           }
    }
    else if(activeChildWindow()==3) // World editing window
    {
        bool valid=false;
        ui->action_Placing_ShowProperties->setChecked(false);
        ui->action_Placing_ShowProperties->setEnabled(false);
        switch(itemType)
        {
            case ItemTypes::WLD_Tile:
            case ItemTypes::WLD_Scenery:
            case ItemTypes::WLD_Path:
            case ItemTypes::WLD_Level:
            case ItemTypes::WLD_MusicBox:
                 valid=true;

                 resetEditmodeButtons();

                 ui->PlacingToolbar->setVisible(true);
                     ui->actionOverwriteMode->setVisible(true);
                     ui->actionSquareFill->setVisible(true);
                     ui->actionLine->setVisible(true);
                     ui->actionFill->setVisible(true);
                     ui->actionFloodSectionOnly->setVisible(true);
                     ui->actionFloodSectionOnly->setEnabled(ui->actionFill->isChecked());

                 activeWldEditWin()->scene->clearSelection();
                 activeWldEditWin()->changeCursor(WorldEdit::MODE_PlaceItem);
                 activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_PlacingNew);

                 WldPlacingItems::placingMode = WldPlacingItems::PMODE_Brush;

                 //WldPlacingItems::squarefillingMode = false;
                 ui->actionSquareFill->setChecked(false);
                 ui->actionSquareFill->setEnabled(true);

                 //WldPlacingItems::lineMode = false;
                 ui->actionLine->setChecked(false);
                 ui->actionLine->setEnabled(true);

                 //WldPlacingItems::floodFillingMode = false;
                 ui->actionFill->setChecked(false);
                 ui->actionFill->setEnabled(true);

                 qApp->setActiveWindow(this);
                 activeWldEditWin()->setFocus();
            default:;
        }

        Placing_ShowProperties_lastType = itemType;

        //Switch placing mode
        if(valid)
            switch(itemType)
            {
                case ItemTypes::WLD_Tile:
                    {
                        activeWldEditWin()->scene->setItemPlacer(0, itemID);
                        WldItemProps(-1, FileFormats::dummyWldLevel(), true);
                        break;
                    }
                case ItemTypes::WLD_Scenery:
                    {
                        activeWldEditWin()->scene->setItemPlacer(1, itemID);
                        WldItemProps(-1, FileFormats::dummyWldLevel(), true);
                        break;
                    }
                case ItemTypes::WLD_Path:\
                    {
                        activeWldEditWin()->scene->setItemPlacer(2, itemID);
                        WldItemProps(-1, FileFormats::dummyWldLevel(), true);
                        break;
                    }
                case ItemTypes::WLD_Level:
                    {
                        ui->action_Placing_ShowProperties->setChecked(true);
                        ui->action_Placing_ShowProperties->setEnabled(true);

                        activeWldEditWin()->scene->setItemPlacer(3, itemID);
                        WldItemProps(0, WldPlacingItems::LevelSet, true);

                        if(GlobalSettings::Placing_dontShowPropertiesBox)
                             ui->WLD_ItemProps->hide();

                        break;
                    }
                case ItemTypes::WLD_MusicBox:
                    {
                        ui->actionSquareFill->setEnabled(false);
                        ui->actionLine->setEnabled(false);
                        ui->actionFill->setEnabled(false);
                        activeWldEditWin()->scene->setItemPlacer(4, itemID);
                        WldItemProps(-1, FileFormats::dummyWldLevel(), true);
                        break;
                    }
            }

    }
}

