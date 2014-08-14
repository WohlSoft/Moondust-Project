/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

#include "saveimage.h"
#include "ui_saveimage.h"

ExportToImage::ExportToImage(QVector<long> &imgSize, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportToImage)
{
    imageSize = imgSize;
    ui->setupUi(this);
    if(imageSize.size()>=3)
    {
        ui->imgHeight->setValue(imageSize[0]);
        ui->imgWidth->setValue(imageSize[1]);
        ui->SaveProportion->setChecked((bool)imageSize[2]);
    }

}

ExportToImage::~ExportToImage()
{
    delete ui;
}

bool ExportToImage::HideWatersAndDoors()
{
    return ui->HideWarpsWaters->isChecked();
}

bool ExportToImage::TiledBackground()
{
    return ui->tiledBackround->isChecked();
}

void ExportToImage::on_imgHeight_valueChanged(int arg1)
{
    if( (ui->SaveProportion->isChecked()) && (ui->imgHeight->hasFocus()) )
        ui->imgWidth->setValue( (int)round((float)arg1 / ((float)imageSize[0]/(float)imageSize[1])) );

}

void ExportToImage::on_imgWidth_valueChanged(int arg1)
{
    if( (ui->SaveProportion->isChecked()) && (ui->imgWidth->hasFocus()) )
        ui->imgHeight->setValue( (int)round((float)arg1 / ((float)imageSize[1]/(float)imageSize[0])) );
}

void ExportToImage::on_buttonBox_accepted()
{
    imageSize[0]=ui->imgHeight->value();
    imageSize[1]=ui->imgWidth->value();
    imageSize[2]= (int)ui->SaveProportion->isChecked();
    accept();
}

void ExportToImage::on_SaveProportion_toggled(bool checked)
{
    if(checked)
    {
        ui->imgWidth->setValue( imageSize[1]);
        ui->imgHeight->setValue( imageSize[0]);
    }
}


void ExportToImage::on_buttonBox_rejected()
{
    reject();
}
