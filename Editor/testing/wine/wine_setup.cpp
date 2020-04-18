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

#ifdef __APPLE__
static const QStringList g_wineSearchPaths =
{
    "/Applications/Wine Development.app/Content/Resources/wine/bin",
    "/Applications/Wine Stable.app/Content/Resources/wine/bin",
    "/usr/local/bin"
};
#else
static const QStringList g_wineSearchPaths; // Leave blank
#endif

WineSetup::WineSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WineSetup)
{
    ui->setupUi(this);

#if QT_VERSION_CHECK(5, 6, 0)
    QObject::connect(&m_wineTestProc, &QProcess::errorOccurred,
                     this, &WineSetup::testErrorOccurred);
#endif
    QObject::connect(&m_wineTestProc, &QProcess::started,
                     this, &WineSetup::testStarted);
    QObject::connect(&m_wineTestProc,
                     static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &WineSetup::testFinished);
    QObject::connect(&m_wineTestProc, &QProcess::stateChanged,
                     this, &WineSetup::testStateChanged);
    QObject::connect(&m_wineTestProc, &QProcess::readyReadStandardError,
                     this, &WineSetup::testReadyReadStandardError);
    QObject::connect(&m_wineTestProc, &QProcess::readyReadStandardOutput,
                     this, &WineSetup::testReadyReadStandardOutput);

#ifdef __APPLE__
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnMac"));
#else
    ui->importFromPlay->setTitle(tr("Import setup from PlayOnLinux"));
#endif
    auto wineExPath = QStandardPaths::findExecutable("wine", g_wineSearchPaths);
    auto winePathExPath = QStandardPaths::findExecutable("winepath", g_wineSearchPaths);
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
        if(!homePrefix.isEmpty())
            ui->winePrefixPath->setText(homePrefix);
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
#ifdef __APPLE__
#define POL_WINE_DIR    "darwin"
    auto polHome = home + "/Library/PlayOnMac";
#else
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

        const QString cfgFile = p + "/playonlinux.cfg";

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
        profile.wineRoot = wineDirs.absolutePath() + ("/" POL_WINE_DIR "-") + arch + "/" + version;
        QDir wineVer(profile.wineRoot);
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


WineSetupData WineSetup::getSetup()
{
    WineSetupData setup;

    setup.useCustom = ui->pathCustom->isChecked();
    setup.wineRoot = ui->wineRootPath->text();

    setup.useCustomEnv = ui->wineCustomEnvGroup->isChecked();
    setup.useWinePrefix = ui->winePrefix->isChecked();
    setup.winePrefix = ui->winePrefixPath->text();

    setup.enableWineDebug = ui->wineDebug->isChecked();

    prepareSetup(setup);

    return setup;
}

void WineSetup::setSetup(const WineSetupData &setup)
{
    ui->pathCustom->setChecked(setup.useCustom);
    ui->pathDefault->setChecked(!setup.useCustom);

    if(!setup.wineRoot.isEmpty())
        ui->wineRootPath->setText(setup.wineRoot);

    ui->wineCustomEnvGroup->setChecked(setup.useCustomEnv);
    ui->winePrefix->setChecked(setup.useWinePrefix);
    if(!setup.winePrefix.isEmpty())
        ui->winePrefixPath->setText(setup.winePrefix);

    ui->wineDebug->setChecked(setup.enableWineDebug);
}

void WineSetup::prepareSetup(WineSetupData &setup)
{
    if(!setup.useCustom)
    {
        // Detect system-wide
        setup.metaWineExec = QStandardPaths::findExecutable("wine", g_wineSearchPaths);
        QFileInfo f(setup.metaWineExec);
        auto d = f.absoluteDir();
        setup.metaWineBinDir = d.absolutePath();
        d.cdUp();

        setup.metaWinePrefix = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.wine";

        if(d.exists("lib64"))
            setup.metaWineLib64Dir = d.absoluteFilePath("lib64");

        if(d.exists("lib32"))
            setup.metaWineLibDir = d.absoluteFilePath("lib32");
        else
            setup.metaWineLibDir = d.absoluteFilePath("lib");
    }
    else
    {
        // Use custom
        setup.metaWinePrefix = setup.winePrefix;
        setup.metaWineBinDir = setup.wineRoot + "/bin";
        setup.metaWineLibDir = setup.wineRoot + "/lib";
        setup.metaWineLib64Dir = setup.wineRoot + "/lib64";
    }

    if(!QDir(setup.metaWineLibDir + "/wine").exists())
    {
        if(QDir(setup.wineRoot + "/lib32/wine").exists())
            setup.metaWineLibDir = setup.wineRoot + "/lib32";
    }

    if(!QDir(setup.metaWineLib64Dir + "/wine").exists())
        setup.metaWineLib64Dir.clear();

    setup.metaWineExec = setup.metaWineBinDir + "/wine";
    setup.metaWine64Exec = setup.metaWineBinDir + "/wine";
    setup.metaWineDllDir = setup.metaWineLibDir + "/wine";
}

QProcessEnvironment WineSetup::buildEnv(const WineSetupData &profile)
{
    WineSetupData setup = profile;

    prepareSetup(setup);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if(!setup.enableWineDebug)
        env.insert("WINEDEBUG", "-all");

    if(!setup.useCustom)
        return env; // Use system-default

    env.insert("WINEDIR", setup.wineRoot);

    QString winePath = setup.wineRoot;
    if(!winePath.isEmpty())
    {
        if(!setup.metaWineLib64Dir.isEmpty())
            env.insert("LD_LIBRARY_PATH",
                       QString("%1:%2:%3")
                       .arg(setup.metaWineLib64Dir)
                       .arg(setup.metaWineLibDir)
                       .arg(env.value("LD_LIBRARY_PATH")));
        else
            env.insert("LD_LIBRARY_PATH",
                       QString("%1:%2")
                       .arg(setup.metaWineLibDir)
                       .arg(env.value("LD_LIBRARY_PATH")));
    }

    if(setup.useWinePrefix)
    {
        env.insert("WINEPREFIX", setup.winePrefix);
        env.insert("PATH", QString("%1/bin:%2/drive_c/windows:%3")
                            .arg(winePath)
                            .arg(setup.winePrefix)
                            .arg(env.value("PATH"))
        );
    }
    else
    {
        env.insert("PATH", QString("%1/bin:%2")
                            .arg(winePath)
                            .arg(env.value("PATH"))
        );
    }

    env.insert("WINEDLLPATH", setup.metaWineDllDir);
    env.insert("WINELOADER", setup.metaWine64Exec);
    env.insert("WINESERVER", setup.metaWine64Exec);
//    env.insert("WINEDLLOVERRIDES", "");

    return env;
}

void WineSetup::iniLoad(QSettings &settings, WineSetupData &setup)
{
    setup.useCustom = settings.value("wine-custom", false).toBool();
    setup.wineRoot = settings.value("wine-root", QString()).toString();
    setup.useCustomEnv = settings.value("wine-custom-env", false).toBool();
    setup.useWinePrefix = settings.value("wine-use-prefix", false).toBool();
    setup.winePrefix = settings.value("wine-prefix", QString()).toString();
    setup.enableWineDebug = settings.value("wine-debug", false).toBool();
}

void WineSetup::iniSave(QSettings &settings, WineSetupData &setup)
{
    settings.setValue("wine-custom", setup.useCustom);
    settings.setValue("wine-root", setup.wineRoot);
    settings.setValue("wine-custom-env", setup.useCustomEnv);
    settings.setValue("wine-use-prefix", setup.useWinePrefix);
    settings.setValue("wine-prefix", setup.winePrefix);
    settings.setValue("wine-debug", setup.enableWineDebug);
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

void WineSetup::on_doImportFromPoL_clicked()
{
    int i = ui->playOnLinuxDrive->currentData().toInt();
    if(i < 0 || i >= m_polProfiles.size())
        return; // nothing to do

    const auto &p = m_polProfiles[i];
    ui->pathCustom->setChecked(true);
    ui->wineRootPath->setText(p.wineRoot);

    ui->wineCustomEnvGroup->setChecked(true);
    ui->winePrefix->setChecked(true);
    ui->winePrefixPath->setText(p.winePrefix);
}

static void printEnv(QPlainTextEdit *e, QProcessEnvironment &env)
{
    auto sl = env.toStringList();
    e->appendPlainText("Environment:\n");
    for(auto &s : sl)
    {
        if(!s.startsWith("WINE") && !s.startsWith("LD_LIBRARY_PATH") && !s.startsWith("PATH"))
            continue;
        e->appendPlainText(QString("%1").arg(s));
    }
    e->appendPlainText("\n");
}

void WineSetup::on_runWineCfg_clicked()
{
    auto setup = getSetup();
    auto env = buildEnv(setup);

    QString wineCfgPath = QStandardPaths::findExecutable("winecfg", {setup.metaWineBinDir});

    ui->testLog->clear();
    printEnv(ui->testLog, env);
    m_wineTestProc.setProgram(wineCfgPath);
    m_wineTestProc.setProcessEnvironment(env);
    m_wineTestProc.setArguments({});
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
    auto setup = getSetup();
    auto env = buildEnv(setup);

    ui->testLog->clear();
    printEnv(ui->testLog, env);
    m_wineTestProc.setProgram(setup.metaWineExec);
    m_wineTestProc.setProcessEnvironment(env);
    m_wineTestProc.setArguments({"winver"});
    m_wineTestProc.start();
    if(!m_wineTestProc.waitForStarted())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Can't start \"%1\" because of: %2")
                             .arg(setup.metaWineExec)
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


#if QT_VERSION_CHECK(5, 6, 0)
void WineSetup::testErrorOccurred(QProcess::ProcessError error)
{
    switch(error)
    {
    case QProcess::FailedToStart:
        ui->testLog->appendPlainText(QString("\nFailed to start: %1").arg(m_wineTestProc.errorString()));
        break;
    case QProcess::Crashed:
        ui->testLog->appendPlainText(QString("\nCrashed: %1").arg(m_wineTestProc.errorString()));
        break;
    case QProcess::Timedout:
        ui->testLog->appendPlainText(QString("\nTimed out: %1").arg(m_wineTestProc.errorString()));
        break;
    case QProcess::WriteError:
        ui->testLog->appendPlainText(QString("\nWrite error: %1").arg(m_wineTestProc.errorString()));
        break;
    case QProcess::ReadError:
        ui->testLog->appendPlainText(QString("\nRead error: %1").arg(m_wineTestProc.errorString()));
        break;
    default:
    case QProcess::UnknownError:
        ui->testLog->appendPlainText(QString("\nUnknown error: %1").arg(m_wineTestProc.errorString()));
        break;
    }
}
#endif

void WineSetup::testStarted()
{
    ui->testLog->appendPlainText(QString("\nStarted\n"));
}

void WineSetup::testFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::NormalExit)
        ui->testLog->appendPlainText(QString("\nExited with %1 error code").arg(exitCode));
    else
        ui->testLog->appendPlainText(QString("\nCrashed with %1 error code").arg(exitCode));
}

void WineSetup::testStateChanged(QProcess::ProcessState newState)
{
    switch(newState)
    {
    case QProcess::NotRunning:
        ui->runWineCfg->setEnabled(true);
        ui->runWineCmd->setEnabled(true);
        ui->testState->setText(tr("Not running", "State of a test app"));
        ui->testState->setStyleSheet(QString());
        break;
    case QProcess::Starting:
        ui->runWineCfg->setEnabled(false);
        ui->runWineCmd->setEnabled(false);
        ui->testState->setText(tr("Starting...", "State of a test app"));
        ui->testState->setStyleSheet("color: red;");
        break;
    case QProcess::Running:
        ui->runWineCfg->setEnabled(false);
        ui->runWineCmd->setEnabled(false);
        ui->testState->setText(tr("Running...", "State of a test app"));
        ui->testState->setStyleSheet("color: green;");
        break;
    }
}

void WineSetup::testReadyReadStandardError()
{
    QByteArray in = m_wineTestProc.readAllStandardError();
    ui->testLog->appendPlainText(in);
}

void WineSetup::testReadyReadStandardOutput()
{
    QByteArray in = m_wineTestProc.readAllStandardOutput();
    ui->testLog->appendPlainText(in);
}
