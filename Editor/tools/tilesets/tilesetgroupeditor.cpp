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

#include "tilesetgroupeditor.h"
#include <ui_tilesetgroupeditor.h>

#include <QtDebug>

#include "tilesetitembutton.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/util.h"
#include "../../dev_console/devconsole.h"
#include "../../defines.h"

QString TilesetGroupEditor::lastFileName="";

TilesetGroupEditor::TilesetGroupEditor(QGraphicsScene *scene, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    scn = scene;
    ui->setupUi(this);
    layout = new FlowLayout();
    delete ui->PreviewBox->layout();
    ui->PreviewBox->setLayout(layout);
    ui->tilesetList->clear();
    ui->tilesetList->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tilesetList->setDragEnabled(true);
    ui->tilesetList->setDragDropMode(QAbstractItemView::NoDragDrop);
    //ui->tilesetList->viewport()->setAcceptDrops(true);
    //ui->tilesetList->setDropIndicatorShown(true);
    lastFileName="";

    //qDebug() << "connecting";

//    connect(ui->tilesetList->model(),
//            SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
//            this,
//            SLOT(movedTileset(QModelIndex,int,int,QModelIndex,int)));

}


TilesetGroupEditor::~TilesetGroupEditor()
{
    delete layout;
    delete ui;
}

SimpleTilesetGroup TilesetGroupEditor::toSimpleTilesetGroup()
{
    SimpleTilesetGroup s;
    s.groupName = ui->tilesetGroupName->text();
    s.groupCat = ui->category->text();
    for(int i = 0; i < tilesets.size(); ++i){
        s.tilesets << tilesets[i].first;
    }
    return s;
}

void TilesetGroupEditor::SaveSimpleTilesetGroup(const QString &path, const SimpleTilesetGroup &tilesetGroup)
{
    QString modifiedPath;
    modifiedPath = path;

    QSettings simpleTilesetGroupINI(modifiedPath,QSettings::IniFormat);
    simpleTilesetGroupINI.setIniCodec("UTF-8");
    simpleTilesetGroupINI.clear();
    simpleTilesetGroupINI.beginGroup("tileset-group"); //HEADER
    simpleTilesetGroupINI.setValue("category", tilesetGroup.groupCat);
    simpleTilesetGroupINI.setValue("name", tilesetGroup.groupName);
    simpleTilesetGroupINI.setValue("tilesets-count", tilesetGroup.tilesets.size());
    simpleTilesetGroupINI.endGroup();
    simpleTilesetGroupINI.beginGroup("tilesets");
    for(int i = 1; i < tilesetGroup.tilesets.size()+1; ++i){
        simpleTilesetGroupINI.setValue(QString("tileset-%1").arg(i),tilesetGroup.tilesets[i-1]);
    }
    simpleTilesetGroupINI.endGroup();

    lastFileName = QFileInfo(path).baseName();
}

bool TilesetGroupEditor::OpenSimpleTilesetGroup(const QString &path, SimpleTilesetGroup &tilesetGroup)
{
    QSettings simpleTilesetINI(path,QSettings::IniFormat);
    simpleTilesetINI.setIniCodec("UTF-8");
    simpleTilesetINI.beginGroup("tileset-group");
    if(!simpleTilesetINI.contains("tilesets-count"))
        return false;

    tilesetGroup.groupName = simpleTilesetINI.value("name","").toString();
    tilesetGroup.groupCat = simpleTilesetINI.value("category","").toString();
    int tc = simpleTilesetINI.value("tilesets-count",0).toInt()+1;
    simpleTilesetINI.endGroup();
    simpleTilesetINI.beginGroup("tilesets");
    for(int i = 1; i < tc; ++i){
        if(!simpleTilesetINI.contains(QString("tileset-%1").arg(i)))
            return false;
        tilesetGroup.tilesets << simpleTilesetINI.value(QString("tileset-%1").arg(i)).toString();
    }
    simpleTilesetINI.endGroup();
    lastFileName = QFileInfo(path).baseName();
    return true;
}

void TilesetGroupEditor::on_Close_clicked()
{
    this->close();
}

