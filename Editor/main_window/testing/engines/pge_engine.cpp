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

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QProcess>
#ifdef Q_OS_MAC
#include <QStandardPaths>
#endif

#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <common_features/logger_sets.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>
#include <dev_console/devconsole.h>
#include <audio/sdl_music_player.h>

#include <networking/engine_intproc.h>

#include <mainwindow.h>

#include "pge_engine.h"

#ifdef _WIN32
#define PGE_ENGINE_EXE "pge_engine.exe"
#define PGE_ENGINE_BUNLDE PGE_ENGINE_EXE
#define PGE_ENGINE_BUNLDE_MASK PGE_ENGINE_EXE
#elif __APPLE__
#define PGE_ENGINE_BUNLDE "PGE Engine.app"
#define PGE_ENGINE_BUNLDE_MASK "PGE Engine"
#define PGE_ENGINE_EXECUTABLE "/Contents/MacOS/PGE Engine"
#define PGE_ENGINE_EXE PGE_ENGINE_BUNLDE PGE_ENGINE_EXECUTABLE
#else
#define PGE_ENGINE_EXE "pge_engine"
#define PGE_ENGINE_BUNLDE PGE_ENGINE_EXE
#define PGE_ENGINE_BUNLDE_MASK PGE_ENGINE_EXE
#endif


/*!
 * \brief Shows a notification about PGE Engine alpha-testing
 * \param parent Pointer to parent main window
 */
static void pge_engine_alphatestingNotify(MainWindow *parent)
{
    /************************Alpha-testing notify*****************************/
    QSettings cCounters(AppPathManager::settingsFile(), QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");
    cCounters.beginGroup("message-boxes");
    bool showNotice = cCounters.value("pge-engine-test-launch", true).toBool();
    if(showNotice)
    {
        QMessageBox msg(parent);
        msg.setWindowTitle(MainWindow::tr("PGE Engine testing"));
        msg.setWindowIcon(parent->windowIcon());
        QCheckBox box;
        box.setText(MainWindow::tr("Don't show this message again."));
        msg.setCheckBox(&box);
        msg.setText(MainWindow::tr("Hello! You are attempting to test a level in the PGE Engine.\n"
                                   "The PGE Engine is still at an early stage in development, and there "
                                   "are several features which are missing or do not work correctly. "
                                   "If you are making levels or episodes for the old SMBX Engine and you "
                                   "want to test them with a complete feature-set, please test them in "
                                   "SMBX directly. Use PGE Testing for cases when you want to test the "
                                   "PGE Engine itself or you want to test levels with PGE-specific features.")
                   );
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setWindowModality(Qt::WindowModal);
        msg.exec();
        showNotice = !box.isChecked();
    }
    cCounters.setValue("pge-engine-test-launch", showNotice);
    cCounters.endGroup();
    /************************Alpha-testing notify*****************************/
}

/*!
 * \brief Find the engine application in the available paths list
 * \param enginePath found engine application
 * \return true if engine application has been found, false if engine application wasn't found
 */
static bool findEngineApp(QString &enginePath)
{
    QStringList possiblePaths;
    possiblePaths.push_back(ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE);
#ifdef Q_OS_MAC
    {
        //! Because of path randomizer thing since macOS Sierra, we are must detect it by absolute path
        QString app = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, "PGE Project/" PGE_ENGINE_BUNLDE, QStandardPaths::LocateDirectory);
        if(!app.isEmpty())
            possiblePaths.push_back(app + QStringLiteral(PGE_ENGINE_EXECUTABLE));
    }
#endif
    //! Reserve path in case Engine is not available in the default path
    if(!GlobalSettings::testing.enginePath.isEmpty())
        possiblePaths.push_back(GlobalSettings::testing.enginePath);

    for(QString &f : possiblePaths)
    {
        if(QFile::exists(f))
        {
            enginePath = f;
            return true;
        }
    }
    return false;
}

