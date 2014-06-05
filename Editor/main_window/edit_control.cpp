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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"


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
}


void MainWindow::on_actionSelect_triggered()
{
    resetEditmodeButtons();
    ui->actionSelect->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionSelect->isChecked()))
    {
       activeLvlEditWin()->changeCursor(0);
       activeLvlEditWin()->scene->EditingMode = 0;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->DrawMode=false;
    }
}



void MainWindow::on_actionSelectOnly_triggered()
{
    resetEditmodeButtons();
    ui->actionSelectOnly->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionSelectOnly->isChecked()))
    {
       activeLvlEditWin()->changeCursor(0);
       activeLvlEditWin()->scene->EditingMode = 0;
       activeLvlEditWin()->scene->disableMoveItems=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->DrawMode=false;
    }
}


void MainWindow::on_actionEriser_triggered()
{
    resetEditmodeButtons();
    ui->actionEriser->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionEriser->isChecked()))
    {
       activeLvlEditWin()->changeCursor(1);
       activeLvlEditWin()->scene->EditingMode = 1;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->DrawMode=false;
    }
}


void MainWindow::on_actionHandScroll_triggered()
{
    resetEditmodeButtons();
    ui->actionHandScroll->setChecked(1);

    ui->ItemProperties->hide();

    if ((activeChildWindow()==1) && (ui->actionHandScroll->isChecked()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(-1);
       activeLvlEditWin()->scene->EditingMode = 0;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->DrawMode=false;
    }
}


void MainWindow::on_actionSetFirstPlayer_triggered()
{
    resetEditmodeButtons();
    ui->actionSetFirstPlayer->setChecked(1);

    ui->ItemProperties->hide();

    if((activeChildWindow()==1))
    {
        activeLvlEditWin()->changeCursor(2);
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
        activeLvlEditWin()->changeCursor(2);
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
        activeLvlEditWin()->changeCursor(3);
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
        activeLvlEditWin()->changeCursor(3);
        activeLvlEditWin()->scene->setItemPlacer( 3, 1 );
    }
}

////////////////////////////////////////////////////////////////////////////////////////
