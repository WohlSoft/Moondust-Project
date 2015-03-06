/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <PGE_File_Formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::save()
{
    qApp->setActiveWindow(this);
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        QProgressDialog progress(tr("Saving of file..."), tr("Abort"), 0, 1, this);
             progress.setWindowTitle(tr("Saving"));
             progress.setWindowModality(Qt::WindowModal);
             progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
             progress.setFixedSize(progress.size());
             progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
             progress.setMinimumDuration(0);
             progress.setAutoClose(false);
             progress.setCancelButton(NULL);
        progress.show();
        qApp->processEvents();

        if(WinType==3)
            saved = activeWldEditWin()->save(true);
        if(WinType==2)
            saved = activeNpcEditWin()->save();
        if(WinType==1)
            saved = activeLvlEditWin()->save(true);

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_as()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==3)
            saved = activeWldEditWin()->saveAs(true);
        if(WinType==2)
            saved = activeNpcEditWin()->saveAs();
        if(WinType==1)
            saved = activeLvlEditWin()->saveAs(true);

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_all()
{
    LevelEdit *ChildWindow0=NULL;
    NpcEdit *ChildWindow2=NULL;
    WorldEdit *ChildWindow3=NULL;

    QProgressDialog progress(tr("Saving of files..."), tr("Abort"), 0, ui->centralWidget->subWindowList().size(), this);
         progress.setWindowTitle(tr("Saving"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(0);
         progress.setAutoClose(false);
         progress.setCancelButton(NULL);
    progress.show();
    qApp->processEvents();

    int counter=0;
    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
        {
        ChildWindow3 = qobject_cast<WorldEdit *>(window->widget());
            ChildWindow3->save();
        }
        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
        {
        ChildWindow0 = qobject_cast<LevelEdit *>(window->widget());
            ChildWindow0->save();
        }
        else if(QString(window->widget()->metaObject()->className())==NPC_EDIT_CLASS)
        {
        ChildWindow2 = qobject_cast<NpcEdit *>(window->widget());
            ChildWindow2->save();
        }

        progress.setValue(++counter);
        qApp->processEvents();
    }

    progress.close();

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


