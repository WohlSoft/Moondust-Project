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

#include <QDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardPaths>

#if !defined(_WIN32)
#include <unistd.h>
#include "wine/wine_setup.h"
#else
#include <windows.h>
#endif

#include "38a_engine.h"
#include "38a_symlinks.h"

#include <mainwindow.h>
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/pge_file_lib_private.h>
#include <common_features/app_path.h>
#include <dev_console/devconsole.h>

#define SMBX38A_EXE "smbx.exe"


SanBaEiRuntimeEngine::SanBaEiRuntimeEngine(QObject *parent) :
    AbstractRuntimeEngine(parent)
{}

SanBaEiRuntimeEngine::~SanBaEiRuntimeEngine()
{
    terminateAll();
    removeTempDir();
}

void SanBaEiRuntimeEngine::init()
{
    MainWindow *w = qobject_cast<MainWindow*>(parent());

    Q_ASSERT(w);
    if(!w)
    {
        LogCritical("Can't run test: MainWindow parent is null!");
        return;
    }

    m_w = w;

    QObject::connect(&m_testingProc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SIGNAL(testFinished()));

    QObject::connect(&m_testingProc, &QProcess::started,
                     this, &SanBaEiRuntimeEngine::gameStarted);
    QObject::connect(&m_testingProc,
                     static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &SanBaEiRuntimeEngine::gameFinished);

    QObject::connect(&m_testingProc, &QProcess::readyReadStandardError,
                     this, &SanBaEiRuntimeEngine::testReadyReadStandardError);
    QObject::connect(&m_testingProc, &QProcess::readyReadStandardOutput,
                     this, &SanBaEiRuntimeEngine::testReadyReadStandardOutput);

    QObject::connect(&m_gameProc, &QProcess::started,
                     this, &SanBaEiRuntimeEngine::gameStarted);
    QObject::connect(&m_gameProc,
                     static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &SanBaEiRuntimeEngine::gameFinished);

    QObject::connect(&m_gameProc, &QProcess::readyReadStandardError,
                     this, &SanBaEiRuntimeEngine::gameReadyReadStandardError);
    QObject::connect(&m_gameProc, &QProcess::readyReadStandardOutput,
                     this, &SanBaEiRuntimeEngine::gameReadyReadStandardOutput);

    QObject::connect(this, &SanBaEiRuntimeEngine::testStarted,
                     m_w, &MainWindow::stopMusicForTesting);
    QObject::connect(this, &SanBaEiRuntimeEngine::testFinished,
                     m_w, &MainWindow::testingFinished);

    m_interface.init(&m_testingProc);
    loadSetup();
}

void SanBaEiRuntimeEngine::unInit()
{
    terminate();
    saveSetup();
}

