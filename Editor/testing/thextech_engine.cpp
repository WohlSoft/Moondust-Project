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

#include "thextech_engine.h"

#include <mainwindow.h>
#include <common_features/app_path.h>


#ifdef _WIN32
#define THEXTECH_EXE "thextech.exe"
#define PGE_ENGINE_BUNLDE THEXTECH_EXE
#define PGE_ENGINE_BUNLDE_MASK THEXTECH_EXE
#elif __APPLE__
#define PGE_ENGINE_BUNLDE "TheXTech.app"
#define PGE_ENGINE_BUNLDE_MASK "TheXTech"
#define THEXTECH_EXECUTABLE "/Contents/MacOS/thextech"
#define THEXTECH_EXE PGE_ENGINE_BUNLDE THEXTECH_EXECUTABLE
#else
#define THEXTECH_EXE "thextech"
#define PGE_ENGINE_BUNLDE THEXTECH_EXE
#define PGE_ENGINE_BUNLDE_MASK THEXTECH_EXE
#endif


QString TheXTechEngine::getEnginePath()
{
    return m_customEnginePath.isEmpty() ?
                ApplicationPath + "/" + THEXTECH_EXE :
                m_customEnginePath;
}

void TheXTechEngine::loadSetup()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("TheXTech");
    {
        m_customEnginePath = settings.value("custom-runtime-path", QString()).toString();
        m_enableMagicHand = settings.value("enable-magic-hand", true).toBool();
        m_renderType = settings.value("render-type", -1).toInt();
    }
    settings.endGroup();
}

void TheXTechEngine::saveSetup()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("TheXTech");
    {
        settings.setValue("custom-runtime-path", m_customEnginePath);
        settings.setValue("enable-magic-hand", m_enableMagicHand);
        settings.setValue("render-type", m_renderType);
    }
    settings.endGroup();
}


void TheXTechEngine::startTestAction()
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

void TheXTechEngine::startSafeTestAction()
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
}

void TheXTechEngine::startBattleTestAction()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        m_battleMode = true;
        doTestLevelIPC(edit->LvlData);
    }
}

