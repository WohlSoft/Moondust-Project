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

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QProcess>
#ifdef Q_OS_MAC
#include <QStandardPaths>
#endif

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>
#include <dev_console/devconsole.h>
#include <audio/sdl_music_player.h>

#include "testing_settings.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <PGE_File_Formats/file_formats.h>

#include <testing/pge_engine.h>
#include <testing/luna_tester_engine.h>
#include <testing/thextech_engine.h>
#include <testing/38a_engine.h>
#include <networking/engine_intproc.h>


void MainWindow::initTesting()
{
    QAction *menuNext = ui->sepEngineExtras;

    bool noDefault = ConfStatus::hideNonDefaultEngines;
    auto defEngine = ConfStatus::defaultTestEngine;

    m_testPGE.reset(new PgeEngine(this));
    m_testPGE->init();
    m_testAllEngines.push_back(&m_testPGE);
    if(!noDefault || defEngine == ConfStatus::ENGINE_MOONDUST)
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon pgeEngine;
        pgeEngine.addPixmap(QPixmap(":/engines/moondust/16.png"));
        pgeEngine.addPixmap(QPixmap(":/engines/moondust/32.png"));
        pgeEngine.addPixmap(QPixmap(":/engines/moondust/48.png"));
        pgeEngine.addPixmap(QPixmap(":/engines/moondust/128.png"));
        pgeEngine.addPixmap(QPixmap(":/engines/moondust/256.png"));

        QMenu *pgeEngineMenu = ui->menuTest->addMenu(pgeEngine, "Moondust Engine [ALPHA]");
        ui->menuTest->insertMenu(menuNext, pgeEngineMenu);

        m_testPGE->initMenu(pgeEngineMenu);

        QAction *sep = pgeEngineMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);

        menuNext = sep;
    }

    m_testTheXTech.reset(new TheXTechEngine(this));
    m_testTheXTech->init();
    m_testAllEngines.push_back(&m_testTheXTech);
    if(!noDefault || defEngine == ConfStatus::ENGINE_THEXTECH)
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon theXTech;
        theXTech.addPixmap(QPixmap(":/engines/thextech/16.png"));
        theXTech.addPixmap(QPixmap(":/engines/thextech/32.png"));
        theXTech.addPixmap(QPixmap(":/engines/thextech/48.png"));
        theXTech.addPixmap(QPixmap(":/engines/thextech/128.png"));
        theXTech.addPixmap(QPixmap(":/engines/thextech/256.png"));

        QMenu *theXTechMenu = ui->menuTest->addMenu(theXTech, "TheXTech");
        ui->menuTest->insertMenu(menuNext, theXTechMenu);

        m_testTheXTech->initMenu(theXTechMenu);

        QAction *sep = theXTechMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);

        menuNext = sep;
    }

    m_testSMBX38A.reset(new SanBaEiRuntimeEngine(this));
    m_testSMBX38A->init();
    m_testAllEngines.push_back(&m_testSMBX38A);
    if(!noDefault || defEngine == ConfStatus::ENGINE_38A)
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon sanBaIcon;
        sanBaIcon.addPixmap(QPixmap(":/engines/38a/16.png"));
        sanBaIcon.addPixmap(QPixmap(":/engines/38a/32.png"));
        sanBaIcon.addPixmap(QPixmap(":/engines/38a/48.png"));
        sanBaIcon.addPixmap(QPixmap(":/engines/38a/256.png"));

        QMenu *sanBaEiMenu = ui->menuTest->addMenu(sanBaIcon, "SMBX-38A");
        ui->menuTest->insertMenu(menuNext, sanBaEiMenu);

        m_testSMBX38A->initMenu(sanBaEiMenu);

        QAction *sep = sanBaEiMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);
        menuNext = sep;
    }

    m_testLunaTester.reset(new LunaTesterEngine(this));
    m_testLunaTester->init();
    m_testAllEngines.push_back(&m_testLunaTester);
    if(!noDefault || defEngine == ConfStatus::ENGINE_LUNA)
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon lunaIcon;
        lunaIcon.addPixmap(QPixmap(":/engines/lunalua/16.png"));
        lunaIcon.addPixmap(QPixmap(":/engines/lunalua/32.png"));
        lunaIcon.addPixmap(QPixmap(":/engines/lunalua/48.png"));
        lunaIcon.addPixmap(QPixmap(":/engines/lunalua/256.png"));

        QMenu *lunaMenu = ui->menuTest->addMenu(lunaIcon, "LunaTester");
        ui->menuTest->insertMenu(menuNext, lunaMenu);

        m_testLunaTester->initMenu(lunaMenu);

        QAction *sep = lunaMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);
        menuNext = sep;
    }

    switch(defEngine)
    {
    case ConfStatus::ENGINE_MOONDUST:
    default:
        m_testEngine = m_testPGE.get();
        break;
    case ConfStatus::ENGINE_LUNA:
        m_testEngine = m_testLunaTester.get();
        break;
    case ConfStatus::ENGINE_THEXTECH:
        m_testEngine = m_testTheXTech.get();
        break;
    case ConfStatus::ENGINE_38A:
        m_testEngine = m_testSMBX38A.get();
        break;
    }

    updateTestingCaps();

    QObject::connect(&g_intEngine, &IntEngineSignals::engineNumStarsChanged, this, [](int stars)
    {
        GlobalSettings::testing.xtra_starsNum = stars;
    });

    QObject::connect(&g_intEngine, &IntEngineSignals::enginePlayerStateUpdated,
                     this, [](int playerID, int character, int state, int vehicleID, int vehicleType)
    {
        auto &t =GlobalSettings::testing;
        if(playerID == 0)
        {
            t.p1_char        = character;
            t.p1_state       = state;
            t.p1_vehicleID   = vehicleID;
            t.p1_vehicleType = vehicleType;
        }
        else if(playerID == 1)
        {
            t.p2_char        = character;
            t.p2_state       = state;
            t.p2_vehicleID   = vehicleID;
            t.p2_vehicleType = vehicleType;
        }
    });
}