void SanBaEiRuntimeEngine::initMenu(QMenu *destmenu)
{
    size_t menuItemId = 0;
    QAction *runLevelTest;
    {
        runLevelTest = destmenu->addAction("runTesting");
        QObject::connect(runLevelTest,   &QAction::triggered,
                    this,               &SanBaEiRuntimeEngine::actionStartTest,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = runLevelTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
        });
    }

    {
        QAction *RunBattleLevelTest = destmenu->addAction("runBattleTest");
        QObject::connect(RunBattleLevelTest,   &QAction::triggered,
                    this,               &SanBaEiRuntimeEngine::actionStartBattleTest,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunBattleLevelTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
        });
    }

    QAction *runLevelSafeTest;
    {
        runLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(runLevelSafeTest,   &QAction::triggered,
                    this,               &SanBaEiRuntimeEngine::actionStartSafeTest,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = runLevelSafeTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_FILE));
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_FILE));
        });
    }

    destmenu->addSeparator();

    QAction *resetCheckPoints = destmenu->addAction("resetCheckpoints");
    {
        QObject::connect(resetCheckPoints,   &QAction::triggered,
                    this,               &SanBaEiRuntimeEngine::actionResetCheckPoints,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = resetCheckPoints;
    }

    destmenu->addSeparator();

    {
        QAction *enableMagicHand;
        enableMagicHand = destmenu->addAction("enableMagicHand");
        enableMagicHand->setCheckable(true);
        enableMagicHand->setChecked(m_interface.m_enableMagicHand);
        QObject::connect(enableMagicHand,   &QAction::toggled,
                    [this](bool state)
        {
            m_interface.setMagicHand(state);
        });
        m_menuItems[menuItemId++] = enableMagicHand;
    }
    {
        QAction *noSuspend;
        noSuspend = destmenu->addAction("don't suspent while unfocused");
        noSuspend->setCheckable(true);
        noSuspend->setChecked(m_interface.m_noSuspend);
        QObject::connect(noSuspend,   &QAction::toggled,
                    [this](bool state)
        {
            m_interface.setNoSuspend(state);
        });
        m_menuItems[menuItemId++] = noSuspend;
    }

    destmenu->addSeparator();

    QAction *choosEnginePath;
    {
        choosEnginePath = destmenu->addAction("change38aPath");
        QObject::connect(choosEnginePath,   &QAction::triggered,
                    this,                   &SanBaEiRuntimeEngine::actionChooseEnginePath,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosEnginePath;
    }

#ifndef _WIN32
    destmenu->addSeparator();
    {
        QAction *wineSetup;
        wineSetup = destmenu->addAction("wineSetup38a");
        QObject::connect(wineSetup,   &QAction::triggered,
                    this,                   &SanBaEiRuntimeEngine::actionRunWineSetup,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = wineSetup;
    }
#endif

    {
        destmenu->addSeparator();
        QAction *startGame = destmenu->addAction("start38aEngine");
        QObject::connect(startGame,   &QAction::triggered,
                    this,             &SanBaEiRuntimeEngine::actionStartGame,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = startGame;
    }

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &SanBaEiRuntimeEngine::retranslateMenu);

    if(ConfStatus::defaultTestEngine == ConfStatus::ENGINE_38A)
    {
        resetCheckPoints->setShortcut(QStringLiteral("Ctrl+F5"));
        resetCheckPoints->setShortcutContext(Qt::WindowShortcut);
    }
}

void SanBaEiRuntimeEngine::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *run38aTest = m_menuItems[menuItemId++];
        run38aTest->setText(tr("Test level",
                                "Run the testing of current file in SMBX-38A via interprocessing tunnel."));
    }
    {
        QAction *run38aTest = m_menuItems[menuItemId++];
        run38aTest->setText(tr("Test level in battle mode",
                                "Run a battle testing of current file in SMBX-38A via interprocessing tunnel."));
    }
    {
        QAction *run38aTest = m_menuItems[menuItemId++];
        run38aTest->setText(tr("Test saved level/world",
                                "Run the testing of current file in SMBX-38A from disk."));
    }


    {
        QAction *resetCheckPoints = m_menuItems[menuItemId++];
        resetCheckPoints->setText(tr("Reset checkpoints"));
        resetCheckPoints->setToolTip(tr("Reset all checkpoint states to initial state."));
    }


    {
        QAction *enableMagicHand = m_menuItems[menuItemId++];
        enableMagicHand->setText(tr("Enable magic hand",
                                    "Allow real-time picking-up of elements while playing a level test."));
        enableMagicHand->setToolTip(tr("Allows real-time editing: picking-up elements from a level scene, "
                                       "placing new elements, selected at back in the editor, and erasing. "
                                       "Doesn't works when running a test of a saved file."));
    }
    {
        QAction *noAutoSuspend = m_menuItems[menuItemId++];
        noAutoSuspend->setText(tr("Don't auto-suspend game",
                                    "Don't pause game when it's window is unfocused"));
        noAutoSuspend->setToolTip(tr("Game will always run and it will never suspend while window is unfocused."));
    }
    {
        QAction *chooseEnginePath = m_menuItems[menuItemId++];
        chooseEnginePath->setText(tr("Change the path to SMBX-38A...",
                                     "Select the path to SMBX-38A executable."));
        chooseEnginePath->setToolTip(tr("Select the path to SMBX-38A executable."));
    }

#ifndef _WIN32
    {
        QAction *runWineSetup = m_menuItems[menuItemId++];
        runWineSetup->setText(tr("Wine settings...",
                                 "Open Wine settings to choose which Wine toolchain use"));
        runWineSetup->setToolTip(tr("Select a Wine toolchain for use."));
    }
#endif

    {
        QAction *runLunaTest = m_menuItems[menuItemId++];
        runLunaTest->setText(tr("Start Game",
                                "Launch SMBX-38A as a normal game"));
    }
}

