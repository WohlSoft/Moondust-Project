#include "gifs2png.h"
#include "ui_gifs2png.h"

GIFs2PNG::GIFs2PNG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GIFs2PNG)
{
    ui->setupUi(this);
}

GIFs2PNG::~GIFs2PNG()
{
    delete ui;
}

void GIFs2PNG::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GIFs2PNG::on_DoMadJob_clicked()
{

}

void GIFs2PNG::on_configPackPathBrowse_clicked()
{

}

void GIFs2PNG::on_inPathBrowse_clicked()
{

}
