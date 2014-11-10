#include "wld_saveimage.h"
#include <ui_wld_saveimage.h>

bool WldSaveImage_lock=false;

WldSaveImage::WldSaveImage(QRect sourceRect, QSize targetSize, bool proportion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WldSaveImage)
{
    getRect = sourceRect;
    imageSize = targetSize;
    saveProportion = proportion;
    hideMusBoxes = false;
    ui->setupUi(this);

    WldSaveImage_lock=true;

    ui->imgWidth->setValue(imageSize.width());
    ui->imgHeight->setValue(imageSize.height());
    ui->SaveProportion->setChecked(saveProportion);

    ui->ExportRect->setText(tr("Will be exported:\nTop:\t%1\nLeft:\t%2\nRight:\t%3\nBottom:\t%4")
                            .arg(getRect.top())
                            .arg(getRect.left())
                            .arg(getRect.right())
                            .arg(getRect.bottom()));

    WldSaveImage_lock=false;
}

WldSaveImage::~WldSaveImage()
{
    delete ui;
}

void WldSaveImage::on_imgHeight_valueChanged(int arg1)
{
    if(WldSaveImage_lock) return;

    if( (ui->SaveProportion->isChecked()) && (ui->imgHeight->hasFocus()) )
        ui->imgWidth->setValue( qRound(qreal(arg1) / ( qreal(imageSize.height())/qreal(imageSize.width()) ) ) );
}

void WldSaveImage::on_imgWidth_valueChanged(int arg1)
{
    if(WldSaveImage_lock) return;

    if( (ui->SaveProportion->isChecked()) && (ui->imgWidth->hasFocus()) )
        ui->imgHeight->setValue( qRound(qreal(arg1) / ( qreal(imageSize.width())/qreal(imageSize.height())  )) );

}

void WldSaveImage::on_SaveProportion_clicked(bool checked)
{
    if(WldSaveImage_lock) return;

    WldSaveImage_lock=true;

    if(checked)
    {
        ui->imgWidth->setValue( imageSize.width());
        ui->imgHeight->setValue( imageSize.height());
    }

    WldSaveImage_lock=false;
}

void WldSaveImage::on_HidePaths_clicked(bool checked)
{
    if(WldSaveImage_lock) return;
    hidePaths = checked;
}

void WldSaveImage::on_buttonBox_accepted()
{
    imageSize.setWidth(ui->imgWidth->value());
    imageSize.setHeight(ui->imgHeight->value());
    saveProportion = ui->SaveProportion->isChecked();
    hideMusBoxes=ui->HideMusBoxes->isChecked();
    hidePaths = ui->HidePaths->isChecked();
    accept();
}

void WldSaveImage::on_buttonBox_rejected()
{
    reject();
}

void WldSaveImage::on_HideMusBoxes_clicked(bool checked)
{
    hideMusBoxes = checked;
}
