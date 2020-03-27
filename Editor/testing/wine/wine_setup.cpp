/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wine_setup.h"
#include "ui_wine_setup.h"

wine_setup::wine_setup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wine_setup)
{
    ui->setupUi(this);
#ifdef __APPLE__
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnMac"));
#else
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnLinux"));
#endif
}

wine_setup::~wine_setup()
{
    delete ui;
}

void wine_setup::fetchPlayOnLinux()
{

}

QProcessEnvironment wine_setup::getEnv(const QString &profile)
{
    // TODO: Make data importing from a config
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("WINEDIR", "");
    env.insert("LANG", "en_US.UTF-8");
    QString winePath = env.value("WINEDIR");
    if(!winePath.isEmpty())
    {
        env.insert("LD_LIBRARY_PATH",
                   QString("%1/lib:%2").arg(env.value("WINEDIR")).
                   arg(env.value("LD_LIBRARY_PATH")));
    }
    env.insert("WINEPREFIX", QString("%1/.wine").arg(env.value("HOME")));
    env.insert("PATH", QString("%1/bin:%2/drive_c/windows:%3")
                        .arg(env.value("WINEDIR"))
                        .arg(env.value("WINEPREFIX"))
                        .arg(env.value("PATH"))
    );
    env.insert("WINEDLLPATH", QString("%1/lib/wine").arg(env.value("WINEDIR")));
    env.insert("WINELOADER", QString("%1/bin/wine64").arg(env.value("WINEDIR")));
    env.insert("WINESERVER", QString("%1/bin/wine64").arg(env.value("wineserver")));
    env.insert("WINEDLLOVERRIDES", "");

    return env;
}
