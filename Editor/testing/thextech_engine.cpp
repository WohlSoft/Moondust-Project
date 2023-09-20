/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QInputDialog>

#include "thextech_engine.h"

#include <mainwindow.h>
#include <common_features/app_path.h>

#include "qfile_dialogs_default_options.hpp"


#ifdef _WIN32
#define THEXTECH_EXE "thextech.exe"
#define PGE_ENGINE_BUNLDE THEXTECH_EXE
#define PGE_ENGINE_BUNLDE_MASK THEXTECH_EXE
#elif defined(__APPLE__)
#define PGE_ENGINE_BUNLDE "TheXTech.app"
#define PGE_ENGINE_BUNLDE_MASK "TheXTech"
#define THEXTECH_EXECUTABLE "/Contents/MacOS/thextech"
#define THEXTECH_EXE PGE_ENGINE_BUNLDE THEXTECH_EXECUTABLE
#else
#define THEXTECH_EXE "thextech"
#define PGE_ENGINE_BUNLDE THEXTECH_EXE
#define PGE_ENGINE_BUNLDE_MASK THEXTECH_EXE
#endif


static QString getDefaultEnginePath(const QString &executableName)
{
    QString pathConfig = ConfStatus::configDataPath + executableName;
    QString pathApp = ApplicationPath + "/" + executableName;

    if(QFileInfo(executableName).isAbsolute())
        pathConfig = executableName;

    if(QFile::exists(pathConfig))
        return pathConfig;
    else
        return pathApp;
}

QString TheXTechEngine::getEnginePath()
{
    return m_customEnginePath.isEmpty() ?
           getDefaultEnginePath(m_defaultEngineName) :
               m_customEnginePath;
}

void TheXTechEngine::rescanCapabilities()
{
    QString execPath = getEnginePath();

    if(!getTheXTechCapabilities(m_caps, execPath))
        qWarning() << "Failed to load capabilities for TheXTech executable" << execPath;

    updateMenuCapabilities();
}

void TheXTechEngine::updateMenuCapabilities()
{
    Q_ASSERT(m_w);

    if(m_menuRunWorldTestFile)
        m_menuRunWorldTestFile->setEnabled(m_caps.features.contains("test-world-file"));

    if(m_renderVSync)
        m_renderVSync->setVisible(!m_caps.features.contains("vsync-flag"));

    if(m_renderVSyncFlag)
        m_renderVSyncFlag->setEnabled(m_caps.features.contains("vsync-flag"));

    if(m_renderModernOpenGL)
        m_renderModernOpenGL->setVisible(m_caps.renders.contains("opengl"));

    if(m_renderLegacyOpenGL)
        m_renderLegacyOpenGL->setVisible(m_caps.renders.contains("opengl11"));

    if(m_renderModernOpenGLES)
        m_renderModernOpenGLES->setVisible(m_caps.renders.contains("opengles"));

    if(m_renderLegacyOpenGLES)
        m_renderLegacyOpenGLES->setVisible(m_caps.renders.contains("opengles11"));

    if(m_startWarpAction)
        m_startWarpAction->setEnabled(m_caps.arguments.contains("start-warp"));

    if(m_saveSlotMenu)
        m_saveSlotMenu->setEnabled(m_caps.arguments.contains("save-slot"));

    m_w->updateTestingCaps();
}

void TheXTechEngine::loadSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("main");
    {
        m_defaultEngineName = settings.value("executable-name", THEXTECH_EXE).toString();
    }
    settings.endGroup();

    settings.beginGroup("TheXTech");
    {
        m_customEnginePath = settings.value("custom-runtime-path", QString()).toString();
        m_enableMagicHand = settings.value("enable-magic-hand", true).toBool();
        m_renderType = settings.value("render-type", -1).toInt();
        m_vsyncEnable = settings.value("render-vsync", false).toBool();
        m_compatLevel = settings.value("compat-level", -1).toInt();
        m_speedRunMode = settings.value("speedrun-mode", -1).toInt();
        m_speedRunTimerST = settings.value("speedrun-st-stopwatch", false).toBool();
        m_saveSlot = settings.value("save-slot", 0).toInt();
        m_startWarp = settings.value("start-warp", 0).toInt();
    }
    settings.endGroup();

    rescanCapabilities();
}