static bool findEngine(MainWindow *parent, QString &command)
{
    while(!findEngineApp(command))
    {
        QMessageBox::warning(parent, MainWindow::tr("Engine is not found"),
                             MainWindow::tr("Can't start testing, engine is not found: \n%1\n"
                                            "Please, choose the engine application yourself!").arg(PGE_ENGINE_BUNLDE),
                             QMessageBox::Ok);
        command = QFileDialog::getOpenFileName(parent,
                                               MainWindow::tr("Choose the Engine application"),
                                               GlobalSettings::testing.enginePath,
                                               QString("PGE Engine executable (%1);;All files(*.*)")
                                               .arg(PGE_ENGINE_BUNLDE_MASK));
        if(command.isEmpty())
            return false;
        GlobalSettings::testing.enginePath = command;
    }
    return true;
}


PgeEngine::PgeEngine(QObject *parent) :
    AbstractRuntimeEngine(parent)
{}

PgeEngine::~PgeEngine()
{}

void PgeEngine::init()
{
    MainWindow *w = qobject_cast<MainWindow*>(parent());

    Q_ASSERT(w);
    if(!w)
    {
        LogCritical("Can't run test: MainWindow parent is null!");
        return;
    }

    m_w = w;

    QObject::connect(&engine_proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(testFinished()));
}

void PgeEngine::unInit()
{
    this->terminate();
}

void PgeEngine::initMenu(QMenu *destmenu)
{
    size_t menuItemId = 0;
    QAction *RunLevelTest;
    {
        RunLevelTest = destmenu->addAction("runTesting");
        QObject::connect(RunLevelTest,   &QAction::triggered,
                    this,               &PgeEngine::startTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelTest;
    }

    QAction *RunLevelSafeTest;
    {
        RunLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(RunLevelSafeTest,   &QAction::triggered,
                    this,               &PgeEngine::startSafeTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelSafeTest;
    }

    {
        destmenu->addSeparator();
        QAction *startGame = destmenu->addAction("startLegacyEngine");
        QObject::connect(startGame,   &QAction::triggered,
                    this,             &PgeEngine::startGameAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = startGame;
    }

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &PgeEngine::retranslateMenu);
}

void PgeEngine::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test level/world",
                                "Run the testing of current file in PGE Engine via interprocessing tunnel."));
    }
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test saved level/world",
                                "Run the testing of current file in PGE Engine from disk."));
    }
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Start Game",
                                "Launch PGE Engine as a normal game"));
    }
}

void PgeEngine::startTestAction()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        doTestLevelIPC(edit->LvlData);
    }
    else if(m_w->activeChildWindow() == MainWindow::WND_World)
    {
        QMessageBox::warning(m_w,
                             tr("Unsupported yet"),
                             tr("Currently this is not supported for world maps yet. Please use 'Test saved level/world' action tu run a world map test."));
    }
}

void PgeEngine::startSafeTestAction()
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

void PgeEngine::startGameAction()
{
    runNormalGame();
}

bool PgeEngine::doTestLevelIPC(const LevelData &d)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    pge_engine_alphatestingNotify(m_w);

    QString command;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker)

    if(!findEngine(m_w, command))
        return false;

    IntEngine::init(&engine_proc);

    LevelData data = d;

    //if(activeLvlEditWin()->isUntitled) return;
    LevelEdit *edit = m_w->activeLvlEditWin();
    if(!edit)
        return false;

    QStringList args;
    args << "--debug";
    args << "--config=\"" + m_w->configs.config_dir + "\"";
    args << "--interprocessing";//activeLvlEditWin()->curFile;

    SETTINGS_TestSettings t = GlobalSettings::testing;
    args << QString("--num-players=%1").arg(t.numOfPlayers);
    args << QString("--p1c=%1").arg(t.p1_char);
    args << QString("--p1s=%1").arg(t.p1_state);
    args << QString("--p2c=%1").arg(t.p2_char);
    args << QString("--p2s=%1").arg(t.p2_state);
    if(t.xtra_chuck) args << "--debug-chucknorris";
    if(t.xtra_god) args << "--debug-pagan-god";
    if(t.xtra_flyup) args << "--debug-superman";
    if(t.xtra_worldfreedom) args << "--debug-worldfreedom";
    if(t.xtra_physdebug) args << "--debug-physics";
    if(t.xtra_debug)
        args << "--debug-print=yes";
    else
        args << "--debug-print=no";

    edit->prepareLevelFile(data);
    IntEngine::setTestLvlBuffer(data);

    qDebug() << "Executing engine..." << command;
    engine_proc.start(command, args);
    if(engine_proc.waitForStarted())
    {
        qDebug() << "Started";
        testStarted();
        return true;
    }
    else
    {
        m_errorString = "Failed to start PGE Engine!" + command + "with args" + args.join(" ");
        return false;
    }
}

