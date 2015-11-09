/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "aboutdialog.h"
#include <ui_aboutdialog.h>
#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif
#include <QFile>
#include <QTextStream>

#include "../../version.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QSysInfo::WindowsVersion>=QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1,-1,-1, -1);
            QtWin::enableBlurBehindWindow(this);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }
    #endif

    ui->About1->setText(ui->About1->text()
                        .arg(_FILE_VERSION)
                        .arg(_FILE_RELEASE)
                        .arg(QString("Revision: %1, Build date: <u>%2</u>")
                             .arg(_BUILD_VER)
                             .arg(_DATE_OF_BUILD))
                        );

    QFile mFile(":/credits.html");
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        return;
    }

    QTextStream in(&mFile);
    in.setCodec("UTF-8");
    QString mText = in.readAll();
    ui->About2->setText(mText);
    mFile.close();
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::on_pushButton_clicked()
{
    this->close();
}