void TheXTechEngine::saveSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("TheXTech");
    {
        settings.setValue("custom-runtime-path", m_customEnginePath);
        settings.setValue("enable-magic-hand", m_enableMagicHand);
        settings.setValue("render-type", m_renderType);
        settings.setValue("render-vsync", m_vsyncEnable);
        settings.setValue("compat-level", m_compatLevel);
        settings.setValue("speedrun-mode", m_speedRunMode);
        settings.setValue("speedrun-st-stopwatch", m_speedRunTimerST);
        settings.setValue("save-slot", m_saveSlot);
        settings.setValue("start-warp", m_startWarp);
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

        m_battleMode = false;
        doTestWorldFile(edit->curFile);
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
        c->setText(getDefaultEnginePath(m_defaultEngineName));
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
        QString p = QFileDialog::getOpenFileName(&d, tr("Select a path to TheXTech executable", "File dialog title"),
                                                 c->text(), QString(), nullptr, c_fileDialogOptions);
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

#ifdef __APPLE__
        // Automatically recognize an executable inside of an app bundle
        QFileInfo x(m_customEnginePath);
        if(!m_customEnginePath.isEmpty() && x.isDir() && m_customEnginePath.endsWith(".app", Qt::CaseInsensitive))
        {
            QDir bundleMac(m_customEnginePath + "/Contents/MacOS");
            if(bundleMac.exists())
            {
                auto f = bundleMac.entryList(QDir::Files);
                if(!f.isEmpty())
                {
                    m_customEnginePath = bundleMac.absoluteFilePath(f.first());
                }
            }
        }
#endif

        rescanCapabilities();
    }
}

void TheXTechEngine::startGameAction()
{
    runNormalGame();
}

