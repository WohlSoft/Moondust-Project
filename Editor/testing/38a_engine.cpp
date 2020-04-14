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

#if !defined(_WIN32)
#include <QStandardPaths>
#include "wine/wine_setup.h"
#endif

#include "38a_engine.h"

#include <mainwindow.h>
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/pge_file_lib_private.h>
#include <common_features/app_path.h>

#define SMBX38A_EXE "smbx.exe"


SanBaEiRuntimeEngine::SanBaEiRuntimeEngine(QObject *parent) :
    AbstractRuntimeEngine(parent)
{}

SanBaEiRuntimeEngine::~SanBaEiRuntimeEngine()
{
    terminateAll();
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
                     this, SLOT(testFinished()));
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
                    this,               &SanBaEiRuntimeEngine::startTestAction,
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

    QAction *runLevelSafeTest;
    {
        runLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(runLevelSafeTest,   &QAction::triggered,
                    this,               &SanBaEiRuntimeEngine::startSafeTestAction,
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

    QAction *choosEnginePath;
    {
        choosEnginePath = destmenu->addAction("change38aPath");
        QObject::connect(choosEnginePath,   &QAction::triggered,
                    this,                   &SanBaEiRuntimeEngine::chooseEnginePath,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosEnginePath;
    }

#ifndef _WIN32
    destmenu->addSeparator();
    {
        QAction *wineSetup;
        wineSetup = destmenu->addAction("wineSetup38a");
        QObject::connect(wineSetup,   &QAction::triggered,
                    this,                   &SanBaEiRuntimeEngine::runWineSetup,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = wineSetup;
    }
#endif

    {
        destmenu->addSeparator();
        QAction *startGame = destmenu->addAction("start38aEngine");
        QObject::connect(startGame,   &QAction::triggered,
                    this,             &SanBaEiRuntimeEngine::startGameAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = startGame;
    }

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &SanBaEiRuntimeEngine::retranslateMenu);
}

void SanBaEiRuntimeEngine::retranslateMenu()
{

}

QString SanBaEiRuntimeEngine::getEnginePath()
{
    return m_customEnginePath.isEmpty() ?
                ApplicationPath + "/" + SMBX38A_EXE :
                m_customEnginePath;
}

void SanBaEiRuntimeEngine::loadSetup()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("38aTester");
    {
        m_customEnginePath = settings.value("custom-runtime-path", QString()).toString();
#ifndef _WIN32
        WineSetup::iniLoad(settings, m_wineSetup);
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


void SanBaEiRuntimeEngine::startTestAction()
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

void SanBaEiRuntimeEngine::startSafeTestAction()
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

void SanBaEiRuntimeEngine::startGameAction()
{
    runNormalGame();
}

void SanBaEiRuntimeEngine::chooseEnginePath()
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
                                                    "File dialog title"), c->text(), "SMBX-38A executable (smbx.exe)");
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
void SanBaEiRuntimeEngine::runWineSetup()
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
        m_wineSetup = d.getSetup();
}
#endif

bool SanBaEiRuntimeEngine::doTestLevelIPC(const LevelData &d)
{

}

bool SanBaEiRuntimeEngine::doTestLevelFile(const QString &levelFile)
{
    const QString smbxExe = getEnginePath();
    QFileInfo smbxExeInfo(smbxExe);
    const QString smbxPath = smbxExeInfo.absoluteDir().absolutePath();

    LevelData lvl;
    if(!FileFormats::OpenLevelFileHeader(levelFile, lvl))
    {
        QMessageBox::warning(m_w,
                             "SMBX-38A",
                             tr("Impossible to launch a level because of an invalid file."),
                             QMessageBox::Ok);
        return false;
    }

    if((lvl.meta.RecentFormat != LevelData::SMBX38A) &&
       (lvl.meta.RecentFormat != LevelData::SMBX64))
    {
        QMessageBox::warning(m_w,
                             "SMBX-38A",
                             tr("Cannot launch the level because the level file is saved in an unsupported format. "
                                "Please save the level in the SMBX-38A or the SMBX64-LVL format."),
                             QMessageBox::Ok);
        return false;
    }

    QString command = smbxExe;
    QStringList params;

    SETTINGS_TestSettings t = GlobalSettings::testing;
    params << pathUnixToWine(levelFile);
    if(t.numOfPlayers == 1)
        params << "0";
    else if(t.numOfPlayers >= 2)
        params << "1";
    params << QString::number(t.p1_char);
    params << QString::number(t.p2_char);

    QString smbxArgsStr =
        QString("SMBXArgs|%1,%2,%3|%4,%5,%6,%7|%8,%9,%10")
            .arg(25)
            .arg(0)
            .arg(0)

            .arg(t.p1_state)
            .arg(t.p1_vehicleType == 1 ? -t.p1_vehicleID : t.p1_vehicleType == 3 ? t.p1_vehicleID : 0)
            .arg(t.p2_state)
            .arg(t.p2_vehicleType == 1 ? -t.p2_vehicleID : t.p2_vehicleType == 3 ? t.p2_vehicleID : 0)

            .arg(PGE_FileFormats_misc::url_encode(lvl.LevelName))
            .arg(0)
            .arg(0);

    params << PGE_FileFormats_misc::url_encode(smbxArgsStr);

    useWine(m_testingProc, command, params);
    m_testingProc.setProgram(command);
    m_testingProc.setArguments(params);
    m_testingProc.setWorkingDirectory(smbxPath);
    m_testingProc.start();
    LogDebug(QString("SMBX-38A: starting command: %1 %2").arg(command).arg(params.join(' ')));

    return true;
}

bool SanBaEiRuntimeEngine::doTestWorldIPC(const WorldData &d)
{

}

bool SanBaEiRuntimeEngine::doTestWorldFile(const QString &worldFile)
{

}

bool SanBaEiRuntimeEngine::runNormalGame()
{

}

void SanBaEiRuntimeEngine::terminate()
{
    m_testingProc.terminate();
    m_testingProc.waitForFinished(3000);
    m_bridgeProc.terminate();
    m_bridgeProc.waitForFinished(3000);
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

void SanBaEiRuntimeEngine::testStarted()
{

}

void SanBaEiRuntimeEngine::testFinished()
{

}
