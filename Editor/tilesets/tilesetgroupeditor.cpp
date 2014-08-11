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
#include "ui_tilesetgroupeditor.h"

#include "tilesetitembutton.h"
#include "../common_features/mainwinconnect.h"
#include "../common_features/util.h"
#include "../dev_console/devconsole.h"
#include "../defines.h"

TilesetGroupEditor::TilesetGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    ui->setupUi(this);
    layout = new FlowLayout();
    delete ui->PreviewBox->layout();
    ui->PreviewBox->setLayout(layout);
//    TilesetItemButton* b = new TilesetItemButton(&(MainWinConnect::pMainWin->configs));
//    b->applySize(32,32);
//    b->applyItem(ItemTypes::LVL_Block,1,32,32);
//    ui->PreviewBox->layout()->addWidget(b);
    ui->tilesetList->clear();
}

TilesetGroupEditor::~TilesetGroupEditor()
{
    delete layout;
    delete ui;
}

TilesetGroupEditor::SimpleTilesetGroup TilesetGroupEditor::toSimpleTilesetGroup()
{
    SimpleTilesetGroup s;
    s.groupName = ui->tilesetGroupName->text();
    for(int i = 0; i < tilesets.size(); ++i){
        s.tilesets << tilesets[i].first;
    }
    return s;
}

void TilesetGroupEditor::SaveSimpleTilesetGroup(const QString &path, const TilesetGroupEditor::SimpleTilesetGroup &tilesetGroup)
{
    QString modifiedPath;
#ifdef __linux__

    if(!path.contains("*.ini"))
    {
        modifiedPath = path + ".ini";
        //QMessageBox::information(mainwindow, "Information", path, QMessageBox.Ok);
    }
    else
    {
        modifiedPath = path;
    }
#elif _WIN32
    modifiedPath = path;
#endif
    QSettings simpleTilesetGroupINI(modifiedPath,QSettings::IniFormat);
    simpleTilesetGroupINI.setIniCodec("UTF-8");
    simpleTilesetGroupINI.clear();
    simpleTilesetGroupINI.beginGroup("tileset-group"); //HEADER
    simpleTilesetGroupINI.setValue("category", "ND");
    simpleTilesetGroupINI.setValue("name", tilesetGroup.groupName);
    simpleTilesetGroupINI.setValue("tilesets-count", tilesetGroup.tilesets.size());
    simpleTilesetGroupINI.endGroup();
    simpleTilesetGroupINI.beginGroup("tilesets");
    for(int i = 1; i < tilesetGroup.tilesets.size()+1; ++i){
        simpleTilesetGroupINI.setValue(QString("tileset-%1").arg(i),tilesetGroup.tilesets[i-1]);
    }
    simpleTilesetGroupINI.endGroup();
}

bool TilesetGroupEditor::OpenSimpleTilesetGroup(const QString &path, TilesetGroupEditor::SimpleTilesetGroup &tileset)
{

}

void TilesetGroupEditor::on_Close_clicked()
{
    this->close();
}

void TilesetGroupEditor::on_addTileset_clicked()
{
    QString f = QFileDialog::getOpenFileName(this, tr("Select Tileset"), QApplication::applicationDirPath() + "/" +  "configs/SMBX/", QString("PGE Tileset (*.ini)"));
    if(f.isEmpty())
        return;

    tileset::SimpleTileset t;
    if(tileset::OpenSimpleTileset(f,t)){
        QPair<QString, tileset::SimpleTileset> i;
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

}

void TilesetGroupEditor::on_Save_clicked()
{

}

void TilesetGroupEditor::redrawAll()
{
    util::memclear(ui->tilesetList);
    //QGroupBox* preview = ui->PreviewBox;
    util::clearLayoutItems(layout);
    for(int i = 0; i < tilesets.size(); ++i){
        ui->tilesetList->addItem(tilesets[i].first);

        QGroupBox *f= new QGroupBox;
        QGridLayout* l = new QGridLayout();
        f->setLayout(l);
        f->setTitle(tilesets[i].first.section('.',0,0));
        tileset::SimpleTileset* items = &tilesets[i].second;
        for(int j = 0; j < items->items.size(); ++j){
            tileset::SimpleTilesetItem* item = &items->items[j];
            TilesetItemButton* ib = new TilesetItemButton(&MainWinConnect::pMainWin->configs);
            ib->applySize(32,32);
            ib->applyItem(items->type,item->id);
            l->addWidget(ib,item->row, item->col);
        }
        DevConsole::log(QString("Current GridLayout Items: %1").arg(l->count()), QString("Tileset"));
        layout->addWidget(f);
        DevConsole::log(QString("Current FlowLayout Items: %1").arg(layout->count()), QString("Tileset"));
    }
}
