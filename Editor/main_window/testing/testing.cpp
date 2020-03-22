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
#include <common_features/logger_sets.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>
#include <dev_console/devconsole.h>
#include <audio/sdl_music_player.h>

#include "testing_settings.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <networking/engine_intproc.h>
#include <PGE_File_Formats/file_formats.h>

#include "engines/pge_engine.h"
#include "engines/luna_tester_engine.h"


void MainWindow::initTesting()
{
    QAction *menuNext = ui->sepEngineExtras;

    m_testPGE.reset(new PgeEngine(this));
    m_testPGE->init();
    if(!ConfStatus::SmbxTest_HidePgeEngine)
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon pgeEngine;
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_16.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_32.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_48.png"));

        QMenu *pgeEngineMenu = ui->menuTest->addMenu(pgeEngine, "PGE Engine");
        ui->menuTest->insertMenu(menuNext, pgeEngineMenu);

        m_testPGE->initMenu(pgeEngineMenu);

        QAction *sep = pgeEngineMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);

        menuNext = sep;
    }

    m_testLunaTester.reset(new LunaTesterEngine(this));
    m_testLunaTester->init();
    {
        ui->sepEngineExtras->setVisible(false);
        QIcon lunaIcon(":/lunalua.ico");
        QMenu *lunaMenu = ui->menuTest->addMenu(lunaIcon, "LunaTester");
        ui->menuTest->insertMenu(menuNext, lunaMenu);

        m_testLunaTester->initMenu(lunaMenu);

        QAction *sep = lunaMenu->addSeparator();
        ui->menuTest->insertAction(menuNext, sep);
        menuNext = sep;
    }

    if(ConfStatus::SmbxTest_By_Default)
        m_testEngine = m_testLunaTester.get();
    else
        m_testEngine = m_testPGE.get();

    ui->action_doTest->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
    ui->action_doSafeTest->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_LEVEL_FILE));

    ui->action_doTestWld->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
    ui->action_doSafeTest->setEnabled(m_testEngine->hasCapability(AbstractRuntimeEngine::CAP_WORLD_FILE));
}

void MainWindow::closeTesting()
{
    if(m_testPGE.get())
    {
        m_testPGE->unInit();
        m_testPGE.reset();
    }

    if(m_testLunaTester.get())
    {
        m_testLunaTester->unInit();
        m_testLunaTester.reset();
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
