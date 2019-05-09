/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <SDL2/SDL_version.h>
#include <SDL2/SDL_mixer_ext.h>

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

    SDL_version sdlVer;
    SDL_GetVersion(&sdlVer);
    const SDL_version *mixerXVer = Mix_Linked_Version();

    ui->About1->setText(ui->About1->text()
                        .arg(V_FILE_VERSION)
                        .arg(V_FILE_RELEASE)
                        .arg(FILE_CPU)
                        .arg(QString("<b>Revision:</b> %1-%2, <b>Build date:</b> <u>%3</u><br/>"
                                     "<b>Qt:</b> %4, <b>SDL2:</b> %5.%6.%7, <b>SDL Mixer X:</b> %8.%9.%10")
                             .arg(V_BUILD_VER)
                             .arg(V_BUILD_BRANCH)
                             .arg(V_DATE_OF_BUILD)
                             .arg(qVersion())
                             .arg(sdlVer.major).arg(sdlVer.minor).arg(sdlVer.patch)
                             .arg(mixerXVer->major).arg(mixerXVer->minor).arg(mixerXVer->patch)
                             )
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
