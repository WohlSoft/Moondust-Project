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

#include "../world_scene/wld_item_placing.h"
#include "../level_scene/lvl_item_placing.h"


void MainWindow::on_actionZoomIn_triggered()
{
    if(activeChildWindow()==1)
        activeLvlEditWin()->zoomIn();
    else
    if(activeChildWindow()==3)
        activeWldEditWin()->zoomIn();

}

void MainWindow::on_actionZoomOut_triggered()
{
    if(activeChildWindow()==1)
        activeLvlEditWin()->zoomOut();
    else
    if(activeChildWindow()==3)
        activeWldEditWin()->zoomOut();
}

void MainWindow::on_actionZoomReset_triggered()
{
    if(activeChildWindow()==1)
        activeLvlEditWin()->ResetZoom();
    else
    if(activeChildWindow()==3)
        activeWldEditWin()->ResetZoom();
}

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
    ui->ResizingToolbar->setVisible(false);

    if(activeChildWindow()==1)
       activeLvlEditWin()->scene->setMessageBoxItem(false);
    else if(activeChildWindow()==3)
       activeWldEditWin()->scene->setMessageBoxItem(false);

}


void MainWindow::on_actionSelect_triggered()
{
    resetEditmodeButtons();
    ui->actionSelect->setChecked(1);

    ui->ItemProperties->hide();
    ui->WLD_ItemProps->hide();

    if ((activeChildWindow()==1) && (ui->actionSelect->isChecked()))
    {
       //activeLvlEditWin()->changeCursor(leveledit::MODE_Selecting);
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Selecting);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionSelect->isChecked()))
    {
       //activeWldEditWin()->changeCursor(WorldEdit::MODE_Selecting);
       activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Selecting);
    }
}



void MainWindow::on_actionSelectOnly_triggered()
{
    resetEditmodeButtons();
    ui->actionSelectOnly->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionSelectOnly->isChecked()))
    {
       //activeLvlEditWin()->changeCursor(leveledit::MODE_Selecting);
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_SelectingOnly);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionSelectOnly->isChecked()))
    {
       //activeWldEditWin()->changeCursor(WorldEdit::MODE_Selecting);
       activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_SelectingOnly);
    }
}


void MainWindow::on_actionEriser_triggered()
{
    resetEditmodeButtons();
    ui->actionEriser->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionEriser->isChecked()))
    {
       //activeLvlEditWin()->changeCursor(leveledit::MODE_Erasing);
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Erasing);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionEriser->isChecked()))
    {
       //activeWldEditWin()->changeCursor(WorldEdit::MODE_Erasing);
       activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Erasing);
    }

}


void MainWindow::on_actionHandScroll_triggered()
{
    resetEditmodeButtons();
    ui->actionHandScroll->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionHandScroll->isChecked()))
    {
       //activeLvlEditWin()->scene->clearSelection();
       //activeLvlEditWin()->changeCursor(leveledit::MODE_HandDrag);
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_HandScroll);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionHandScroll->isChecked()))
    {
       //activeWldEditWin()->scene->clearSelection();
       //activeWldEditWin()->changeCursor(WorldEdit::MODE_HandDrag);
       activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Selecting);
    }
}


void MainWindow::on_actionSetFirstPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetFirstPlayer->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        //activeLvlEditWin()->changeCursor(leveledit::MODE_PlaceItem);
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
        //activeLvlEditWin()->changeCursor(leveledit::MODE_PlaceItem);
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
        //activeLvlEditWin()->changeCursor(leveledit::MODE_DrawSquares);
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
        //activeLvlEditWin()->changeCursor(leveledit::MODE_DrawSquares);
        activeLvlEditWin()->scene->setItemPlacer( 3, 1 );
    }
}

////////////////////////////////////////////////////////////////////////////////////////



