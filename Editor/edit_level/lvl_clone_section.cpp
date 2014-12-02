#include "lvl_clone_section.h"
#include "ui_lvl_clone_section.h"

LvlCloneSection::LvlCloneSection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LvlCloneSection)
{
    ui->setupUi(this);
}

LvlCloneSection::~LvlCloneSection()
{
    delete ui;
}