void TheXTechEngine::chooseEnginePath()
{
    QDialog d(m_w);
    d.setWindowTitle(tr("Path to TheXTech", "Title of dialog"));
    d.setModal(true);

    QGridLayout *g = new QGridLayout(&d);
    d.setLayout(g);

    QGroupBox *gr = new QGroupBox(&d);
    gr->setTitle(tr("Please select a path to TheXTech executable:"));
    g->addWidget(gr, 0, 0, 1, 2);
    QGridLayout *gd = new QGridLayout(gr);
    gr->setLayout(gd);

    g->setColumnStretch(0, 1000);
    g->setColumnStretch(1, 0);

    QRadioButton *useDefault = new QRadioButton(gr);
    useDefault->setText(tr("Use default", "Using default TheXTech path, specified by an applcation path of Editor"));

    QRadioButton *useCustom = new QRadioButton(gr);
    useCustom->setText(tr("Custom", "Using a user selected TheXTech path"));
    QLineEdit *c = new QLineEdit(gr);
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
        c->setText(ApplicationPath + "/" + THEXTECH_EXE);
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
        QString p = QFileDialog::getOpenFileName(&d, tr("Select a path to TheXTech executable", "File dialog title"), c->text());
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

void TheXTechEngine::startGameAction()
{
    runNormalGame();
}

TheXTechEngine::TheXTechEngine(QObject *parent) :
    AbstractRuntimeEngine(parent),
    m_interface(parent)
{}

TheXTechEngine::~TheXTechEngine()
{}

void TheXTechEngine::init()
{
    MainWindow *w = qobject_cast<MainWindow*>(parent());

    Q_ASSERT(w);
    if(!w)
    {
        LogCritical("Can't run test: MainWindow parent is null!");
        return;
    }

    m_w = w;

    QObject::connect(&m_engineProc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(testFinished()));
    m_interface.init(&m_engineProc);

    loadSetup();
}

void TheXTechEngine::unInit()
{
    this->terminate();
    m_interface.quit();
    saveSetup();
}

void TheXTechEngine::initMenu(QMenu *destmenu)
{
    size_t menuItemId = 0;
    {
        QAction *RunLevelTest = destmenu->addAction("runTesting");
        QObject::connect(RunLevelTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelTest;
    }
    {
        QAction *RunBattleLevelTest = destmenu->addAction("runBattleTest");
        QObject::connect(RunBattleLevelTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startBattleTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunBattleLevelTest;
    }
    {
        QAction *RunLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(RunLevelSafeTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startSafeTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelSafeTest;
    }

    destmenu->addSeparator();

    {
        QMenu *renderType = destmenu->addMenu("renderType");
        m_menuItems[menuItemId++] = renderType->menuAction();

        QAction *r_d, *r_s, *r_a, *r_v;

        r_d = renderType->addAction("renderDefault");
        r_d->setCheckable(true);
        r_d->setChecked(m_renderType == -1);
        m_menuItems[menuItemId++] = r_d;

        r_s = renderType->addAction("renderSoftware");
        r_s->setCheckable(true);
        r_s->setChecked(m_renderType == 0);
        m_menuItems[menuItemId++] = r_s;

        r_a = renderType->addAction("renderAccelerated");
        r_a->setCheckable(true);
        r_a->setChecked(m_renderType == 1);
        m_menuItems[menuItemId++] = r_a;

        r_v = renderType->addAction("renderVSync");
        r_v->setCheckable(true);
        r_v->setChecked(m_renderType == 2);
        m_menuItems[menuItemId++] = r_v;

        QObject::connect(r_d,   &QAction::triggered,
                    [this, r_d, r_s, r_a, r_v](bool)
        {
            r_d->setChecked(true);
            r_s->setChecked(false);
            r_a->setChecked(false);
            r_v->setChecked(false);
            m_renderType = -1;
        });
        QObject::connect(r_s,   &QAction::triggered,
                    [this, r_d, r_s, r_a, r_v](bool)
        {
            r_d->setChecked(false);
            r_s->setChecked(true);
            r_a->setChecked(false);
            r_v->setChecked(false);
            m_renderType = 0;
        });
        QObject::connect(r_a,   &QAction::triggered,
                    [this, r_d, r_s, r_a, r_v](bool)
        {
            r_d->setChecked(false);
            r_s->setChecked(false);
            r_a->setChecked(true);
            r_v->setChecked(false);
            m_renderType = 1;
        });
        QObject::connect(r_v,   &QAction::triggered,
                    [this, r_d, r_s, r_a, r_v](bool)
        {
            r_d->setChecked(false);
            r_s->setChecked(false);
            r_a->setChecked(true);
            r_v->setChecked(false);
            m_renderType = 2;
        });
    }

    {
        QAction *enableMagicHand;
        enableMagicHand = destmenu->addAction("enableMagicHand");
        enableMagicHand->setCheckable(true);
        enableMagicHand->setChecked(m_enableMagicHand);
        QObject::connect(enableMagicHand,   &QAction::toggled,
                    [this](bool state)
        {
            m_enableMagicHand = state;
        });
        m_menuItems[menuItemId++] = enableMagicHand;
    }
    {
        QAction *enableMaxFps;
        enableMaxFps = destmenu->addAction("enableMaxFps");
        enableMaxFps->setCheckable(true);
        enableMaxFps->setChecked(m_enableMaxFps);
        QObject::connect(enableMaxFps,   &QAction::toggled,
                    [this](bool state)
        {
            m_enableMaxFps = state;
        });
        m_menuItems[menuItemId++] = enableMaxFps;
    }
    {
        QAction *enableGrabAll;
        enableGrabAll = destmenu->addAction("enableGrabAll");
        enableGrabAll->setCheckable(true);
        enableGrabAll->setChecked(m_enableGrabAll);
        QObject::connect(enableGrabAll,   &QAction::toggled,
                    [this](bool state)
        {
            m_enableGrabAll = state;
        });
        m_menuItems[menuItemId++] = enableGrabAll;
    }

    destmenu->addSeparator();

    {
        QAction *choosEnginePath = destmenu->addAction("changePath");
        QObject::connect(choosEnginePath,   &QAction::triggered,
                    this,                   &TheXTechEngine::chooseEnginePath,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosEnginePath;
    }

    destmenu->addSeparator();

    {
        QAction *startGame = destmenu->addAction("startLegacyEngine");
        QObject::connect(startGame,   &QAction::triggered,
                    this,             &TheXTechEngine::startGameAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = startGame;
    }

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &TheXTechEngine::retranslateMenu);
}

void TheXTechEngine::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test level",
                                "Run the testing of current file in TheXTech via interprocessing tunnel."));
    }
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test level in battle mode",
                                "Run a battle testing of current file in TheXTech via interprocessing tunnel."));
    }
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test saved level",
                                "Run the testing of current file in TheXTech from disk."));
    }

    {
        QAction *renderType = m_menuItems[menuItemId++];
        renderType->setText(tr("Graphics type",
                               "Choose a rendering system: software or accelerated"));
        //  Sub-menu
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Default",
                                   "Automatically selected rendering engine"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Software",
                                   "Software rendering"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Accelerated",
                                   "Hardware accelerated rendering"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Accelerated with V-Sync",
                                   "Hardware accelerated rendering with vertical synchronization support"));
        }
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
        QAction *enableMagicHand = m_menuItems[menuItemId++];
        enableMagicHand->setText(tr("Enable max FPS",
                                    "When running non-vsync, run game with a maximum possible frame-rate"));
        enableMagicHand->setToolTip(tr("When playing a game without V-Sync, run a game with a maximum possible frame-rate."));
    }
    {
        QAction *enableMagicHand = m_menuItems[menuItemId++];
        enableMagicHand->setText(tr("Enable grab all",
                                    "Allow player to grab absolutely any NPCs in a game."));
        enableMagicHand->setToolTip(tr("Allow player to grab any NPCs in a game."));
    }



    {
        QAction *chooseEnginePath = m_menuItems[menuItemId++];
        chooseEnginePath->setText(tr("Change the path to TheXTech...",
                                     "Select the path to TheXTech executable."));
        chooseEnginePath->setToolTip(tr("Select the path to TheXTech executable."));
    }
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Start Game",
                                "Launch TheXTech as a normal game"));
    }
}

