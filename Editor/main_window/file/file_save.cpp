/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMdiSubWindow>

#include <PGE_File_Formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::save()
{
    qApp->setActiveWindow(this);
    bool saved = false;
    int WinType = activeChildWindow();

    if(WinType != WND_NoWindow)
    {
        QProgressDialog progress(tr("Saving of file..."), tr("Abort"), 0, 1, this);
        progress.setWindowTitle(tr("Saving"));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(util::alignToScreenCenter(progress.size()));
        progress.setMinimumDuration(0);
        progress.setAutoClose(false);
        progress.setCancelButton(nullptr);
        progress.show();
        qApp->processEvents();

        if(WinType == WND_World)
            saved = activeWldEditWin()->save(true);
        else if(WinType == WND_NpcTxt)
            saved = activeNpcEditWin()->save();
        else if(WinType == WND_Level)
            saved = activeLvlEditWin()->save(true);

        if(saved)
            statusBar()->showMessage(tr("File saved"), 2000);

        saveSettings();
    }
}

void MainWindow::save_as()
{
    bool saved = false;
    int WinType = activeChildWindow();

    if(WinType != WND_NoWindow)
    {
        if(WinType == WND_World)
            saved = activeWldEditWin()->saveAs(true);
        else if(WinType == WND_NpcTxt)
            saved = activeNpcEditWin()->saveAs();
        else if(WinType == WND_Level)
            saved = activeLvlEditWin()->saveAs(true);

        if(saved)
            statusBar()->showMessage(tr("File saved"), 2000);

        saveSettings();
    }
}

void MainWindow::save_all()
{
    QProgressDialog progress(tr("Saving of files..."), tr("Abort"), 0, ui->centralWidget->subWindowList().size(), this);
    {
         progress.setWindowTitle(tr("Saving"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(util::alignToScreenCenter(progress.size()));
         progress.setMinimumDuration(0);
         progress.setAutoClose(false);
         progress.setCancelButton(nullptr);
    }
    progress.show();
    qApp->processEvents();

    int counter = 0;
    for(QMdiSubWindow *window : ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className()) == WORLD_EDIT_CLASS)
        {
            auto *w = qobject_cast<WorldEdit *>(window->widget());
            Q_ASSERT(w);
            w->save();
        }
        else if(QString(window->widget()->metaObject()->className()) == LEVEL_EDIT_CLASS)
        {
            auto *w = qobject_cast<LevelEdit *>(window->widget());
            Q_ASSERT(w);
            w->save();
        }
        else if(QString(window->widget()->metaObject()->className()) == NPC_EDIT_CLASS)
        {
            auto *w = qobject_cast<NpcEdit *>(window->widget());
            Q_ASSERT(w);
            w->save();
        }

        progress.setValue(++counter);
        qApp->processEvents();
    }

    progress.close();
    saveSettings();
}


// ///////////Events////////////////////


void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_as_triggered()
{
    save_as();
}

void MainWindow::on_actionClose_triggered()
{
    close_sw();
}

void MainWindow::on_actionSave_all_triggered()
{
    save_all();
}

void MainWindow::updateAutoSaver()
{
    m_autoSaveTimer.stop();
    if(GlobalSettings::LvlOpts.autoSave_enable)
    {
        if(GlobalSettings::LvlOpts.autoSave_interval < 1)
            GlobalSettings::LvlOpts.autoSave_interval = 1;
        else if(GlobalSettings::LvlOpts.autoSave_interval > 30)
            GlobalSettings::LvlOpts.autoSave_interval = 30;

        m_autoSaveTimer.setInterval(GlobalSettings::LvlOpts.autoSave_interval * 60000);
        m_autoSaveTimer.start();
    }
}

void MainWindow::runAutoSave()
{
    QList<QMdiSubWindow *> listOfAllSubWindows = allEditWins();
    for(QMdiSubWindow *subWin : listOfAllSubWindows)
    {
        if(activeChildWindow(subWin) == MainWindow::WND_Level)
        {
            LevelEdit *leveledit = activeLvlEditWin(subWin);

            if(!leveledit)
                continue;
            leveledit->runAutoSave();
        }
        else if(activeChildWindow(subWin) == MainWindow::WND_NpcTxt)
        {
            NpcEdit *npcedit = activeNpcEditWin(subWin);

            if(!npcedit)
                continue;
            npcedit->runAutoSave();
        }
        else if(activeChildWindow(subWin) == MainWindow::WND_World)
        {
            WorldEdit *worldedit = activeWldEditWin(subWin);

            if(!worldedit)
                continue;
            worldedit->runAutoSave();
        }
    }
}
