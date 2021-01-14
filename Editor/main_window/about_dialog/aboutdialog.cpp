/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA &&
       QSysInfo::WindowsVersion <= QSysInfo::WV_WINDOWS7)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
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

    QString aboutText = QString("<div style=\"width: 100%; text-align: center;\">"
        "<p><img src=\":/images/cat_builder/cat_builder_128_logo.png\" /></p>"

        "<b>%1</b><br>"
        "<u>%2</u><br>"
        "<i>%3</i><br>"
        "<i>%4</i><br>"
        "%5<br>"
        "<br>"
        "%6:<br>"
        "<a href=\"http://wohlsoft.ru/PGE/\">http://wohlsoft.ru/PGE/</a><br>"
        "<br>"
        "%7"
        "</div>")
        .arg("Moondust")
        .arg(tr("By Wohlstand"))
        .arg(tr("Editor, version %1").arg(V_FILE_VERSION V_FILE_RELEASE))
        .arg(tr("Architecture: %1").arg(FILE_CPU))
        .arg(QString("<b>%1:</b> %2-%3, <b>%4:</b> <u>%5</u><br/>"
                     "<b>Qt:</b> %6, <b>SDL2:</b> %7.%8.%9, <b>SDL Mixer X:</b> %10.%11.%12")
             .arg(tr("Revision"))
             .arg(V_BUILD_VER)
             .arg(V_BUILD_BRANCH)
             .arg(tr("Build date"))
             .arg(V_DATE_OF_BUILD)
             .arg(qVersion())
             .arg(sdlVer.major).arg(sdlVer.minor).arg(sdlVer.patch)
             .arg(mixerXVer->major).arg(mixerXVer->minor).arg(mixerXVer->patch)
            )
        .arg(tr("Our project site"))
        .arg(tr("This program is distributed under %1").arg("<a href=\"http://www.gnu.org/licenses/gpl.html\">GNU GPLv3</a>"));

/*
    <p align="center"><img src=":/images/cat_builder/cat_builder_128_logo.png" /></p>

    <b>Platformer Game Engine</b><br>
    <u>By Wohlstand</u><br>
    <i>Editor, version %1%2</i>
    <i>Architecture: %3</i>
    <i>%4</i>

    Our project site:
    <a href="http://wohlsoft.ru/PGE/">http://wohlsoft.ru/PGE/</a>

    This program is distributed under <a href="http://www.gnu.org/licenses/gpl.html">GNU GPLv3</a>.
*/

    ui->About1->setText(aboutText);

    QFile mFile(":/credits.html");

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
        return;

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