bool PgeEngine::doTestLevelFile(const QString &levelFile)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    pge_engine_alphatestingNotify(m_w);

    QString command = ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker)

    if(!findEngine(m_w, command))
        return false;

    QStringList args;
    args << "--debug";
    args << "--config=\"" + m_w->configs.config_dir + "\"";

    SETTINGS_TestSettings t = GlobalSettings::testing;
    args << QString("--num-players=%1").arg(t.numOfPlayers);
    args << QString("--p1c=%1").arg(t.p1_char);
    args << QString("--p1s=%1").arg(t.p1_state);
    args << QString("--p2c=%1").arg(t.p2_char);
    args << QString("--p2s=%1").arg(t.p2_state);
    if(t.xtra_chuck) args << "--debug-chucknorris";
    if(t.xtra_god) args << "--debug-pagan-god";
    if(t.xtra_flyup) args << "--debug-superman";
    if(t.xtra_worldfreedom) args << "--debug-worldfreedom";
    if(t.xtra_physdebug) args << "--debug-physics";
    if(t.xtra_debug)
        args << "--debug-print=yes";
    else
        args << "--debug-print=no";

    args << levelFile;

    engine_proc.start(command, args);
    if(engine_proc.waitForStarted())
    {
        testStarted();
        return true;
    }

    return false;
}

bool PgeEngine::doTestWorldFile(const QString &worldFile)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    pge_engine_alphatestingNotify(m_w);

    QString command = ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker)

    if(!findEngine(m_w, command))
        return false;

    QStringList args;
    args << "--debug";
    args << "--config=\"" + m_w->configs.config_dir + "\"";

    SETTINGS_TestSettings t = GlobalSettings::testing;
    args << QString("--num-players=%1").arg(t.numOfPlayers);
    args << QString("--p1c=%1").arg(t.p1_char);
    args << QString("--p1s=%1").arg(t.p1_state);
    args << QString("--p2c=%1").arg(t.p2_char);
    args << QString("--p2s=%1").arg(t.p2_state);
    if(t.xtra_chuck) args << "--debug-chucknorris";
    if(t.xtra_god) args << "--debug-pagan-god";
    if(t.xtra_flyup) args << "--debug-superman";
    if(t.xtra_worldfreedom) args << "--debug-worldfreedom";
    if(t.xtra_physdebug) args << "--debug-physics";
    if(t.xtra_debug)
        args << "--debug-print=yes";
    else
        args << "--debug-print=no";

    args << worldFile;

    engine_proc.start(command, args);
    if(engine_proc.waitForStarted())
    {
        testStarted();
        return true;
    }

    return false;
}

bool PgeEngine::runNormalGame()
{
    Q_ASSERT(m_w);
    QString command = ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker)

    if(!findEngine(m_w, command))
        return false;

    QStringList args;
    args << "--config=\"" + m_w->configs.config_dir + "\"";

    QProcess::startDetached(command, args);

    //Stop music playback in the PGE Editor!
    m_w->setMusicButton(false);
    m_w->on_actionPlayMusic_triggered(false);

    return true;
}

void PgeEngine::terminate()
{
    engine_proc.terminate();
    engine_proc.close();
}

bool PgeEngine::isRunning()
{
    return (engine_proc.state() == QProcess::Running);
}

int PgeEngine::capabilities()
{
    return  CAP_LEVEL_IPC |
            CAP_LEVEL_FILE |
            CAP_WORLD_FILE |
            CAP_RUN_GAME |
            CAP_HAS_MENU;
}

void PgeEngine::testStarted()
{
    Q_ASSERT(m_w);
    //Stop music playback in the PGE Editor!
    m_w->stopMusicForTesting();
}

void PgeEngine::testFinished()
{
    Q_ASSERT(m_w);
    m_w->testingFinished();
}
