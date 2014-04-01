#include "saveimage.h"
#include "ui_saveimage.h"

ExportToImage::ExportToImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportToImage)
{
    ui->setupUi(this);
}

ExportToImage::~ExportToImage()
{
    delete ui;
}