TheXTechEngine::TheXTechEngine(QObject *parent) :
    AbstractRuntimeEngine(parent),
    m_interface(parent)
{
    std::memset(m_menuItems, 0, sizeof(m_menuItems));
}

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
                     this, SIGNAL(testFinished()));

    QObject::connect(this, &TheXTechEngine::testStarted,
                     m_w, &MainWindow::stopMusicForTesting);
    QObject::connect(this, &TheXTechEngine::testFinished,
                     m_w, &MainWindow::testingFinished);

    QObject::connect(m_w, &MainWindow::configPackReloadBegin,
                     this, &TheXTechEngine::saveSetup);
    QObject::connect(m_w, &MainWindow::configPackReloaded,
                     this, &TheXTechEngine::loadSetup);

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
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setVisible(false);
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
            {
                m->setVisible(true);
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
            }
        });
    }

    {
        QAction *RunWorldTest = destmenu->addAction("runWorldTesting");
        QObject::connect(RunWorldTest,   &QAction::triggered,
                         this,               &TheXTechEngine::startTestAction,
                         Qt::QueuedConnection);
        m_menuRunWorldTestIPC = RunWorldTest;
        m_menuItems[menuItemId++] = RunWorldTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
            {
                m->setVisible(true);
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
            }
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setVisible(false);
        });
    }

    {
        QAction *RunBattleLevelTest = destmenu->addAction("runBattleTest");
        QObject::connect(RunBattleLevelTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startBattleTestAction,
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

    {
        QAction *RunLevelSafeTest = destmenu->addAction("runSafeTesting");
        QObject::connect(RunLevelSafeTest,   &QAction::triggered,
                    this,               &TheXTechEngine::startSafeTestAction,
                    Qt::QueuedConnection);
        m_menuRunWorldTestFile = RunLevelSafeTest;
        m_menuItems[menuItemId++] = RunLevelSafeTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setVisible(false);
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
            {
                m->setVisible(true);
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_FILE));
            }
        });
    }

    {
        QAction *RunWorldSafeTest = destmenu->addAction("runWorldSafeTesting");
        QObject::connect(RunWorldSafeTest,   &QAction::triggered,
                         this,               &TheXTechEngine::startSafeTestAction,
                         Qt::QueuedConnection);
        m_menuRunWorldTestFile = RunWorldSafeTest;
        m_menuItems[menuItemId++] = RunWorldSafeTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
            {
                m->setVisible(true);
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_FILE));
            }
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setVisible(false);
        });
    }

    destmenu->addSeparator();

    {
        QAction *selectStartWarp;
        m_startWarpAction = selectStartWarp = destmenu->addAction("setStartWarp");
        m_menuItems[menuItemId++] = selectStartWarp;
        QObject::connect(selectStartWarp, &QAction::triggered,
        [this](bool)
        {
            bool ok = false;
            int ret = QInputDialog::getInt(m_w,
                                           tr("Select the entrance warp"),
                                           tr("Please select the entrance warp number "
                                              "(if you specify 0, the level will start from the usual start point):"),
                                           m_startWarp,
                                           0, 5000, 1, &ok);
            if(ok)
                m_startWarp = ret;
        });
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setVisible(false);
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setVisible(true);
        });
    }

    {
        QMenu *saveSlot = destmenu->addMenu("testGameSave");
        m_saveSlotMenu = m_menuItems[menuItemId++] = saveSlot->menuAction();

        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setVisible(true);
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setVisible(false);
        });

        {
            QAction *sn, *s1, *s2, *s3;

            auto addSaveSlot = [this, saveSlot, &menuItemId](int id, const char *name)->QAction*
            {
                QAction *ret = saveSlot->addAction(name);
                ret->setCheckable(true);
                ret->setChecked(m_saveSlot == id);
                m_menuItems[menuItemId++] = ret;
                return ret;
            };

            sn = addSaveSlot(0, "saveNone");
            saveSlot->addSeparator();
            s1 = addSaveSlot(1, "save1");
            s2 = addSaveSlot(2, "save2");
            s3 = addSaveSlot(3, "save3");

            auto updateMenu = [this, sn, s1, s2, s3]()->void
            {
                sn->setChecked(m_saveSlot == 0);
                s1->setChecked(m_saveSlot == 1);
                s2->setChecked(m_saveSlot == 2);
                s3->setChecked(m_saveSlot == 3);
            };

            QObject::connect(sn,   &QAction::triggered,
            [this, updateMenu](bool)
            {
                m_saveSlot = 0;
                updateMenu();
            });
            QObject::connect(s1,   &QAction::triggered,
            [this, updateMenu](bool)
            {
                m_saveSlot = 1;
                updateMenu();
            });
            QObject::connect(s2,   &QAction::triggered,
            [this, updateMenu](bool)
            {
                m_saveSlot = 2;
                updateMenu();
            });
            QObject::connect(s3,   &QAction::triggered,
            [this, updateMenu](bool)
            {
                m_saveSlot = 3;
                updateMenu();
            });
        }

    }

    destmenu->addSeparator();

    {
        QMenu *renderType = destmenu->addMenu("renderType");
        m_menuItems[menuItemId++] = renderType->menuAction();

        QAction *r_d, *r_s, *r_a, *r_v, *r_gl, *r_gl11, *r_es, *r_es11;

        auto addRender = [this, renderType, &menuItemId](int id, const char *name)->QAction*
        {
            QAction *ret = renderType->addAction(name);
            ret->setCheckable(true);
            ret->setChecked(m_renderType == id);
            m_menuItems[menuItemId++] = ret;
            return ret;
        };

        r_d = addRender(-1, "renderDefault");
        r_s = addRender(0, "renderSoftware");
        r_a = addRender(1, "renderAccelerated");
        m_renderVSync = r_v = addRender(2, "renderVSync");
        m_renderModernOpenGL = r_gl = addRender(3, "renderGL");
        m_renderLegacyOpenGL = r_gl11 = addRender(4, "renderGL11");
        m_renderModernOpenGLES = r_es = addRender(5, "renderGLES");
        m_renderLegacyOpenGLES = r_es11 = addRender(6, "renderGLES11");

        auto updateMenu = [this, r_d, r_s, r_a, r_v, r_gl, r_gl11, r_es, r_es11]()->void
        {
            r_d->setChecked(m_renderType == -1);
            r_s->setChecked(m_renderType == 0);
            r_a->setChecked(m_renderType == 1);
            r_v->setChecked(m_renderType == 2);
            r_gl->setChecked(m_renderType == 3);
            r_gl11->setChecked(m_renderType == 4);
            r_es->setChecked(m_renderType == 5);
            r_es11->setChecked(m_renderType == 6);
        };

        QObject::connect(r_d,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = -1;
            updateMenu();
        });
        QObject::connect(r_s,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 0;
            updateMenu();
        });
        QObject::connect(r_a,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 1;
            updateMenu();
        });
        QObject::connect(r_v,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 2;
            updateMenu();
        });
        QObject::connect(r_gl,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 3;
            updateMenu();
        });
        QObject::connect(r_gl11,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 4;
            updateMenu();
        });
        QObject::connect(r_es,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 5;
            updateMenu();
        });
        QObject::connect(r_es11,   &QAction::triggered,
        [this, updateMenu](bool)
        {
            m_renderType = 6;
            updateMenu();
        });

        renderType->addSeparator();

        {
            QAction *enableVSync;
            m_renderVSyncFlag = enableVSync = renderType->addAction("enableVSync");
            enableVSync->setCheckable(true);
            enableVSync->setChecked(m_vsyncEnable);
            QObject::connect(enableVSync,   &QAction::toggled,
                             [this](bool state)
                             {
                                 m_vsyncEnable = state;
                             });
            m_menuItems[menuItemId++] = enableVSync;
        }
    }

    {
        QMenu *compatMode = destmenu->addMenu("compatMode");
        m_menuItems[menuItemId++] = compatMode->menuAction();

        QAction *c_d, *c_m, *c_x, *c_v;

        c_d = compatMode->addAction("compatDefault");
        c_d->setCheckable(true);
        c_d->setChecked(m_compatLevel == -1);
        m_menuItems[menuItemId++] = c_d;

        c_m = compatMode->addAction("compatModern");
        c_m->setCheckable(true);
        c_m->setChecked(m_compatLevel == 0);
        m_menuItems[menuItemId++] = c_m;

        c_x = compatMode->addAction("compatX2");
        c_x->setCheckable(true);
        c_x->setChecked(m_compatLevel == 1);
        m_menuItems[menuItemId++] = c_x;

        c_v = compatMode->addAction("compatVanilla");
        c_v->setCheckable(true);
        c_v->setChecked(m_compatLevel == 2);
        m_menuItems[menuItemId++] = c_v;

        QObject::connect(c_d,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(true);
            c_m->setChecked(false);
            c_x->setChecked(false);
            c_v->setChecked(false);
            m_compatLevel = -1;
        });
        QObject::connect(c_m,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(true);
            c_x->setChecked(false);
            c_v->setChecked(false);
            m_compatLevel = 0;
        });
        QObject::connect(c_x,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(false);
            c_x->setChecked(true);
            c_v->setChecked(false);
            m_compatLevel = 1;
        });
        QObject::connect(c_v,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(false);
            c_x->setChecked(false);
            c_v->setChecked(true);
            m_compatLevel = 2;
        });
    }

    {
        QMenu *speedRunMode = destmenu->addMenu("speedRunMode");
        m_menuItems[menuItemId++] = speedRunMode->menuAction();

        QAction *c_d, *c_m, *c_x, *c_v, *stt;

        c_d = speedRunMode->addAction("speedRunDisabled");
        c_d->setCheckable(true);
        c_d->setChecked(m_speedRunMode == -1);
        m_menuItems[menuItemId++] = c_d;

        c_m = speedRunMode->addAction("speedRunModern");
        c_m->setCheckable(true);
        c_m->setChecked(m_speedRunMode == 0);
        m_menuItems[menuItemId++] = c_m;

        c_x = speedRunMode->addAction("speedRunX2");
        c_x->setCheckable(true);
        c_x->setChecked(m_speedRunMode == 1);
        m_menuItems[menuItemId++] = c_x;

        c_v = speedRunMode->addAction("speedRunVanilla");
        c_v->setCheckable(true);
        c_v->setChecked(m_speedRunMode == 2);
        m_menuItems[menuItemId++] = c_v;

        speedRunMode->addSeparator();

        stt = speedRunMode->addAction("semiTransparentTimer");
        stt->setCheckable(true);
        stt->setChecked(m_speedRunTimerST);
        m_menuItems[menuItemId++] = stt;

        QObject::connect(c_d,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(true);
            c_m->setChecked(false);
            c_x->setChecked(false);
            c_v->setChecked(false);
            m_speedRunMode = -1;
        });
        QObject::connect(c_m,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(true);
            c_x->setChecked(false);
            c_v->setChecked(false);
            m_speedRunMode = 0;
        });
        QObject::connect(c_x,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(false);
            c_x->setChecked(true);
            c_v->setChecked(false);
            m_speedRunMode = 1;
        });
        QObject::connect(c_v,   &QAction::triggered,
                    [this, c_d, c_m, c_x, c_v](bool)
        {
            c_d->setChecked(false);
            c_m->setChecked(false);
            c_x->setChecked(false);
            c_v->setChecked(true);
            m_speedRunMode = 2;
        });
        QObject::connect(stt,   &QAction::triggered,
                    [this](bool b)
        {
            m_speedRunTimerST = b;
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

    Q_ASSERT(menuItemId < m_menuItemsSize);

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &TheXTechEngine::retranslateMenu);

    updateMenuCapabilities();
}

