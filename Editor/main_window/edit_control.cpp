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

#include "../editing/_scenes/world/wld_item_placing.h"
#include "../editing/_scenes/level/lvl_item_placing.h"


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
        ui->actionOverwriteMode->setVisible(false);
        ui->actionSquareFill->setVisible(false);
        ui->actionLine->setVisible(false);
        ui->actionFill->setVisible(false);
        ui->actionFloodSectionOnly->setVisible(false);


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
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Selecting);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionSelect->isChecked()))
    {
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
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_SelectingOnly);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionSelectOnly->isChecked()))
    {
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
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_Erasing);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionEriser->isChecked()))
    {
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
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_HandScroll);
    }
    else
    if ((activeChildWindow()==3) && (ui->actionHandScroll->isChecked()))
    {
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////



void MainWindow::on_actionSquareFill_triggered(bool checked)
{
        resetEditmodeButtons();
        ui->PlacingToolbar->setVisible(true);
            ui->actionOverwriteMode->setVisible(true);
            ui->actionSquareFill->setVisible(true);
            ui->actionLine->setVisible(true);
            ui->actionFill->setVisible(true);
            ui->actionFloodSectionOnly->setVisible(true);
            ui->actionFloodSectionOnly->setEnabled(false);

        if (activeChildWindow()==1)
        {
            leveledit * edit = activeLvlEditWin();

            LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_Square
                                                    : LvlPlacingItems::PMODE_Brush );

            ui->actionLine->setChecked(false);
            ui->actionFill->setChecked(false);

            switch(edit->scene->placingItem)
            {
                case LvlScene::PLC_Block:
                   edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
                   WriteToLog(QtDebugMsg, QString("Block Square draw -> %1").arg(checked));

                break;
                case LvlScene::PLC_BGO:
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

            WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_Square
                                                    : WldPlacingItems::PMODE_Brush );

            ui->actionLine->setChecked(false);
            ui->actionFill->setChecked(false);

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

void MainWindow::on_actionFloodSectionOnly_triggered(bool checked)
{
    LvlPlacingItems::noOutSectionFlood=checked;
}

void MainWindow::on_actionLine_triggered(bool checked)
{
    resetEditmodeButtons();
    ui->PlacingToolbar->setVisible(true);
        ui->actionOverwriteMode->setVisible(true);
        ui->actionSquareFill->setVisible(true);
        ui->actionLine->setVisible(true);
        ui->actionFill->setVisible(true);
        ui->actionFloodSectionOnly->setVisible(true);
        ui->actionFloodSectionOnly->setEnabled(false);

    if (activeChildWindow()==1)
    {
        leveledit * edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_Line
                                                : LvlPlacingItems::PMODE_Brush );

        ui->actionSquareFill->setChecked(false);
        ui->actionFill->setChecked(false);


        switch(edit->scene->placingItem)
        {
            case LvlScene::PLC_Block:
               edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
               WriteToLog(QtDebugMsg, QString("Block Line draw -> %1").arg(checked));

            break;
            case LvlScene::PLC_BGO:
               edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id );
               WriteToLog(QtDebugMsg, QString("BGO Line draw -> %1").arg(checked));

            break;
            case LvlScene::PLC_NPC:
               edit->scene->setItemPlacer(2, LvlPlacingItems::npcSet.id );
               WriteToLog(QtDebugMsg, QString("NPC Line draw -> %1").arg(checked));

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

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_Line
                                                : WldPlacingItems::PMODE_Brush );

        ui->actionSquareFill->setChecked(false);
        ui->actionFill->setChecked(false);

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
        ui->actionOverwriteMode->setVisible(true);
        ui->actionSquareFill->setVisible(true);
        ui->actionLine->setVisible(true);
        ui->actionFill->setVisible(true);
        ui->actionFloodSectionOnly->setVisible(true);
        ui->actionFloodSectionOnly->setEnabled(checked);

    if (activeChildWindow()==1)
    {
        leveledit * edit = activeLvlEditWin();

        LvlPlacingItems::placingMode = (checked ? LvlPlacingItems::PMODE_FloodFill
                                                : LvlPlacingItems::PMODE_Brush );

        ui->actionSquareFill->setChecked(false);
        ui->actionLine->setChecked(false);

        switch(edit->scene->placingItem)
        {
            case LvlScene::PLC_Block:
                edit->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
                break;
            case LvlScene::PLC_BGO:
                edit->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id );
                break;
            default:
                break;
        }
        edit->setFocus();
    }
    else if (activeChildWindow()==3)
    {
        WorldEdit * edit = activeWldEditWin();

        WldPlacingItems::placingMode = (checked ? WldPlacingItems::PMODE_FloodFill
                                                : WldPlacingItems::PMODE_Brush );

        ui->actionSquareFill->setChecked(false);
        ui->actionLine->setChecked(false);

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
