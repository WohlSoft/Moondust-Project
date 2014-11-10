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

#include "../level_scene/lvl_item_placing.h"
#include "../world_scene/wld_item_placing.h"
#include "../file_formats/file_formats.h"

#include "../defines.h"


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

                activeLvlEditWin()->scene->clearSelection();
                activeLvlEditWin()->changeCursor(leveledit::MODE_PlaceItem);
                activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);

                LvlPlacingItems::fillingMode = false;
                ui->actionSquareFill->setChecked(false);
                ui->actionSquareFill->setEnabled(true);

                LvlPlacingItems::lineMode = false;
                ui->actionLine->setChecked(false);
                ui->actionLine->setEnabled(true);

                ui->PlacingToolbar->setVisible(true);
                qApp->setActiveWindow(this);
                activeLvlEditWin()->setFocus();
           default:;
       }

       //Switch placing mode
       if(valid)
           switch(itemType)
           {
           case ItemTypes::LVL_Block:
               {
                   //Switch scene to placing mode:
                   activeLvlEditWin()->scene->setItemPlacer(0, itemID);

                   //Open block properties toolbox for define placing properties
                   LvlItemProps(0,LvlPlacingItems::blockSet,
                                             FileFormats::dummyLvlBgo(),
                                             FileFormats::dummyLvlNpc(), true);
                   break;
               }
           case ItemTypes::LVL_BGO:
               {
                   activeLvlEditWin()->scene->setItemPlacer(1, itemID );

                   LvlItemProps(1,FileFormats::dummyLvlBlock(),
                                             LvlPlacingItems::bgoSet,
                                             FileFormats::dummyLvlNpc(), true);
                   break;
               }
           case ItemTypes::LVL_NPC:
               {
                   ui->actionSquareFill->setEnabled(false);
                   ui->actionLine->setEnabled(false);

                   activeLvlEditWin()->scene->setItemPlacer(2, itemID );

                   LvlItemProps(2,FileFormats::dummyLvlBlock(),
                                             FileFormats::dummyLvlBgo(),
                                             LvlPlacingItems::npcSet, true);
                   break;
               }
           }
    }
    else if(activeChildWindow()==3) // World editing window
    {
        bool valid=false;
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
                 activeWldEditWin()->scene->clearSelection();
                 activeWldEditWin()->changeCursor(WorldEdit::MODE_PlaceItem);
                 activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_PlacingNew);

                 WldPlacingItems::fillingMode = false;
                 ui->actionSquareFill->setChecked(false);
                 ui->actionSquareFill->setEnabled(true);

                 WldPlacingItems::lineMode = false;
                 ui->actionLine->setChecked(false);
                 ui->actionLine->setEnabled(true);
                 qApp->setActiveWindow(this);
                 activeWldEditWin()->setFocus();
            default:;
        }
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
                        activeWldEditWin()->scene->setItemPlacer(3, itemID);
                        WldItemProps(0, WldPlacingItems::LevelSet, true);
                        break;
                    }
                case ItemTypes::WLD_MusicBox:
                    {
                        ui->actionSquareFill->setEnabled(false);
                        ui->actionLine->setEnabled(false);
                        activeWldEditWin()->scene->setItemPlacer(4, itemID);
                        WldItemProps(-1, FileFormats::dummyWldLevel(), true);
                        break;
                    }
            }

    }
}