void SanBaEiRuntimeEngine::gameStarted()
{
    emit testStarted();
}

void SanBaEiRuntimeEngine::gameFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit testFinished();
    LogDebug(QString("SMBX-38A: finished with Exit Code %1 and status %2").arg(exitCode).arg(exitStatus));
    if(m_interface.isBridgeWorking())
        m_interface.terminateBridge();
}

void SanBaEiRuntimeEngine::testReadyReadStandardError()
{
    QString err = m_testingProc.readAllStandardError();
    DevConsole::log(err, "WineDebug");
}

void SanBaEiRuntimeEngine::testReadyReadStandardOutput()
{
    QString err = m_testingProc.readAllStandardOutput();
    DevConsole::log(err, "WineDebug");
}

void SanBaEiRuntimeEngine::gameReadyReadStandardError()
{
    QString err = m_gameProc.readAllStandardError();
    DevConsole::log(err, "WineDebug");
}

void SanBaEiRuntimeEngine::gameReadyReadStandardOutput()
{
    QString err = m_gameProc.readAllStandardOutput();
    DevConsole::log(err, "WineDebug");
}

QString SanBaEiRuntimeEngine::getEnginePath()
{
    return m_customEnginePath.isEmpty() ?
                ApplicationPath + "/" + SMBX38A_EXE :
                m_customEnginePath;
}

QString SanBaEiRuntimeEngine::getBridgePath()
{
#ifdef Q_OS_MAC
    // on macOS an IPC bridge is shipped inside of a bundle
    QDir exePath(QApplication::applicationDirPath());
    return exePath.absoluteFilePath("../Resources/ipc/38a_ipc_bridge.exe");
#else
    return ApplicationPath + "/ipc/38a_ipc_bridge.exe";
#endif
}

void SanBaEiRuntimeEngine::loadSetup()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("38aTester");
    {
        m_customEnginePath = settings.value("custom-runtime-path", QString()).toString();
        m_interface.m_enableMagicHand = settings.value("enable-magic-hand", true).toBool();
        m_interface.m_noSuspend = settings.value("no-pause-while-unfocused", true).toBool();
#ifndef _WIN32
        WineSetup::iniLoad(settings, m_wineSetup);
        WineSetup::prepareSetup(m_wineSetup);
#endif
    }
    settings.endGroup();
}

void SanBaEiRuntimeEngine::saveSetup()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("38aTester");
    {
        settings.setValue("custom-runtime-path", m_customEnginePath);
        settings.value("enable-magic-hand", m_interface.m_enableMagicHand);
        settings.value("no-pause-while-unfocused", m_interface.m_noSuspend);
#ifndef _WIN32
        WineSetup::iniSave(settings, m_wineSetup);
#endif
    }
    settings.endGroup();
}

void SanBaEiRuntimeEngine::terminateAll()
{
    terminate();
    m_gameProc.terminate();
    m_gameProc.waitForFinished(3000);
}


#ifndef _WIN32

void SanBaEiRuntimeEngine::useWine(QProcess &proc, QString &command, QStringList &args)
{
    args.push_front(command);
    auto setup = m_wineSetup;
    WineSetup::prepareSetup(setup);
    command = setup.metaWineExec;
    auto env = WineSetup::buildEnv(setup);
    proc.setProcessEnvironment(env);
}

QString SanBaEiRuntimeEngine::pathUnixToWine(const QString &unixPath)
{
    auto setup = m_wineSetup;
    WineSetup::prepareSetup(setup);
    auto env = WineSetup::buildEnv(setup);

    QStringList args;
    QString command = QStandardPaths::findExecutable("winepath", {setup.metaWineBinDir});

    QProcess winePathExec;
    // Ask for in-Wine Windows path from in-UNIX native path
    args << "--windows" << unixPath;
    // Use wine custom environment
    winePathExec.setProcessEnvironment(env);

    // Start winepath
    winePathExec.start(command, args);
    winePathExec.waitForFinished();
    // Retrieve converted path
    QString windowsPath = winePathExec.readAllStandardOutput();
    return windowsPath.trimmed();
}

#else // _WIN32

