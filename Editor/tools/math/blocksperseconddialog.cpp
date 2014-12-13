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

#include <QMessageBox>

#include "blocksperseconddialog.h"
#include <ui_blocksperseconddialog.h>

BlocksPerSecondDialog::BlocksPerSecondDialog(QWidget *parent) :
    QDialog(parent),
    m_framesPerSecond(65.0),
    ui(new Ui::BlocksPerSecondDialog)
{
    ui->setupUi(this);
    ui->spinBlockSize->setValue(32);
}

BlocksPerSecondDialog::BlocksPerSecondDialog(unsigned int defBlockSize, QWidget *parent) :
    QDialog(parent),
    m_framesPerSecond(65.0),
    ui(new Ui::BlocksPerSecondDialog)
{
    ui->setupUi(this);
    ui->spinBlockSize->setValue(defBlockSize);
}

BlocksPerSecondDialog::~BlocksPerSecondDialog()
{
    delete ui;
}

void BlocksPerSecondDialog::on_buttonBox_accepted()
{
    if(ui->spinBlocks->value() == 0.0 || ui->spinSecond->value() == 0.0){
        QMessageBox::warning(this, "Error", "Seconds value or blocks value must be non-zero.");
        return;
    }
    m_result = (double)ui->spinBlockSize->value() * ui->spinBlocks->value() / ui->spinSecond->value() / m_framesPerSecond;
    this->accept();
}

void BlocksPerSecondDialog::on_buttonBox_rejected()
{
    this->reject();
}
double BlocksPerSecondDialog::framesPerSecond() const
{
    return m_framesPerSecond;
}

void BlocksPerSecondDialog::setFramesPerSecond(double framesPerSecond)
{
    if(!framesPerSecond)
        return;
    m_framesPerSecond = framesPerSecond;
}

double BlocksPerSecondDialog::result() const
{
    return m_result;
}

void BlocksPerSecondDialog::setResult(double result)
{
    m_result = result;
}

unsigned int BlocksPerSecondDialog::blockSize() const
{
    return m_blockSize;
}

void BlocksPerSecondDialog::setBlockSize(unsigned int blockSize)
{
    m_blockSize = blockSize;
}

