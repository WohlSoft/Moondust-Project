#include "tilesetgroupeditor.h"
#include "ui_tilesetgroupeditor.h"

#include "tilesetitembutton.h"
#include "../common_features/mainwinconnect.h"
#include "../defines.h"

TilesetGroupEditor::TilesetGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    ui->setupUi(this);
    TilesetItemButton* b = new TilesetItemButton(&(MainWinConnect::pMainWin->configs));
    b->applySize(32,32);
    b->applyItem(ItemTypes::LVL_Block,1,32,32);
    ui->PreviewBox->layout()->addWidget(b);
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
