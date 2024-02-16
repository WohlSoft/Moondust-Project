/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QFontMetrics>
#include <QScrollBar>
#include <QStyle>

#include "itemmsgbox.h"
#include <ui_itemmsgbox.h>

ItemMsgBox::ItemMsgBox(Opened_By openedBy, QString text, bool isFriendly, QString label, QString title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemMsgBox)
{
    QFontDatabase::addApplicationFont(":/fonts/PressStart2P.ttf");

    whoOpened = openedBy;
    currentText = text;
    ui->setupUi(this);

    ui->checkFriendly->setChecked(isFriendly);

    if(whoOpened != NPC)
        ui->checkFriendly->hide();

    if(!label.isEmpty())
        ui->NotesLabel->setText(label);

    if(!title.isEmpty())
        this->setWindowTitle(title);

    // QFontMetrics meter(ui->msgTextBox->font());
    // int w_width = meter.size(Qt::TextSingleLine, "XXXXXXXXXXXXXXXXXXXXXXXXXXX").width();
    // int scrW = ui->msgTextBox->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    // ui->msgTextBox->setMaximumWidth(w_width + scrW + 18);
    // ui->msgTextBox->setMinimumWidth(w_width + scrW + 18);
    ui->previewArea->setVisible(ui->previewShow->isChecked());
    QObject::connect(ui->msgTextBox, &QPlainTextEdit::textChanged, this,
    [this]()->void
    {
        ui->previewBox->setText(ui->msgTextBox->toPlainText());
    });

    ui->msgTextBox->clear();
    ui->msgTextBox->appendPlainText(currentText);
    updateGeometry();
}

ItemMsgBox::~ItemMsgBox()
{
    delete ui;
}

bool ItemMsgBox::isFriendlyChecked()
{
    return ui->checkFriendly->isChecked();
}

void ItemMsgBox::on_buttonBox_accepted()
{
    currentText = ui->msgTextBox->toPlainText();
    accept();
}
