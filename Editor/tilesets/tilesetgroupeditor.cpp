#include "tilesetgroupeditor.h"
#include "ui_tilesetgroupeditor.h"

#include "tilesetitembutton.h"
#include "../common_features/mainwinconnect.h"
#include "../common_features/util.h"
#include "../common_features/flowlayout.h"
#include "../dev_console/devconsole.h"
#include "../defines.h"

TilesetGroupEditor::TilesetGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    ui->setupUi(this);
    ui->PreviewBox->setLayout(new FlowLayout(ui->PreviewBox));
//    TilesetItemButton* b = new TilesetItemButton(&(MainWinConnect::pMainWin->configs));
//    b->applySize(32,32);
//    b->applyItem(ItemTypes::LVL_Block,1,32,32);
//    ui->PreviewBox->layout()->addWidget(b);
    ui->tilesetList->clear();
}

TilesetGroupEditor::~TilesetGroupEditor()
{
    delete ui;
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
    QGroupBox* preview = ui->PreviewBox;
    while(preview->layout()->count() != 0){
        preview->layout()->removeItem(preview->layout()->itemAt(0));
    }
    for(int i = 0; i < tilesets.size(); ++i){
        ui->tilesetList->addItem(tilesets[i].first);

        QGridLayout* l = new QGridLayout();
        tileset::SimpleTileset* items = &tilesets[i].second;
        for(int j = 0; j < items->items.size(); ++j){
            tileset::SimpleTilesetItem* item = &items->items[j];
            TilesetItemButton* ib = new TilesetItemButton(&MainWinConnect::pMainWin->configs);
            ib->applySize(32,32);
            ib->applyItem(items->type,item->id);
            l->addWidget(ib,item->row, item->col);

        }
        DevConsole::log(QString("Current GridLayout Items: %1").arg(l->count()), QString("Tileset"));
        preview->layout()->addItem(l);
        DevConsole::log(QString("Current FlowLayout Items: %1").arg(preview->layout()->count()), QString("Tileset"));
    }
}