void TilesetGroupEditor::on_addTileset_clicked()
{
    QString f = QFileDialog::getOpenFileName(this, tr("Select Tileset"), MainWinConnect::configs->config_dir+"tilesets/", QString("PGE Tileset (*.ini)"));
    if(f.isEmpty())
        return;

    if(!f.startsWith(MainWinConnect::configs->config_dir+"tilesets/")){
        QFile file(f);
        QFile tar(f = (MainWinConnect::configs->config_dir+"tilesets/" + f.section("/",-1,-1)));
        if(tar.exists()){
            QMessageBox msgBox;
            msgBox.setText(tr("There is already a file called '%1'!\nImport anyway and overwrite?"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            if(msgBox.exec() == QMessageBox::Ok)
                tar.remove();
            else
                return;
        }
        file.copy(f);
    }

    SimpleTileset t;
    if(tileset::OpenSimpleTileset(f,t)){
        QPair<QString, SimpleTileset> i;
        i.first = f.section("/",-1,-1);
        i.second = t;
        tilesets << i;
        redrawAll();
    }else{
        QMessageBox::warning(this, tr("Failed to load tileset!"), tr("Failed to load tileset!\nData may be corrupted!"));
    }

}

void TilesetGroupEditor::on_RemoveTileset_clicked()
{
    if(ui->tilesetList->selectedItems().size() == 0)
        return;
    tilesets.removeAt(ui->tilesetList->row(ui->tilesetList->selectedItems()[0]));
    redrawAll();
}

void TilesetGroupEditor::on_Open_clicked()
{
    QString f = QFileDialog::getOpenFileName(this, tr("Select Tileset Group"), MainWinConnect::configs->config_dir+"group_tilesets/", QString("PGE Tileset Group (*.tsgrp.ini)"));
    if(f.isEmpty())
        return;

    if(!f.startsWith(MainWinConnect::configs->config_dir+"group_tilesets/")){
        QFile file(f);
        QFile tar(f = (MainWinConnect::configs->config_dir+"group_tilesets/" + f.section("/",-1,-1)));
        if(tar.exists()){
            QMessageBox msgBox;
            msgBox.setText(tr("There is already a file called '%1'!\nImport anyway and overwrite?"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            if(msgBox.exec() == QMessageBox::Ok)
                tar.remove();
            else
                return;
        }
        file.copy(f);
    }

    SimpleTilesetGroup t;
    if(OpenSimpleTilesetGroup(f,t)){
        tilesets.clear();
        ui->tilesetGroupName->setText(t.groupName);
        ui->category->setText(t.groupCat);
        foreach (QString tarName, t.tilesets) {
            QString rootTilesetDir = MainWinConnect::configs->config_dir+"tilesets/";
            SimpleTileset st;
            if(tileset::OpenSimpleTileset(rootTilesetDir+tarName,st)){
                tilesets << qMakePair<QString, SimpleTileset>(tarName,st);
            }
        }
        redrawAll();
    }else{
        QMessageBox::warning(this, tr("Failed to load tileset group!"), tr("Failed to load tileset group!\nData may be corrupted!"));
    }

}

void TilesetGroupEditor::on_Save_clicked()
{
    QDir(MainWinConnect::configs->config_dir).mkpath("group_tilesets/");

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Please enter a filename!"),
                                              tr("Filename:"), QLineEdit::Normal,
                                              lastFileName.isEmpty()?ui->tilesetGroupName->text():lastFileName, &ok);
    if (!ok || fileName.isEmpty())
        return;

    if(!fileName.endsWith(".tsgrp.ini"))
        fileName += ".tsgrp.ini";

    SaveSimpleTilesetGroup(MainWinConnect::configs->config_dir + "group_tilesets/" + fileName,toSimpleTilesetGroup());
}

void TilesetGroupEditor::redrawAll()
{
    util::memclear(ui->tilesetList);
    //QGroupBox* preview = ui->PreviewBox;
    util::clearLayoutItems(layout);
    for(int i = 0; i < tilesets.size(); ++i){
        QListWidgetItem * xxx=new QListWidgetItem;
        xxx->setText(tilesets[i].first);
        xxx->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tilesetList->addItem(xxx);

        QGroupBox *f= new QGroupBox;
        QGridLayout* l = new QGridLayout();
        l->setContentsMargins(4,4,4,4);
        l->setSpacing(2);
        f->setLayout(l);
        f->setTitle(tilesets[i].second.tileSetName);
        SimpleTileset* items = &tilesets[i].second;
        for(int j = 0; j < items->items.size(); ++j){
            SimpleTilesetItem* item = &items->items[j];
            TilesetItemButton* ib = new TilesetItemButton(MainWinConnect::configs, scn);
            ib->applySize(32,32);
            ib->applyItem(items->type,item->id);
            l->addWidget(ib,item->row, item->col);
        }
        DevConsole::log(QString("Current GridLayout Items: %1").arg(l->count()), QString("Tileset"));
        layout->addWidget(f);
        DevConsole::log(QString("Current FlowLayout Items: %1").arg(layout->count()), QString("Tileset"));
    }
}

void TilesetGroupEditor::on_tilesetUp_clicked()
{
    if(ui->tilesetList->selectedItems().size() == 0)
        return;

    int i = ui->tilesetList->row(ui->tilesetList->selectedItems()[0]);

    if(0 < i){
        tilesets.swap(i,i-1);
        redrawAll();
        ui->tilesetList->setCurrentRow(i-1);
    }
}

void TilesetGroupEditor::on_tilesetDown_clicked()
{
    if(ui->tilesetList->selectedItems().size() == 0)
        return;

    int i = ui->tilesetList->row(ui->tilesetList->selectedItems()[0]);

    if(i+1 < tilesets.size() ){
        tilesets.swap(i,i+1);
        redrawAll();
        ui->tilesetList->setCurrentRow(i+1);
    }
}

void TilesetGroupEditor::movedTileset(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destinationParent)

    qDebug()<<"Tilesets moved";

    tilesets.swap(sourceStart,destinationRow);
    redrawAll();
    ui->tilesetList->setCurrentRow(destinationRow);
}