bool TheXTechEngine::doTestLevelIPC(const LevelData &d)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    QString command = getEnginePath();

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

    LevelData data = d;

    //if(activeLvlEditWin()->isUntitled) return;
    LevelEdit *edit = m_w->activeLvlEditWin();
    if(!edit)
        return false;

    QStringList args;
//    args << "--debug";
//    args << "--config=\"" + m_w->configs.config_dir + "\"";
    args << "--interprocessing";//activeLvlEditWin()->curFile;
    if(m_enableMagicHand) args << "--magic-hand";
    if(m_battleMode) args << "--battle";

    args << "--never-pause";

    SETTINGS_TestSettings t = GlobalSettings::testing;
    args << "--num-players" << QString::number(t.numOfPlayers);
    args << "--player1" << QString("c%1;s%2;m%3;t%4")
                                    .arg(t.p1_char)
                                    .arg(t.p1_state)
                                    .arg(t.p1_vehicleID)
                                    .arg(t.p1_vehicleType);
    args << "--player2" << QString("c%1;s%2;m%3;t%4")
                                    .arg(t.p2_char)
                                    .arg(t.p2_state)
                                    .arg(t.p2_vehicleID)
                                    .arg(t.p2_vehicleType);

    if(t.xtra_god) args << "--god-mode";
    if(t.xtra_showFPS) args << "--show-fps";
    if(m_enableMaxFps) args << "--max-fps";
    if(m_enableGrabAll) args << "--grab-all";

    if(m_renderType >= 0)
    {
        switch(m_renderType)
        {
        case 0:
            args << "--render" << "sw";
            break;
        case 1:
            args << "--render" << "hw";
            break;
        case 2:
            args << "--render" << "vsync";
            break;
        }
    }

    edit->prepareLevelFile(data);
    m_interface.setTestLvlBuffer(data);

    qDebug() << "Executing engine..." << command;
    m_engineProc.start(command, args);
    if(m_engineProc.waitForStarted())
    {
        qDebug() << "Started";
        testStarted();
        return true;
    }
    else
    {
        m_errorString = "Failed to start TheXTech!" + command + "with args" + args.join(" ");
        qWarning() << m_errorString;
        return false;
    }
}

