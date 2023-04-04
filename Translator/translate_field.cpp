#include "translate_field.h"
#include "ui_translate_field.h"

TranslateField::TranslateField(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TranslateField)
{
    ui->setupUi(this);
}

TranslateField::~TranslateField()
{
    delete ui;
}

void TranslateField::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
