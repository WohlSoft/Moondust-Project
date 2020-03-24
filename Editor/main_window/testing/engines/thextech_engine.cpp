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

#include <networking/engine_intproc.h>

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

        doTestLevelFile(edit->curFile);
    }
}

void TheXTechEngine::chooseEnginePath()
{
    QDialog d(m_w);
    d.setWindowTitle(tr("TheXTech path select"));
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
    AbstractRuntimeEngine(parent)
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
    interface.init(&m_engineProc);

    loadSetup();
}

void TheXTechEngine::unInit()
{
    this->terminate();
    interface.quit();
    saveSetup();
}

void TheXTechEngine::initMenu(QMenu *destmenu)
{
    size_t menuItemId = 0;
    QAction *RunLevelTest;
    {
        RunLevelTest = destmenu->addAction("runTesting");
        QObject::connect(RunLevelTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelTest;
    }

    QAction *RunLevelSafeTest;
    {
        RunLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(RunLevelSafeTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startSafeTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLevelSafeTest;
    }

    {
        destmenu->addSeparator();
        QAction *choosEnginePath = destmenu->addAction("changePath");
        QObject::connect(choosEnginePath,   &QAction::triggered,
                    this,                   &TheXTechEngine::chooseEnginePath,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosEnginePath;
    }
    {
        destmenu->addSeparator();
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
        RunLunaTest->setText(tr("Test saved level",
                                "Run the testing of current file in TheXTech from disk."));
    }
    {
        QAction *chooseEnginePath = m_menuItems[menuItemId++];
        chooseEnginePath->setText(tr("Select TheXTech path...",
                                    "Select a path to TheXTech for use."));
        chooseEnginePath->setToolTip(tr("Select a path to TheXTech for use."));
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
    args << "--magic-hand";

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

    edit->prepareLevelFile(data);
    interface.setTestLvlBuffer(data);

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
