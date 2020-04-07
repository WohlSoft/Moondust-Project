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
#include <QFileDialog>
#include <QMessageBox>

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
    auto wineExPath = QStandardPaths::findExecutable("wine");
    auto winePathExPath = QStandardPaths::findExecutable("winepath");
    auto homePrefix = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    if(!homePrefix.isEmpty())
    {
        homePrefix += "/.wine";
        if(!QDir(homePrefix).exists())
            homePrefix.clear();
    }

    if(!wineExPath.isEmpty())
    {
        QString libPrefix;
        QString lib64Prefix;
        QFileInfo wineEx(wineExPath);
        QDir dir = wineEx.absoluteDir();
        dir.cdUp();
        if(dir.exists("lib"))
            libPrefix = dir.absolutePath() + "/lib";
        if(dir.exists("lib64"))
            lib64Prefix = dir.absolutePath() + "/lib64";
        ui->wineRootPath->setText(dir.absolutePath());
        ui->wineExecPath->setText(wineExPath);
        ui->winePathExecPath->setText(winePathExPath);
        if(!homePrefix.isEmpty())
            ui->winePrefixPath->setText(homePrefix);
        if(!libPrefix.isEmpty())
            ui->wineDllPath->setText(libPrefix + "/wine");
        else if(!lib64Prefix.isEmpty())
            ui->wineDllPath->setText(lib64Prefix + "/wine");
    }
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
#define POL_WINE_DIR    "darwin"
    auto polHome = home + "/.PlayOnMac";
#else
#define POL_CONFIG_NAME "playonlinux"
#define POL_WINE_DIR    "linux"
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

        const QString cfgFile = p + "/" POL_CONFIG_NAME ".cfg";

        if(!QFile::exists(cfgFile))
            continue;

        auto c = loadCfg(cfgFile);
        auto arch = c["ARCH"];
        auto version = c["VERSION"];

        if(arch == "x86")
            profile.bits = PlayOnProfile::BITS_32;
        else if(arch == "amd64")
            profile.bits = PlayOnProfile::BITS_64;
        else
        {
            qDebug() << "invalid bitness" << arch;
            continue;
        }
        profile.winePrefix = p;
        profile.wineVersion = wineDirs.absolutePath() + ("/" POL_WINE_DIR "-") + arch + "/" + version;
        QDir wineVer(profile.wineVersion);
        if(!wineVer.exists())
        {
            qDebug() << "Invalid Wine toolchain at path:" << wineVer.absolutePath();
            continue; // Invalid toolchain
        }
        m_polProfiles.push_back(profile);
        ui->playOnLinuxDrive->addItem(profile.name, (m_polProfiles.size() - 1));
    }

    if(!m_polProfiles.isEmpty())
    {
        ui->playOnLinuxDrive->setEnabled(true);
        ui->doImportFromPoL->setEnabled(true);
    }
}

QProcessEnvironment WineSetup::getEnv(const WineSetupData &profile)
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

QProcessEnvironment WineSetup::getEnv()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if(ui->pathCustom->isChecked())
    {
        env.insert("WINEDIR", ui->wineRootPath->text());
        QString winePath = env.value("WINEDIR");
        if(!winePath.isEmpty())
        {
            auto wineDll = QString("%1/lib/wine").arg(env.value("WINEDIR"));
            auto wineEx = QString("%1/bin/wine").arg(env.value("WINEDIR"));
            auto wine64Ex = QString("%1/bin/wine64").arg(env.value("WINEDIR"));

            if(ui->wineDll->isChecked())
                wineDll = ui->wineDllPath->text();

            if(!QFile::exists(wine64Ex))
                wine64Ex = wineEx;

            env.insert("LD_LIBRARY_PATH",
                       QString("%1/lib:%1/lib64:%2").arg(env.value("WINEDIR")).
                       arg(env.value("LD_LIBRARY_PATH")));
            env.insert("WINEDLLPATH", wineDll);
            env.insert("WINELOADER", wine64Ex);
            env.insert("WINESERVER", wine64Ex);
        }

        if(ui->winePrefix->isChecked())
            env.insert("WINEPREFIX", ui->winePrefixPath->text());

        env.insert("PATH", QString("%1/bin:%2/drive_c/windows:%3")
                            .arg(env.value("WINEDIR"))
                            .arg(env.value("WINEPREFIX"))
                            .arg(env.value("PATH"))
        );
        env.insert("WINEDLLOVERRIDES", "");
    }
    return env;
}

