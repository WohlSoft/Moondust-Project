#include "levelprops.h"
#include "ui_levelprops.h"

LevelProps::LevelProps(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelProps)
{
    ui->setupUi(this);
}

LevelProps::~LevelProps()
{
    delete ui;
}
