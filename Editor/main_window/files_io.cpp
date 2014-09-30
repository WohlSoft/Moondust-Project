/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include "../file_formats/file_formats.h"

#include "music_player.h"
#include "global_settings.h"


void MainWindow::openFilesByArgs(QStringList args)
{
    for(int i=1; i<args.size(); i++)
    {
        if(QFile::exists(args[i])) OpenFile(args[i]);
    }
}

void MainWindow::OpenFile(QString FilePath)
{

    QFile file(FilePath);


    QMdiSubWindow *existing = findOpenedFileWin(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }


    if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("File open error"),
    tr("Can't open the file."), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    GlobalSettings::openPath = in_1.absoluteDir().absolutePath();

    if((in_1.suffix().toLower() == "lvl")||(in_1.suffix().toLower() == "lvlx"))
    {

        LevelData FileData;

        if(in_1.suffix().toLower() == "lvl")
            FileData = FileFormats::ReadLevelFile(file);         //Read SMBX LVL File
        else
            FileData = FileFormats::ReadExtendedLevelFile(file); //Read PGE LVLX File

        if( !FileData.ReadFileValid ) return;
        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;

        leveledit *child = createLvlChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            updateMenus(true);
            SetCurrentLevelSection(0);
            setDoorsToolbox();
            setLayersBox();

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
            LvlMusPlay::musicForceReset=true; //reset musics
            on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());

        } else {
            WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
            WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
            WriteToLog(QtDebugMsg, ">> Option seted");
            ui->centralWidget->activeSubWindow()->close();
            WriteToLog(QtDebugMsg, ">>Windows closed");
        }
    }
    else
    if((in_1.suffix().toLower() == "wld")||(in_1.suffix().toLower() == "wldx"))
    {
        WorldData FileData;
        if(in_1.suffix().toLower() == "wld")
            FileData= FileFormats::ReadWorldFile(file);
        else
            FileData= FileFormats::ReadExtendedWorldFile(file);

        if( !FileData.ReadFileValid ) return;

        WorldEdit *child = createWldChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            updateMenus(true);            
            setCurrentWorldSettings();
            if(FileData.HubStyledWorld)
            {
                ui->WorldSettings->setVisible(true);
                ui->WorldSettings->raise();
            }
            statusBar()->showMessage(tr("World map file loaded"), 2000);
        } else {
            child->close();
        }

        //QMessageBox::information(this, tr("Dummy"),
        //tr("Sorry, the World Maps support is not inplemented in this version."),
        //QMessageBox::Ok);

    }
    else
    if(in_1.suffix().toLower() == "txt")
    {
        NPCConfigFile FileData = FileFormats::ReadNpcTXTFile(file);
        if( !FileData.ReadFileValid ) return;

        npcedit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
            updateMenus(true);
        } else {
            child->close();
        }
    }
    else
    {
    QMessageBox::warning(this, tr("Bad file"),
     tr("This file have unknown extension"),
     QMessageBox::Ok);
    return;
    }

    // Add to recent fileList
    AddToRecentFiles(FilePath);
    SyncRecentFiles();
}


void MainWindow::save()
{
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
    leveledit *ChildWindow0=NULL;
    npcedit *ChildWindow2=NULL;
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
        if(QString(window->widget()->metaObject()->className())=="WorldEdit")
        {
        ChildWindow3 = qobject_cast<WorldEdit *>(window->widget());
            ChildWindow3->save();
        }
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
            ChildWindow0->save();
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
            ChildWindow2->save();
        }

        progress.setValue(++counter);
        qApp->processEvents();
    }

    progress.close();

}

void MainWindow::close_sw()
{
    if(ui->centralWidget->subWindowList().size()>0)
        ui->centralWidget->activeSubWindow()->close();
}

int MainWindow::subWins()
{
    return ui->centralWidget->subWindowList().size();
}


// ///////////Events////////////////////
void MainWindow::on_OpenFile_triggered()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),GlobalSettings::openPath,
        QString("All supported formats (*.LVLX *.WLDX *.INI *.LVL *.WLD npc-*.TXT)\n"
        "All SMBX files (*.LVL *.WLD npc-*.TXT)\n"
        "All PGE files (*.LVLX *.WLDX npc-*.TXT *.INI)\n"
        "SMBX Level (*.LVL)\n"
        "PGE Level (*.LVLX)\n"
        "SMBX World (*.WLD)\n"
        "PGE World (*.WLDX)\n"
        "SMBX NPC Config (npc-*.TXT)\n"
        "All Files (*.*)"),0);

        if(fileName_DATA==NULL) return;

        OpenFile(fileName_DATA);

}

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

