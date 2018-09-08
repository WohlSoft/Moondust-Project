/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef Q_OS_WIN
#include "luna_tester.h"
#endif

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

/*!
 * \brief Starts level testing in PGE Engine with interprocess communication (File saving is not needed)
 */
void MainWindow::on_action_doTest_triggered()
{
    pge_engine_alphatestingNotify(this);

    QString command;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    if(!findEngine(this, command))
        return;

    if(engine_proc.state() == QProcess::Running)
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                                tr("Engine is already testing another level.\n"
                                   "Do you want to abort current testing process?"),
                                QMessageBox::Abort | QMessageBox::Cancel) == QMessageBox::Abort)
        {
            engine_proc.terminate();
            engine_proc.close();
        }
        return;
    }

    if(activeChildWindow() == 1)
    {
        IntEngine::init(&engine_proc);

        //if(activeLvlEditWin()->isUntitled) return;
        LevelEdit *edit = activeLvlEditWin();
        if(!edit) return;

        QStringList args;
        args << "--debug";
        args << "--config=\"" + configs.config_dir + "\"";
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

        IntEngine::setTestLvlBuffer(edit->LvlData);

        qDebug() << "Executing engine..." << command;
        engine_proc.start(command, args);
        if(engine_proc.waitForStarted())
        {
            qDebug() << "Started";
            //Stop music playback in the PGE Editor!
            stopMusicForTesting();
        }
        else
            qWarning() << "Failed to start PGE Engine!" << command << "with args" << args;

    }
    else
        return;
}

void MainWindow::on_action_doTestWld_triggered()
{
    /*
        FIXME!!! Implement the right world map testing via interprocess
        with ability to start player at absolutely any position on it
    */
    if(activeChildWindow() == 3)
    {
        WorldEdit *we = activeWldEditWin();
        if(!we->isUtitled() && we->isModified())
        {
            QMessageBox::StandardButton reply =
                    QMessageBox::question(this,
                                  tr("World map testing of saved file"),
                                  tr("File is not saved!\n"
                                     "Do you want to save file or you want to run test "
                                     "of copy which is currently saved on the disk?"),
                                  QMessageBox::Yes|QMessageBox::No|QMessageBox::Abort);
            if(reply == QMessageBox::Yes)
            {
                if(!we->save())
                    return;
            }
            else if(reply != QMessageBox::No)
                return;
        }
        on_action_doSafeTest_triggered();
    }
}


/*!
 * \brief Starts level testing in PGE Engine without interprocess communication (File saving is needed)
 */
void MainWindow::on_action_doSafeTest_triggered()
{
    pge_engine_alphatestingNotify(this);

    QString command = ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    if(!findEngine(this, command))
        return;

    if(engine_proc.state() == QProcess::Running)
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                                tr("Engine is already testing another level.\n"
                                   "Do you want to abort current testing process?"),
                                QMessageBox::Abort | QMessageBox::Cancel) == QMessageBox::Abort)
        {
            engine_proc.terminate();
            engine_proc.close();
        }
        return;
    }

    QStringList args;
    args << "--debug";
    args << "--config=\"" + configs.config_dir + "\"";

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

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit * le = activeLvlEditWin();
        if(le->isUtitled())
        {
            QMessageBox::warning(this, tr("Save file first"),
                                 tr("To run testing of saved file, please save them into disk first!\n"
                                    "You can run testing without saving of file if you will use \"Run testing\" menu item."),
                                 QMessageBox::Ok);
            return;
        }
        args << le->curFile;
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *we = activeWldEditWin();
        if(we->isUtitled())
        {
            QMessageBox::warning(this, tr("Save file first"),
                                 tr("To run testing of saved file, please save them into disk first!\n"
                                    "You can run testing without saving of file if you will use \"Run testing\" menu item."),
                                 QMessageBox::Ok);
            return;
        }
        args << we->curFile;
    }

    engine_proc.start(command, args);
    if(engine_proc.waitForStarted())
    {
        //Stop music playback in the PGE Editor!
        stopMusicForTesting();
    }

}

/*!
 * \brief Starts PGE Engine with current configuration package selected
 */
void MainWindow::on_action_Start_Engine_triggered()
{
    QString command = ApplicationPath + QStringLiteral("/") + PGE_ENGINE_EXE;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    if(!findEngine(this, command))
        return;

    QStringList args;
    args << "--config=\"" + configs.config_dir + "\"";

    QProcess::startDetached(command, args);

    //Stop music playback in the PGE Editor!
    setMusicButton(false);
    on_actionPlayMusic_triggered(false);

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
