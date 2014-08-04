#include "tilesetgroupeditor.h"
#include "ui_tilesetgroupeditor.h"

TilesetGroupEditor::TilesetGroupEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetGroupEditor)
{
    ui->setupUi(this);
}

TilesetGroupEditor::~TilesetGroupEditor()
{
    delete ui;
}