void WineSetup::on_wineRootPathBrowse_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this,
        tr("Select a Wine install prefix path"),
        ui->wineRootPath->text());
    if(path.isEmpty())
        return;
    ui->wineRootPath->setText(path);
}

void WineSetup::on_winePrefixBrowse_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this,
        tr("Select a Wine home prefix path"),
        ui->winePrefixPath->text());
    if(path.isEmpty())
        return;
    ui->winePrefixPath->setText(path);
}

void WineSetup::on_wineExecBrowse_clicked()
{
    auto path = QFileDialog::getOpenFileName(this,
        tr("Select a Wine executable"),
        ui->wineExecPath->text(),
        "Wine executable (wine)");
    if(path.isEmpty())
        return;
    ui->wineExecPath->setText(path);
}


void WineSetup::on_winePathExecBrowse_clicked()
{
    auto path = QFileDialog::getOpenFileName(this,
        tr("Select a WinePath executable"),
        ui->winePathExecPath->text(),
        "WinePath executable (winepath)");
    if(path.isEmpty())
        return;
    ui->winePathExecPath->setText(path);
}


void WineSetup::on_wineDllBrowse_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this,
        tr("Select a Wine DLLs path"),
        ui->wineDllPath->text());
    if(path.isEmpty())
        return;
    ui->wineDllPath->setText(path);
}

void WineSetup::on_doImportFromPoL_clicked()
{
    int i = ui->playOnLinuxDrive->currentData().toInt();
    if(i < 0 || i >= m_polProfiles.size())
        return; // nothing to do

    const auto &p = m_polProfiles[i];
    ui->pathCustom->setChecked(true);
    ui->wineRootPath->setText(p.wineVersion);

    ui->winePrefix->setChecked(true);
    ui->winePrefixPath->setText(p.winePrefix);

    auto wineExec = p.wineVersion + "/bin/wine";
    auto winePathExec = p.wineVersion + "/bin/winepath";
    auto dllDir = p.wineVersion + "/lib/wine";

    ui->wineExec->setChecked(false);
    ui->wineExecPath->clear();
    ui->winePathExec->setChecked(false);
    ui->winePathExecPath->clear();

    if(QFile::exists(wineExec))
    {
        ui->wineExec->setChecked(true);
        ui->wineExecPath->setText(wineExec);
    }

    if(QFile::exists(winePathExec))
    {
        ui->winePathExec->setChecked(true);
        ui->winePathExecPath->setText(winePathExec);
    }

    if(QDir(dllDir).exists())
    {
        ui->wineDll->setChecked(true);
        ui->wineDllPath->setText(dllDir);
    }
}

void WineSetup::on_runWineCfg_clicked()
{
    QString wineCfgPath;
    if(ui->pathCustom->isChecked())
        wineCfgPath = QStandardPaths::findExecutable("winecfg", {ui->wineRootPath->text() + "/bin"});
    else
        wineCfgPath = QStandardPaths::findExecutable("winecfg");

    m_wineTestProc.setProgram(wineCfgPath);
    m_wineTestProc.setProcessEnvironment(getEnv());
    m_wineTestProc.start();
    if(!m_wineTestProc.waitForStarted())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Can't start \"%1\" because of: %2")
                             .arg(wineCfgPath)
                             .arg(m_wineTestProc.errorString()),
                             QMessageBox::Ok);
    }
}

void WineSetup::on_runWineCmd_clicked()
{
    QString wineCfgPath;
    if(ui->pathCustom->isChecked())
        wineCfgPath = QStandardPaths::findExecutable("wine", {ui->wineRootPath->text() + "/bin"});
    else
        wineCfgPath = QStandardPaths::findExecutable("wine");

    m_wineTestProc.setProgram(wineCfgPath);
    m_wineTestProc.setProcessEnvironment(getEnv());
    m_wineTestProc.setArguments({"winver"});
    m_wineTestProc.start();
    if(!m_wineTestProc.waitForStarted())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Can't start \"%1\" because of: %2")
                             .arg(wineCfgPath)
                             .arg(m_wineTestProc.errorString()),
                             QMessageBox::Ok);
    }
}

void WineSetup::on_wineStopProc_clicked()
{
    if(m_wineTestProc.state() == QProcess::Running)
    {
        m_wineTestProc.terminate();
        if(!m_wineTestProc.waitForFinished(3000))
            m_wineTestProc.kill();
    }
}
