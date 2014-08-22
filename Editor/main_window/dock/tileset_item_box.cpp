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

#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"
#include "tileset_item_box.h"

#include <QMessageBox>

TilesetItemBox::TilesetItemBox(QWidget *parent) :
    QDockWidget(parent)
{

}



//void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
//{
//    ui->actionLVLToolBox->setChecked(visible);
//}

//void MainWindow::on_actionLVLToolBox_triggered(bool checked)
//{
//    ui->LevelToolBox->setVisible(checked);
//    if(checked) ui->LevelToolBox->raise();
//}

void MainWindow::on_Tileset_Item_Box_visibilityChanged(bool visible)
{
    ui->actionTilesetBox->setChecked(visible);
}

void MainWindow::on_actionTilesetBox_triggered(bool checked)
{
        ui->Tileset_Item_Box->setVisible(checked);
        if(checked) ui->Tileset_Item_Box->raise();
}





void MainWindow::on_tilesetGroup_currentIndexChanged(int index)
{
    Q_UNUSED(index);

}



void MainWindow::on_newTileset_clicked()
{
    QMessageBox::information(this, "test", "test", QMessageBox::Ok);
}
