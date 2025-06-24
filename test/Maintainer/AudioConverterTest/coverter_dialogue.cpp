#include "coverter_dialogue.h"
#include "./ui_coverter_dialogue.h"

CoverterDialogue::CoverterDialogue(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CoverterDialogue)
{
    ui->setupUi(this);
}

CoverterDialogue::~CoverterDialogue()
{
    delete ui;
}
