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

#include <QFontDatabase>

#include "itemmsgbox.h"
#include <ui_itemmsgbox.h>

ItemMsgBox::ItemMsgBox(QString text, QString label, QString title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemMsgBox)
{
    QFontDatabase::addApplicationFont(":/PressStart2P.ttf");

    currentText = text;
    ui->setupUi(this);

    if(!label.isEmpty())
        ui->NotesLabel->setText(label);

    if(!title.isEmpty())
        this->setWindowTitle(title);

    ui->msgTextBox->setFont(QFont("Press Start 2P", 11));

    ui->msgTextBox->clear();
    ui->msgTextBox->appendPlainText(currentText);
}

ItemMsgBox::~ItemMsgBox()
{
    delete ui;
}

void ItemMsgBox::on_buttonBox_accepted()
{
    currentText = ui->msgTextBox->toPlainText();
    accept();
}