void MainWindow::updateTestingCaps()
{
    if(!m_testEngine)
        return;

    ui->action_doTest->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
    ui->action_doSafeTest->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_LEVEL_FILE));

    ui->action_doTestWld->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
    ui->action_doSafeTestWld->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_WORLD_FILE));
}

void MainWindow::closeTesting()
{
    for(auto *e : m_testAllEngines)
    {
        (*e)->unInit();
        (*e).reset();
    }
}

/*!
 * \brief Unlocks music button and starts music if that was started pre-testing state
 */
void MainWindow::testingFinished()
{
    ui->actionPlayMusic->setEnabled(true);
    m_ui_musicVolume->setEnabled(true);
    if(GlobalSettings::recentMusicPlayingState)
    {
        setMusicButton(true);
        on_actionPlayMusic_triggered(true);
    }
}

/*!
 * \brief Stops music playing and locks music button
 */
void MainWindow::stopMusicForTesting()
{
    GlobalSettings::recentMusicPlayingState = ui->actionPlayMusic->isChecked();
    setMusicButton(false);
    on_actionPlayMusic_triggered(false);
    ui->actionPlayMusic->setEnabled(false);
    m_ui_musicVolume->setEnabled(false);
}


/*!
 * \brief Starts level testing in PGE Engine with interprocess communication (File saving is not needed)
 */
void MainWindow::on_action_doTest_triggered()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(m_testEngine, this))
        return;

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *edit = activeLvlEditWin();
        if(!edit)
            return;

        m_testEngine->doTestLevelIPC(edit->LvlData);
    }
    else
        return;
}

void MainWindow::on_action_doTestWld_triggered()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(m_testEngine, this))
        return;

    if(activeChildWindow() == WND_World)
    {
        WorldEdit *we = activeWldEditWin();
        if(!we->isUntitled() && we->isModified())
        {
            QMessageBox::StandardButton reply =
                QMessageBox::question(this,
                                      tr("World map testing of saved file"),
                                      tr("File is not saved!\n"
                                         "Do you want to save file or you want to run test "
                                         "of copy which is currently saved on the disk?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
            if(reply == QMessageBox::Yes)
            {
                if(!we->save())
                    return;
            }
            else if(reply != QMessageBox::No)
                return;
        }
        on_action_doSafeTestWld_triggered();
    }
}


/*!
 * \brief Starts level testing in PGE Engine without interprocess communication (File saving is needed)
 */
void MainWindow::on_action_doSafeTest_triggered()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(m_testEngine, this))
        return;

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *le = activeLvlEditWin();
        if(le->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(this);
            return;
        }
        m_testEngine->doTestLevelFile(le->curFile);
    }
}

void MainWindow::on_action_doSafeTestWld_triggered()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(m_testEngine, this))
        return;

    if(activeChildWindow() == WND_World)
    {
        WorldEdit *we = activeWldEditWin();
        if(we->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(this);
            return;
        }
        m_testEngine->doTestWorldFile(we->curFile);
    }
}

/*!
 * \brief Starts PGE Engine with current configuration package selected
 */
void MainWindow::on_action_Start_Engine_triggered()
{
    m_testEngine->runNormalGame();
}

/*!
 * \brief Settings of testing
 */
void MainWindow::on_action_testSettings_triggered()
{
    TestingSettings testingSetup(this);
    util::DialogToCenter(&testingSetup, true);
    testingSetup.exec();
}
