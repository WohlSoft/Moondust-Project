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

#include <QStandardPaths>
#include <QDirIterator>
#include <QFile>
#include <QSettings>
#include <QHash>

#include <QtDebug>


WineSetup::WineSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WineSetup)
{
    ui->setupUi(this);
#ifdef __APPLE__
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnMac"));
#else
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnLinux"));
#endif
    fetchPlayOnLinux();
}

WineSetup::~WineSetup()
{
    delete ui;
}

static QHash<QString, QString> loadCfg(const QString &path)
{
    QHash<QString, QString> out;
    QFile cfg(path);
    if(!cfg.open(QIODevice::ReadOnly|QIODevice::Text))
        return out;

    while(!cfg.atEnd())
    {
        auto l = cfg.readLine();
        auto ll = l.split('=');
        if(ll.size() != 2) // invalid line
            continue;
        out[ll[0].trimmed()] = ll[1].trimmed();
    }

    return out;
}

void WineSetup::fetchPlayOnLinux()
{
    m_polProfiles.clear();
    m_polProfiles.reserve(25);
    ui->playOnLinuxDrive->clear();
    ui->playOnLinuxDrive->setEnabled(false);
    ui->doImportFromPoL->setEnabled(false);

    auto home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#ifdef __APPLE__ // FIXME: VERIFY THIS ON MACOS
#define POL_CONFIG_NAME "/playonmac.cfg"
    auto polHome = home + "/.PlayOnMac";
#else
#define POL_CONFIG_NAME "playonlinux"
#define POL_WINE_DIR "linux"
    auto polHome = home + "/.PlayOnLinux";
#endif

    QDir polHomeDir(polHome);
    QDir prefixDirs(polHome + "/wineprefix");
    QDir wineDirs(polHome + "/wine");

    if(!polHomeDir.exists() || !prefixDirs.exists() || !wineDirs.exists())
    {
        ui->importFromPlay->setVisible(false);
        return; // nothing to find
    }

    prefixDirs.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    prefixDirs.setSorting(QDir::Name);

    QDirIterator pfxDirs(prefixDirs);
    while(pfxDirs.hasNext())
    {
        PlayOnProfile profile;
        auto p = pfxDirs.next();
        profile.name = QDir(p).dirName();
        qDebug() << p;

        if(!QFile::exists(p + "/" POL_CONFIG_NAME ".cfg"))
            continue;

        auto c = loadCfg(p + POL_CONFIG_NAME);
        auto arch = c["ARCH"];
        auto version = c["VERSION"];

        if(arch  == "x86")
            profile.bits = PlayOnProfile::BITS_32;
        else if(arch  == "amd64")
            profile.bits = PlayOnProfile::BITS_64;
        else
        {
            qDebug() << "invalid bitness" << arch;
            continue;
        }
        profile.winePrefix = p;
        profile.wineVersion = wineDirs.absolutePath() + "/linux-" + arch;
        QDir wineVer(profile.wineVersion);
        if(!wineVer.exists())
        {
            qDebug() << "Invalid Wine toolchain at path:" << wineVer.absolutePath();
            continue; // Invalid toolchain
        }
        m_polProfiles.push_back(profile);
        ui->playOnLinuxDrive->addItem(profile.name);
    }

    if(!m_polProfiles.isEmpty())
    {
        ui->playOnLinuxDrive->setEnabled(true);
        ui->doImportFromPoL->setEnabled(true);
    }
}

QProcessEnvironment WineSetup::getEnv(const QString &profile)
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

void WineSetup::on_wineRootPathBrowse_clicked()
{

}

void WineSetup::on_winePrefixBrowse_clicked()
{

}

void WineSetup::on_wineExecBrowse_clicked()
{

}

void WineSetup::on_wine64ExecBrowse_clicked()
{

}

void WineSetup::on_wineDllBrowse_clicked()
{

}

void WineSetup::on_doImportFromPoL_clicked()
{

}
