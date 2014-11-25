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

#include "newlayerbox.h"
#include <ui_tonewlayerbox.h>
#include <QMessageBox>

ToNewLayerBox::ToNewLayerBox(LevelData * lData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToNewLayerBox)
{
    LvlData = lData;
    ui->setupUi(this);
}

ToNewLayerBox::~ToNewLayerBox()
{
    delete ui;
}

void ToNewLayerBox::closeEvent(QCloseEvent *event)
{
    reject();
    event->accept();
}

void ToNewLayerBox::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::AcceptRole)
    {
        lName = ui->layerName->text();
        lLocked = ui->Lock->isChecked();
        lHidden = ui->hide->isChecked();

        bool AlreadyExist=false;
        foreach(LevelLayers layer, LvlData->layers)
        {
            if( layer.name==lName )
            {
                AlreadyExist=true;
                break;
            }
        }
        if(AlreadyExist)
        {
            valid=false;
            QMessageBox::warning(this,tr("Layer exists"), tr("Layer \"%1\" is exist, please, set other name.").arg(lName), QMessageBox::Ok);
            return;
        }
        else
        {
            valid=true;
            QDialog::accept();
        }
    }
    else
    {
            QDialog::reject();
    }
}
