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
#include "../../common_features/util.h"

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

QFrame* MainWindow::getFrameTilesetOfTab(QWidget* catTab){
    return catTab->findChild<QFrame*>();
}

QComboBox* MainWindow::getGroupComboboxOfTab(QWidget* catTab){
    return catTab->findChild<QComboBox*>();
}

QWidget* MainWindow::findTabWidget(const QString &categoryItem){
    QTabWidget* cat = ui->TileSetsCategories;
    for(int i = 0; i < cat->count(); ++i){
        if(cat->tabText(i) == categoryItem){
            return cat->widget(i);
        }
    }
    return 0;
}

QWidget* MainWindow::makeCategory(const QString &categoryItem){
    QWidget* catWid = new QWidget();
    QGridLayout* catLayout = new QGridLayout();
    QComboBox* catgrp = new QComboBox();
    QFrame* catTileset = new QFrame();
    catWid->setLayout(catLayout);
    catLayout->addWidget(new QLabel(tr("Groups: ")), 0, 0);
    catLayout->addWidget(catgrp, 0, 1);
    catLayout->addItem(new QSpacerItem(207, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);
    catLayout->addWidget(catTileset, 1, 0, -1, -1);
    ui->TileSetsCategories->addTab(catWid, categoryItem);
    return catWid;
}

void MainWindow::prepareTilesetGroup(const TilesetGroupEditor::SimpleTilesetGroup &tilesetGroups){
    QWidget *t = findTabWidget(tilesetGroups.groupCat);
    if(!t){
        t = makeCategory(tilesetGroups.groupCat);
    }
    QComboBox *c = getGroupComboboxOfTab(t);
    if(!c)
        return;

    if(!util::contains(c,tilesetGroups.groupName)){
        c->addItem(tilesetGroups.groupName);
    }
}