void MainWindow::on_actionSquareFill_triggered(bool checked)
{
        resetEditmodeButtons();
        ui->PlacingToolbar->setVisible(true);

        if (activeChildWindow()==1)
        {
            leveledit * edit = activeLvlEditWin();

            //edit->scene->clearSelection();
            //edit->changeCursor(leveledit::MODE_PlaceItem);
            //activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);

            LvlPlacingItems::fillingMode = checked;
            LvlPlacingItems::lineMode = false;
            LvlPlacingItems::floodFillingMode = false;
            ui->actionLine->setChecked(false);
            switch(edit->scene->placingItem)
            {
                case LvlScene::PLC_Block:
                   ui->PROPS_BlockSquareFill->setChecked(checked);
                   edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
                   WriteToLog(QtDebugMsg, QString("Block Square draw -> %1").arg(checked));

                break;
                case LvlScene::PLC_BGO:
                   ui->PROPS_BGOSquareFill->setChecked(checked);
                   edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id );
                   WriteToLog(QtDebugMsg, QString("BGO Square draw -> %1").arg(checked));

                break;

                default:
                break;
            }
            edit->setFocus();
        }
        else
        if (activeChildWindow()==3)
        {
            WorldEdit * edit = activeWldEditWin();

            //edit->scene->clearSelection();
            //edit->changeCursor(WorldEdit::MODE_PlaceItem);
            //activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_PlacingNew);

            WldPlacingItems::fillingMode = checked;
            WldPlacingItems::lineMode = false;
            ui->actionLine->setChecked(false);

            switch(edit->scene->placingItem)
            {
                case WldScene::PLC_Tile:
                   edit->scene->setItemPlacer(0, WldPlacingItems::TileSet.id );
                   WriteToLog(QtDebugMsg, QString("Tile Square draw -> %1").arg(checked));

                break;
                case WldScene::PLC_Scene:
                   edit->scene->setItemPlacer(1, WldPlacingItems::SceneSet.id );
                   WriteToLog(QtDebugMsg, QString("Scenery Square draw -> %1").arg(checked));

                break;
                case WldScene::PLC_Path:
                   edit->scene->setItemPlacer(2, WldPlacingItems::PathSet.id );
                   WriteToLog(QtDebugMsg, QString("Path Square draw -> %1").arg(checked));

                break;
                case WldScene::PLC_Level:
                   edit->scene->setItemPlacer(3, WldPlacingItems::LevelSet.id );
                   WriteToLog(QtDebugMsg, QString("Path Square draw -> %1").arg(checked));

                break;

                default: break;
            }
            edit->setFocus();
        }
}

void MainWindow::on_actionLine_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);

    if (activeChildWindow()==1)
    {
        leveledit * edit = activeLvlEditWin();

        //edit->scene->clearSelection();
        //edit->changeCursor(leveledit::MODE_PlaceItem);
        //activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);

        LvlPlacingItems::fillingMode = false;
        LvlPlacingItems::lineMode = checked;
        LvlPlacingItems::floodFillingMode = false;
        ui->actionSquareFill->setChecked(false);

        switch(edit->scene->placingItem)
        {
            case LvlScene::PLC_Block:
               ui->PROPS_BlockSquareFill->setChecked(false);
               edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
               WriteToLog(QtDebugMsg, QString("Block Square draw -> %1").arg(checked));

            break;
            case LvlScene::PLC_BGO:
               ui->PROPS_BlockSquareFill->setChecked(false);
               edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id );
               WriteToLog(QtDebugMsg, QString("BGO Square draw -> %1").arg(checked));

            break;

            default:
            break;
        }
        edit->setFocus();
    }
    else
    if (activeChildWindow()==3)
    {
        WorldEdit * edit = activeWldEditWin();

        //edit->scene->clearSelection();
        //edit->changeCursor(WorldEdit::MODE_PlaceItem);
        //activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_PlacingNew);

        WldPlacingItems::fillingMode = false;
        WldPlacingItems::lineMode = checked;
        ui->actionSquareFill->setChecked(false);


        switch(edit->scene->placingItem)
        {
            case WldScene::PLC_Tile:
               edit->scene->setItemPlacer(0, WldPlacingItems::TileSet.id );
               WriteToLog(QtDebugMsg, QString("Tile Line draw -> %1").arg(checked));

            break;
            case WldScene::PLC_Scene:
               edit->scene->setItemPlacer(1, WldPlacingItems::SceneSet.id );
               WriteToLog(QtDebugMsg, QString("Scenery Line draw -> %1").arg(checked));

            break;
            case WldScene::PLC_Path:
               edit->scene->setItemPlacer(2, WldPlacingItems::PathSet.id );
               WriteToLog(QtDebugMsg, QString("Path Line draw -> %1").arg(checked));

            break;
            case WldScene::PLC_Level:
               edit->scene->setItemPlacer(3, WldPlacingItems::LevelSet.id );
               WriteToLog(QtDebugMsg, QString("Path Line draw -> %1").arg(checked));

            break;

            default: break;
        }
        edit->setFocus();
    }
}

void MainWindow::on_actionFill_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);

    if (activeChildWindow()==1)
    {
        leveledit * edit = activeLvlEditWin();

        LvlPlacingItems::fillingMode = false;
        LvlPlacingItems::lineMode = false;
        LvlPlacingItems::floodFillingMode = checked;

        switch(edit->scene->placingItem)
        {
            case LvlScene::PLC_Block:
                edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
                break;
            case LvlScene::PLC_BGO:

            default:
                break;
        }
        edit->setFocus();
    }
}

void MainWindow::on_actionOverwriteMode_triggered(bool checked)
{
    LvlPlacingItems::overwriteMode = checked;
    WldPlacingItems::overwriteMode = checked;
}

void MainWindow::resizeToolbarVisible(bool vis)
{
    ui->ResizingToolbar->setVisible(vis);
}

void MainWindow::on_actionResizeApply_triggered()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->applyResizers();
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->applyResizers();
    }
    ui->ResizingToolbar->setVisible(false);
}

void MainWindow::on_actionResizeCancel_triggered()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->resetResizers();;
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->resetResizers();
    }
    ui->ResizingToolbar->setVisible(false);
}