void TheXTechEngine::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *runTest = m_menuItems[menuItemId++];
        runTest->setText(tr("Test level",
                                "Run the testing of current file in TheXTech via interprocessing tunnel."));
    }
    {
        QAction *runTest = m_menuItems[menuItemId++];
        runTest->setText(tr("Test world",
                            "Run the testing of current file in TheXTech via interprocessing tunnel."));
    }
    {
        QAction *runTest = m_menuItems[menuItemId++];
        runTest->setText(tr("Test level in battle mode",
                                "Run a battle testing of current file in TheXTech via interprocessing tunnel."));
    }
    {
        QAction *runTest = m_menuItems[menuItemId++];
        runTest->setText(tr("Test saved level",
                                "Run the testing of current file in TheXTech from disk."));
    }
    {
        QAction *runTest = m_menuItems[menuItemId++];
        runTest->setText(tr("Test saved world",
                            "Run the testing of current file in TheXTech from disk."));
    }

    {
        QAction *startWarp = m_menuItems[menuItemId++];
        startWarp->setText(tr("Choose a start warp...",
                              "Select the warp number at which the game will be started."));
    }

    {
        QAction *saveSlot = m_menuItems[menuItemId++];
        saveSlot->setText(tr("Save slot",
                             "Select a save slot for playing the world map."));

        //  Sub-menu
        {
            QAction *saveSlot = m_menuItems[menuItemId++];
            saveSlot->setText(tr("Don't save",
                                 "Save slot sub-menu item."));
        }
        {
            QAction *saveSlot = m_menuItems[menuItemId++];
            saveSlot->setText(tr("Save slot 1",
                                 "Save slot sub-menu item."));
        }
        {
            QAction *saveSlot = m_menuItems[menuItemId++];
            saveSlot->setText(tr("Save slot 2",
                                 "Save slot sub-menu item."));
        }
        {
            QAction *saveSlot = m_menuItems[menuItemId++];
            saveSlot->setText(tr("Save slot 3",
                                 "Save slot sub-menu item."));
        }
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
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Modern OpenGL",
                                   "Hardware accelerated with modern OpenGL"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Legacy OpenGL 1.1",
                                   "Hardware accelerated with legacy OpenGL"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Modern OpenGL ES",
                                   "Hardware accelerated with modern OpenGL ES"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Legacy OpenGL ES 1.1",
                                   "Hardware accelerated with legacy OpenGL ES"));
        }

        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Enable V-Sync",
                                   "Enable the vertical synchronisation if available"));
        }
    }

    {
        QAction *compatMode = m_menuItems[menuItemId++];
        compatMode->setText(tr("Compatibility level",
                               "Choose the compatibility level used by the game"));
        //  Sub-menu
        {
            QAction *item = m_menuItems[menuItemId++];
            item->setText(tr("Default",
                             "Use preferred compatibility level"));
        }
        {
            QAction *item = m_menuItems[menuItemId++];
            item->setText(tr("Modern",
                             "Prefer all updates and bugfixes enabled"));
        }
        {
            QAction *item = m_menuItems[menuItemId++];
            item->setText(tr("X2",
                             "Disable all bugfixes and updates exceot these made at X2"));
        }
        {
            QAction *item = m_menuItems[menuItemId++];
            item ->setText(tr("Strict SMBX 1.3",
                              "Strict compatibility mode, all bugfixes and updates will be disabled to prepresent an old behaviour."));
        }
    }

    {
        QAction *speedRunMode = m_menuItems[menuItemId++];
        speedRunMode->setText(tr("Speedruner's stopwatch",
                                 "Speedrun mode menu"));
        //  Sub-menu
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Disabled",
                                   "Speedrun mode disabled"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Mode 1 (Modern)",
                                   "Enable speedrun with mode 1 (modern compatibility level)"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Mode 2 (X2)",
                                   "Enable speedrun with mode 2 (X2 compatibility level)"));
        }
        {
            QAction *renderType = m_menuItems[menuItemId++];
            renderType->setText(tr("Mode 3 (Strict SMBX 1.3)",
                                   "Enable speedrun with mode 3 (strict SMBX 1.3 compatibility level)"));
        }
        {
            QAction *stTimer = m_menuItems[menuItemId++];
            stTimer->setText(tr("Semi-transparent stopwatch",
                                "Show the speedrun stopwatch with a semi-transparent font."));
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
        QAction *enableMaxFps = m_menuItems[menuItemId++];
        enableMaxFps->setText(tr("Enable max FPS",
                                    "When running non-vsync, run game with a maximum possible frame-rate"));
        enableMaxFps->setToolTip(tr("When playing a game without V-Sync, run a game with a maximum possible frame-rate."));
    }
    {
        QAction *enableGrabAll = m_menuItems[menuItemId++];
        enableGrabAll->setText(tr("Enable grab all",
                                    "Allow player to grab absolutely any NPCs in a game."));
        enableGrabAll->setToolTip(tr("Allow player to grab any NPCs in a game."));
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


static void msgNotFound(QWidget *parent, const QString &what)
{
    QMessageBox::warning(parent,
                         TheXTechEngine::tr("Executable not found"),
                         TheXTechEngine::tr("Can't start TheXTech game because \"%1\" is not found. "
                                            "That might happen because of incorrect path to TheXTech executable was "
                                            "specified, please check the TheXTech path setup.").arg(what),
                         QMessageBox::Ok);
}

static void msgStartFailed(QWidget *parent, const QString &what, const QStringList &args, QProcess &p)
{
    QMessageBox::critical(parent,
                          TheXTechEngine::tr("TheXtech start failed"),
                          TheXTechEngine::tr("Can't start TheXTech because of following reason:\n%3.\n\nCommand: \"%1\"\nArguments: %2")
                          .arg(what)
                          .arg(args.join(", "))
                          .arg(p.errorString()),
                          QMessageBox::Ok);
}

bool TheXTechEngine::doTestLevelIPC(const LevelData &d)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    QString command = getEnginePath();

    if(!QFile::exists(command))
    {
        msgNotFound(m_w, command);
        return false;
    }

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
        case 3:
            if(m_caps.renders.contains("opengl"))
                args << "--render" << "opengl";
            break;
        case 4:
            if(m_caps.renders.contains("opengl11"))
                args << "--render" << "opengl11";
            break;
        case 5:
            if(m_caps.renders.contains("opengles"))
                args << "--render" << "opengles";
            break;
        case 6:
            if(m_caps.renders.contains("opengles11"))
                args << "--render" << "opengles11";
            break;
        }
    }

    if(m_caps.features.contains("vsync-flag") && m_vsyncEnable)
        args << "--vsync";

    if(m_caps.arguments.contains("start-warp") && m_startWarp > 0)
        args << "--start-warp" << QString::number(m_startWarp);

    if(m_speedRunMode >= 0)
    {
        switch(m_speedRunMode)
        {
        case 0:
            args << "--speed-run-mode" << "1";
            break;
        case 1:
            args << "--speed-run-mode" << "2";
            break;
        case 2:
            args << "--speed-run-mode" << "3";
            break;
        }

        if(m_speedRunTimerST)
            args << "--speed-run-semitransparent";
    }
    else if(m_compatLevel >= 0)
    {
        switch(m_compatLevel)
        {
        case 0:
            args << "--compat-level" << "modern";
            break;
        case 1:
            args << "--compat-level" << "smbx2";
            break;
        case 2:
            args << "--compat-level" << "smbx13";
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
        msgStartFailed(m_w, command, args, m_engineProc);
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

    if(!QFile::exists(command))
    {
        msgNotFound(m_w, command);
        return false;
    }

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
        case 3:
            if(m_caps.renders.contains("opengl"))
                args << "--render" << "opengl";
            break;
        case 4:
            if(m_caps.renders.contains("opengl11"))
                args << "--render" << "opengl11";
            break;
        case 5:
            if(m_caps.renders.contains("opengles"))
                args << "--render" << "opengles";
            break;
        case 6:
            if(m_caps.renders.contains("opengles11"))
                args << "--render" << "opengles11";
            break;
        }
    }

    if(m_caps.features.contains("vsync-flag") && m_vsyncEnable)
        args << "--vsync";

    if(m_caps.arguments.contains("start-warp") && m_startWarp > 0)
        args << "--start-warp" << QString::number(m_startWarp);

    if(m_speedRunMode >= 0)
    {
        switch(m_speedRunMode)
        {
        case 0:
            args << "--speed-run-mode" << "1";
            break;
        case 1:
            args << "--speed-run-mode" << "2";
            break;
        case 2:
            args << "--speed-run-mode" << "3";
            break;
        }

        if(m_speedRunTimerST)
            args << "--speed-run-semitransparent";
    }
    else if(m_compatLevel >= 0)
    {
        switch(m_compatLevel)
        {
        case 0:
            args << "--compat-level" << "modern";
            break;
        case 1:
            args << "--compat-level" << "smbx2";
            break;
        case 2:
            args << "--compat-level" << "smbx13";
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
        msgStartFailed(m_w, command, args, m_engineProc);
        m_errorString = "Failed to start TheXTech!" + command + "with args" + args.join(" ");
        return false;
    }
}

