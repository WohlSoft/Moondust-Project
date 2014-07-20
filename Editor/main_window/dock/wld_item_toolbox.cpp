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
#include "../../file_formats/file_formats.h"
#include "../../common_features/util.h"

#include "../../data_configs/custom_data.h"


void MainWindow::on_WLD_TilesList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeTile

    if ((activeChildWindow()==3) && (ui->WLD_TilesList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(0, item->data(3).toInt() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }

}