void SanBaEiRuntimeEngine::useWine(QProcess &, QString &, QStringList &) // Dummy
{}

QString SanBaEiRuntimeEngine::pathUnixToWine(const QString &unixPath)
{
    // dummy, no need on real Windows
    return unixPath;
}

#endif //_WIN32


void SanBaEiRuntimeEngine::actionStartTest()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        m_battleMode = false;
        doTestLevelIPC(edit->LvlData);
    }
}

void SanBaEiRuntimeEngine::actionStartBattleTest()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        if(edit->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(m_w);
            return;
        }

        m_battleMode = true;
        doTestLevelIPC(edit->LvlData);
    }
}

void SanBaEiRuntimeEngine::actionStartSafeTest()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        if(edit->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(m_w);
            return;
        }

        m_battleMode = false;
        doTestLevelFile(edit->curFile);
    }
    else if(m_w->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = m_w->activeWldEditWin();
        if(!edit)
            return;

        if(edit->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(m_w);
            return;
        }

        doTestWorldFile(edit->curFile);
    }
}

void SanBaEiRuntimeEngine::actionResetCheckPoints()
{
    m_interface.m_lastGameState.reset();

    QMessageBox::information(m_w,
                             "SMBX-38A",
                             tr("Checkpoints successfully reseted!"),
                             QMessageBox::Ok);
}

void SanBaEiRuntimeEngine::actionStartGame()
{
    runNormalGame();
}

void SanBaEiRuntimeEngine::actionChooseEnginePath()
{
    QDialog d(m_w);
    d.setWindowTitle(tr("Path to SMBX-38A", "Title of dialog"));
    d.setModal(true);

    QGridLayout *g = new QGridLayout(&d);
    d.setLayout(g);

    QGroupBox *gr = new QGroupBox(&d);
    gr->setTitle(tr("Please select a path to SMBX-38A executable:"));
    g->addWidget(gr, 0, 0, 1, 2);
    QGridLayout *gd = new QGridLayout(gr);
    gr->setLayout(gd);

    g->setColumnStretch(0, 1000);
    g->setColumnStretch(1, 0);

    QRadioButton *useDefault = new QRadioButton(gr);
    useDefault->setText(tr("Use default", "Using default SMBX-38A path, specified by an applcation path of Editor"));

    QRadioButton *useCustom = new QRadioButton(gr);
    useCustom->setText(tr("Custom", "Using a user selected SMBX-38A path"));
    QLineEdit *c = new QLineEdit(gr);
    c->setMinimumWidth(400);
    QPushButton *br = new QPushButton(gr);
    br->setText(tr("Browse..."));

    gd->addWidget(useDefault, 0, 0, 1, 3);
    gd->addWidget(useCustom, 1, 0);
    gd->addWidget(c, 1, 1);
    gd->addWidget(br, 1, 2);

    gd->setColumnStretch(0, 0);
    gd->setColumnStretch(1, 1000);
    gd->setColumnStretch(2, 0);

    if(m_customEnginePath.isEmpty())
    {
        useDefault->setChecked(true);
        c->setText(ApplicationPath + "/" + SMBX38A_EXE);
        c->setEnabled(false);
    }
    else
    {
        useCustom->setChecked(true);
        c->setText(m_customEnginePath);
    }

    QPushButton *save = new QPushButton(&d);
    save->setText(tr("Save"));
    g->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);
    g->addWidget(save, 1, 1);

    save->setFocus(Qt::TabFocusReason);

    QObject::connect(useCustom, &QRadioButton::toggled, c, &QLineEdit::setEnabled);
    QObject::connect(useCustom, &QRadioButton::toggled, br, &QPushButton::setEnabled);

    QObject::connect(save, &QPushButton::clicked, &d, &QDialog::accept);
    QObject::connect(br, &QPushButton::clicked,
                     [&d, c](bool)->void
    {
        QString p = QFileDialog::getOpenFileName(&d,
                                                 tr("Select a path to SMBX-38A executable",
                                                    "File dialog title"), c->text(),
                                                    "SMBX-38A executable (smbx.exe);;"
                                                    "All Windows executables (*.exe);;"
                                                    "All files(*.*)");
        if(!p.isEmpty())
            c->setText(p);
    });

    int ret = d.exec();

    if(ret == QDialog::Accepted)
    {
        if(useCustom->isChecked())
            m_customEnginePath = c->text();
        else
            m_customEnginePath.clear();
    }
}