bool TheXTechEngine::doTestLevelFile(const QString &levelFile)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    QString command = getEnginePath();

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

    QStringList args;
//    args << "--debug";
//    args << "--config=\"" + m_w->configs.config_dir + "\"";

    SETTINGS_TestSettings t = GlobalSettings::testing;
    args << "--num-players" << QString::number(t.numOfPlayers);
    args << "--player1" << QString("c%1;s%2;m%3;t%4")
                                    .arg(t.p1_char)
                                    .arg(t.p1_state)
                                    .arg(t.p1_vehicleID)
                                    .arg(t.p1_vehicleType);
    args << "--player2" << QString("c%1;s%2;m%3;t%4")
                                    .arg(t.p2_char)
                                    .arg(t.p2_state)
                                    .arg(t.p2_vehicleID)
                                    .arg(t.p2_vehicleType);

    if(t.xtra_god) args << "--god-mode";
    if(t.xtra_showFPS) args << "--show-fps";
    if(m_enableMaxFps) args << "--max-fps";
    if(m_enableGrabAll) args << "--grab-all";

    if(m_renderType >= 0)
    {
        switch(m_renderType)
        {
        case 0:
            args << "--render" << "sw";
            break;
        case 1:
            args << "--render" << "hw";
            break;
        case 2:
            args << "--render" << "vsync";
            break;
        }
    }

    args << "-l" << levelFile;

    m_engineProc.start(command, args);
    if(m_engineProc.waitForStarted())
    {
        testStarted();
        return true;
    }
    else
    {
        m_errorString = "Failed to start TheXTech!" + command + "with args" + args.join(" ");
        return false;
    }
}

bool TheXTechEngine::runNormalGame()
{
    Q_ASSERT(m_w);
    QString command = getEnginePath();

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

//    QStringList args;
//    args << "--config=\"" + m_w->configs.config_dir + "\"";

    QProcess::startDetached(command/*, args*/);

    //Stop music playback in the PGE Editor!
    m_w->setMusicButton(false);
    m_w->on_actionPlayMusic_triggered(false);

    return true;
}

void TheXTechEngine::terminate()
{
    m_engineProc.terminate();
    m_engineProc.close();
}

bool TheXTechEngine::isRunning()
{
    return (m_engineProc.state() == QProcess::Running);
}

int TheXTechEngine::capabilities()
{
    return  CAP_LEVEL_IPC |
            CAP_LEVEL_FILE |
            CAP_RUN_GAME |
            CAP_HAS_MENU;
}

void TheXTechEngine::testStarted()
{
    Q_ASSERT(m_w);
    //Stop music playback in the PGE Editor!
    m_w->stopMusicForTesting();
}

void TheXTechEngine::testFinished()
{
    Q_ASSERT(m_w);
    m_w->testingFinished();
}
