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

void MainWindow::on_Tileset_Item_Box_visibilityChanged(bool visible)
{
    ui->actionTilesetBox->setChecked(visible);
}

void MainWindow::on_actionTilesetBox_triggered(bool checked)
{
        ui->Tileset_Item_Box->setVisible(checked);
        if(checked) ui->Tileset_Item_Box->raise();
}


 // Create and refresh tileset box data
void MainWindow::setTileSetBox()
{
    clearTilesetGroups();
    QStringList filters("*.tsgrp.ini");
    QDir grpDir = configs.config_dir + "group_tilesets/";
    QStringList entries;
    entries = grpDir.entryList(filters, QDir::Files);
    foreach (QString f, entries) {
        TilesetGroupEditor::SimpleTilesetGroup grp;
        if(!TilesetGroupEditor::OpenSimpleTilesetGroup(configs.config_dir + "group_tilesets/" + f,grp))
            continue;
        prepareTilesetGroup(grp);
    }
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
    QTabWidget* TileSetsCategories = ui->TileSetsCategories;
    QWidget* catWid;
    QGridLayout* catLayout;
    QLabel* grpLabel;
    QComboBox* tilesetGroup;
    QSpacerItem* spItem;
    QFrame* TileSets;

    catWid = new QWidget();
    catLayout = new QGridLayout(catWid);
    catLayout->setSpacing(6);
    catLayout->setContentsMargins(11, 11, 11, 11);
    grpLabel = new QLabel(catWid);

    catLayout->addWidget(grpLabel, 0, 0, 1, 1);

    tilesetGroup = new QComboBox(catWid);

    catLayout->addWidget(tilesetGroup, 0, 1, 1, 1);

    spItem = new QSpacerItem(1283, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    catLayout->addItem(spItem, 0, 2, 1, 1);

    TileSets = new QFrame(catWid);
    TileSets->setFrameShape(QFrame::StyledPanel);
    TileSets->setFrameShadow(QFrame::Raised);

    catLayout->addWidget(TileSets, 1, 0, 1, 3);

    TileSetsCategories->addTab(catWid, QString());
    TileSetsCategories->setTabText(TileSetsCategories->indexOf(catWid), categoryItem);


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

void MainWindow::clearTilesetGroups(){
    QTabWidget* cat = ui->TileSetsCategories;
    int i = 0;
    int f = cat->count();
    while(f > i){ //include custom tab
        if(cat->tabText(i) == tr("Custom")){
            ++i;
            continue;
        }

        QWidget* acCat = cat->widget(i);
        cat->removeTab(i);
        delete acCat;
        f = cat->count();
    }
}