#ifndef _WIN32
void SanBaEiRuntimeEngine::actionRunWineSetup()
{
    if(isRunning() || (m_gameProc.state() == QProcess::Running))
    {
        int ret = QMessageBox::question(m_w,
                                        tr("SMBX-38A is still active"),
                                        tr("To change a setup of Wine, you will need to shut down a currently working SMBX-38A. "
                                           "Do you want to shut down the SMBX-38A now?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return; // Do nothing
        terminate();
    }

    WineSetup d(m_w);
    d.setModal(true);
    d.setSetup(m_wineSetup);
    int ret = d.exec();
    if(ret == QDialog::Accepted)
    {
        m_wineSetup = d.getSetup();
        WineSetup::prepareSetup(m_wineSetup);
    }
}
#endif

QStringList SanBaEiRuntimeEngine::getTestingArgs()
{
    QStringList params;

    SETTINGS_TestSettings t = GlobalSettings::testing;
    if(m_battleMode)
        params << "2";
    else if(t.numOfPlayers == 1)
        params << "0";
    else if(t.numOfPlayers >= 2)
        params << "1";

    params << QString::number(t.p1_char);
    params << QString::number(t.p2_char);

    int p1mount = 0;
    int p2mount = 0;

    if(t.p1_vehicleID == 1)
        p1mount = -t.p1_vehicleType;
    if(t.p1_vehicleID == 3)
        p1mount = t.p1_vehicleType;

    if(t.p2_vehicleID == 1)
        p2mount = -t.p2_vehicleType;
    if(t.p2_vehicleID == 3)
        p2mount = t.p2_vehicleType;

    QString smbxArgsStr = "SMBXArgs|";
    smbxArgsStr += QString::number(m_interface.m_lastGameState.hp);
    smbxArgsStr += "," + QString::number(m_interface.m_lastGameState.co);
    smbxArgsStr += "," + QString::number(m_interface.m_lastGameState.sr);
    smbxArgsStr += "|";

    smbxArgsStr += QString::number(t.p1_state);
    smbxArgsStr += "," + QString::number(p1mount);
    smbxArgsStr += "," + QString::number(t.p2_state);
    smbxArgsStr += "," + QString::number(p2mount);
    smbxArgsStr += "|";

    smbxArgsStr += PGE_FileFormats_misc::url_encode(m_interface.m_lastGameState.levelName);
    smbxArgsStr += "," + QString::number(m_interface.m_lastGameState.cid);
    smbxArgsStr += "," + QString::number(m_interface.m_lastGameState.id);

    params << PGE_FileFormats_misc::url_encode(smbxArgsStr);
    return params;
}

QString SanBaEiRuntimeEngine::prepareTempDir()
{
    if(m_tempPath.isEmpty())
    {
#ifndef _WIN32
        m_tempPath = m_wineSetup.metaWinePrefix + "/drive_c/windows/temp";
        QDir t(m_tempPath);
        if(!t.exists())
            t.mkpath(m_tempPath);
#else
        m_tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
#endif
    }

    removeTempDir(); // Delete old temp path
    QDir tmp(m_tempPath);
    tmp.mkdir("38a-lvl-test");
    tmp.cd("38a-lvl-test");

    return tmp.absolutePath();
}

void SanBaEiRuntimeEngine::removeTempDir()
{
    if(!m_tempPath.isEmpty())
    {
        QDir tmp(m_tempPath);
        if(tmp.exists("38a-lvl-test"))
        {
            tmp.cd("38a-lvl-test");
            removeAllLinks(tmp);
            tmp.cdUp();
        }
    }
}

QString SanBaEiRuntimeEngine::initTempLevel(const LevelData &d)
{
    QString tempPath;
    QString origPath;
    if(!d.meta.path.isEmpty())
        origPath = d.meta.path;

    tempPath = prepareTempDir();
    if(tempPath.isEmpty() || !QFile::exists(tempPath))
        return QString(); // Failed to prepare a temp path

    QString levelFile = tempPath + "/___smbx38a-temp-level.lvl";
    LogDebug(QString("Orig Path %1 -> %2").arg(origPath).arg(levelFile));

    if(!origPath.isEmpty())
    {
        /* FIXME: WORKAROUND: reproduce an episode directory of a testing level by a
         * symbolic links to every file in the original episode folder.
         * Remove this cludge once SMBX-38A wil get a proper ability to specify
         * the location of episode and level custom resources set. */
        QDir op(origPath);
        auto entries = op.entryList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
        for(auto &i : entries)
        {
            auto from = origPath + "/" + i;
            auto to = tempPath + "/" + i;
            symLink(from, to);
        }

        auto cfFrom = origPath + "/" + d.meta.filename;
        auto cfTo = tempPath + "/___smbx38a-temp-level";
        if(QFile::exists(cfFrom))
            symLink(cfFrom, cfTo);
    }

    LevelData lvl = d;
    if(!FileFormats::WriteSMBX38ALvlFileF(levelFile, lvl))
        return QString(); // Failed to write a level file

    return levelFile;
}

static void msgNotFound(QWidget *parent, const QString &what)
{
    QMessageBox::warning(parent,
                         SanBaEiRuntimeEngine::tr("Executable not found"),
                         SanBaEiRuntimeEngine::tr("Can't start SMBX-38A game because \"%1\" is not found. "
                                                  "That might happen because of incorrect path to SMBX-38A executable was "
                                                  "specified, please check the SMBX-38A path setup.").arg(what),
                         QMessageBox::Ok);
}

bool SanBaEiRuntimeEngine::doTestLevelIPC(const LevelData &d)
{
    const QString smbxExe = getEnginePath();
    QFileInfo smbxExeInfo(smbxExe);
    const QString smbxPath = smbxExeInfo.absoluteDir().absolutePath();

    if(!QFile::exists(smbxExe))
    {
        msgNotFound(m_w, smbxExe);
        return false;
    }

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

    QString levelFile = initTempLevel(d);
    if(levelFile.isEmpty())
    {
        QMessageBox::critical(m_w,
                              "SMBX-38A",
                              tr("Impossible to prepare a temp file for a test run."),
                              QMessageBox::Ok);
        return false;
    }

    if(!m_interface.isBridgeWorking()) // Initialize bridge
    {
        QString cmd = getBridgePath();
        if(!QFile::exists(cmd))
        {
            msgNotFound(m_w, cmd);
            return false;
        }

        QStringList params;
        if(QFile::exists(cmd))
        {
            auto &b = m_interface.m_bridge;
            useWine(b, cmd, params);
            b.setProgram(cmd);
            b.setArguments(params);
            b.setWorkingDirectory(smbxPath);
            b.start();
            b.waitForStarted();
        }
    }

    QString command = smbxExe;
    QStringList params;
    params << pathUnixToWine(levelFile);
    params << getTestingArgs();

    useWine(m_testingProc, command, params);
    m_testingProc.setProgram(command);
    m_testingProc.setArguments(params);
    m_testingProc.setWorkingDirectory(smbxPath);
    m_testingProc.start();
    LogDebug(QString("SMBX-38A: starting command: %1 %2").arg(command).arg(params.join(' ')));

    return true;
}

bool SanBaEiRuntimeEngine::doTestLevelFile(const QString &levelFile)
{
    const QString smbxExe = getEnginePath();
    QFileInfo smbxExeInfo(smbxExe);
    const QString smbxPath = smbxExeInfo.absoluteDir().absolutePath();

    if(!QFile::exists(smbxExe))
    {
        msgNotFound(m_w, smbxExe);
        return false;
    }

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

    LevelData lvl;
    if(!FileFormats::OpenLevelFileHeader(levelFile, lvl))
    {
        QMessageBox::critical(m_w,
                              "SMBX-38A",
                              tr("Impossible to launch a level because of an invalid file."),
                              QMessageBox::Ok);
        return false;
    }

    if((lvl.meta.RecentFormat != LevelData::SMBX38A) &&
       (lvl.meta.RecentFormat != LevelData::SMBX64))
    {
        QMessageBox::critical(m_w,
                              "SMBX-38A",
                              tr("Cannot launch the level because the level file is saved in an unsupported format. "
                                 "Please save the level in the SMBX-38A or the SMBX64-LVL format."),
                              QMessageBox::Ok);
        return false;
    }

    if(lvl.meta.RecentFormat == WorldData::SMBX64)
    {
        int ret = QMessageBox::warning(m_w,
                                       tr("Caution"),
                                       tr("Your level is not in SMBX-38A format. That means, the game WILL automatically convert it into SMBX-38A format. "
                                          "Your level will become incompatible with a Classic SMBX. Do you want to continue on your own risk?"),
                                       QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return false;
    }

    if(m_interface.isBridgeWorking())
        m_interface.terminateBridge();

    QString command = smbxExe;
    QStringList params;
    params << pathUnixToWine(levelFile);
    params << getTestingArgs();

    useWine(m_testingProc, command, params);
    m_testingProc.setProgram(command);
    m_testingProc.setArguments(params);
    m_testingProc.setWorkingDirectory(smbxPath);
    m_testingProc.start();
    LogDebug(QString("SMBX-38A: starting command: %1 %2").arg(command).arg(params.join(' ')));

    return true;
}

bool SanBaEiRuntimeEngine::doTestWorldFile(const QString &worldFile)
{
    const QString smbxExe = getEnginePath();
    QFileInfo smbxExeInfo(smbxExe);
    const QString smbxPath = smbxExeInfo.absoluteDir().absolutePath();

    if(!QFile::exists(smbxExe))
    {
        msgNotFound(m_w, smbxExe);
        return false;
    }

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

    WorldData wld;
    if(!FileFormats::OpenWorldFileHeader(worldFile, wld))
    {
        QMessageBox::critical(m_w,
                              "SMBX-38A",
                              tr("Impossible to launch an episode because of an invalid file."),
                              QMessageBox::Ok);
        return false;
    }

    if((wld.meta.RecentFormat != WorldData::SMBX38A) &&
       (wld.meta.RecentFormat != WorldData::SMBX64))
    {
        QMessageBox::critical(m_w,
                              "SMBX-38A",
                              tr("Cannot launch the episode because the world map file is saved in an unsupported format. "
                                 "Please save the level in the SMBX-38A or the SMBX64-WLD format."),
                              QMessageBox::Ok);
        return false;
    }

    if(wld.meta.RecentFormat == WorldData::SMBX64)
    {
        int ret = QMessageBox::warning(m_w,
                                       tr("Caution"),
                                       tr("Your world map is not in SMBX-38A format. That means, the game will automatically convert it into SMBX-38A format. "
                                          "Your episode will become incompatible with a Classic SMBX. Do you want to continue on your own risk?"),
                                       QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return false;
    }

    QString command = smbxExe;
    QStringList params;
    params << pathUnixToWine(worldFile);

    useWine(m_testingProc, command, params);
    m_testingProc.setProgram(command);
    m_testingProc.setArguments(params);
    m_testingProc.setWorkingDirectory(smbxPath);
    m_testingProc.start();
    LogDebug(QString("SMBX-38A: starting command: %1 %2").arg(command).arg(params.join(' ')));

    return true;
}

bool SanBaEiRuntimeEngine::runNormalGame()
{
    const QString smbxExe = getEnginePath();
    QFileInfo smbxExeInfo(smbxExe);
    const QString smbxPath = smbxExeInfo.absoluteDir().absolutePath();

    if(!QFile::exists(smbxExe))
    {
        msgNotFound(m_w, smbxExe);
        return false;
    }

    QString command = smbxExe;
    QStringList params;
    useWine(m_gameProc, command, params);
    m_gameProc.setProgram(command);
    m_gameProc.setArguments(params);
    m_gameProc.setWorkingDirectory(smbxPath);
    m_gameProc.start();
    LogDebug(QString("SMBX-38A: starting command: %1 %2").arg(command).arg(params.join(' ')));

    return true;
}

void SanBaEiRuntimeEngine::terminate()
{
    m_testingProc.terminate();
    m_testingProc.waitForFinished(3000);
    m_interface.terminateBridge();
}

bool SanBaEiRuntimeEngine::isRunning()
{
    return (m_testingProc.state() == QProcess::Running);
}

int SanBaEiRuntimeEngine::capabilities()
{
    return  CAP_LEVEL_IPC |
            CAP_LEVEL_FILE |
            CAP_WORLD_FILE |
            CAP_RUN_GAME |
            CAP_HAS_MENU;
}