bool TheXTechEngine::doTestWorldFile(const QString &worldFile)
{
    Q_ASSERT(m_w);

    m_errorString.clear();

    if((capabilities() & CAP_WORLD_FILE) == 0)
        return false;

    QString command = getEnginePath();

    if(!QFile::exists(command))
    {
        msgNotFound(m_w, command);
        return false;
    }

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
        case 3:
            if(m_caps.renders.contains("opengl"))
                args << "--render" << "opengl";
            break;
        case 4:
            if(m_caps.renders.contains("opengl11"))
                args << "--render" << "opengl11";
            break;
        case 5:
            if(m_caps.renders.contains("opengles"))
                args << "--render" << "opengles";
            break;
        case 6:
            if(m_caps.renders.contains("opengles11"))
                args << "--render" << "opengles11";
            break;
        }
    }

    if(m_caps.features.contains("vsync-flag") && m_vsyncEnable)
        args << "--vsync";

    args << "--save-slot" << QString::number(m_saveSlot);
    args << worldFile;

    m_engineProc.start(command, args);
    if(m_engineProc.waitForStarted())
    {
        testStarted();
        return true;
    }
    else
    {
        msgStartFailed(m_w, command, args, m_engineProc);
        m_errorString = "Failed to start TheXTech!" + command + "with args" + args.join(" ");
        return false;
    }
}

bool TheXTechEngine::runNormalGame()
{
    Q_ASSERT(m_w);
    QString command = getEnginePath();

    if(!QFile::exists(command))
    {
        msgNotFound(m_w, command);
        return false;
    }

    QMutexLocker mlocker(&m_engineMutex);
    Q_UNUSED(mlocker)

//    QStringList args;
//    args << "--config=\"" + m_w->configs.config_dir + "\"";

    QProcess::startDetached(command, QStringList());

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
            CAP_HAS_MENU |
            (m_caps.features.contains("test-world-file") ? CAP_WORLD_FILE : 0);
}
