#include "world_edit.h"
#include "ui_world_edit.h"

WorldEdit::WorldEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorldEdit)
{
    ui->setupUi(this);
}

WorldEdit::~WorldEdit()
{
    delete ui;
}
