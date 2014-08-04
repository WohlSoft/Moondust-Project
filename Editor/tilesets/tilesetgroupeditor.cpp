#include "tilesetgroupeditor.h"
#include "ui_tilesetgroupeditor.h"

#include "tilesetitembutton.h"
#include "../common_features/mainwinconnect.h"

TilesetGroupEditor::TilesetGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    ui->setupUi(this);
    //ui->PreviewBox->layout()->addWidget(new TilesetItemButton(&(MainWinConnect::pMainWin->configs)));
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
