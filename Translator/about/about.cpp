/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "about.h"
#include <ui_about.h>
#include "../version.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    ui->about1->setText(ui->about1->text()
                           .arg(V_FILE_VERSION)
                           .arg(V_FILE_RELEASE)
                           .arg(FILE_CPU)
                           .arg(QString("Revision: %1-%2, Build date: <u>%3</u>")
                                        .arg(V_BUILD_VER)
                                        .arg(V_BUILD_BRANCH)
                                        .arg(V_DATE_OF_BUILD))
                        );
}

about::~about()
{
    delete ui;
}

void about::on_pushButton_clicked()
{
    close();
}
