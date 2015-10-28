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
#include <audio/music_player.h>
#include <script/gui/additionalsettings.h>
#include <main_window/global_settings.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_item_toolbox.h>
#include <main_window/dock/wld_settings_box.h>
#include <common_features/bool_reseter.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>


//Reload opened file data
void MainWindow::on_actionReload_triggered()
{
    if(_is_reloading) return;
    _is_reloading=true;
    BoolReseter rst(&_is_reloading);
    Q_UNUSED(rst);

    QString filePath;
    QRect wnGeom;

    if (activeChildWindow()==1)
    {
        LevelData FileData;
        filePath = activeLvlEditWin()->curFile;

        if(activeLvlEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if (!QFileInfo(filePath).exists() ) {
            QMessageBox::critical(this, tr("File open error"),
            tr("Can't open the file.\nFile not exist."), QMessageBox::Ok);
                return;
        }

        if(activeLvlEditWin()->LvlData.modified)
        {
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

        //Open level file
        FileData = FileFormats::OpenLevelFile(filePath);

        if( !FileData.ReadFileValid ){
            formatErrorMsgBox(filePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeLvlEditWin()->LvlData.modified = false;

        QFile file(filePath+".meta");
        if(QFileInfo(filePath+".meta").exists())
        {
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                if(FileData.metaData.script)
                {
                    delete FileData.metaData.script;
                    FileData.metaData.script = NULL;
                }
                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw);
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

        //Remember last section ID and positions!
        int lastSection=0;
        QMap<int, QPair<long, long> > sectionPoss;
        lastSection = activeLvlEditWin()->LvlData.CurSection;
        SetCurrentLevelSection(lastSection);//Need to remember position of current section
        for(int i=0; i<activeLvlEditWin()->LvlData.sections.size(); i++)
        {
            LevelSection sct = activeLvlEditWin()->LvlData.sections[i];
            sectionPoss[sct.id]=QPair<long, long >(sct.PositionX, sct.PositionY);
        }

        activeLvlEditWin()->close();//Close old widget without closing of sub-window

        //Get pointer to current sub-window
        QMdiSubWindow *window = ui->centralWidget->activeSubWindow();

        //Get geometry of current subwindow
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        //delete window->widget();
        LevelEdit *chLvlWin = new LevelEdit(window);
        connect(chLvlWin, SIGNAL(forceReload()), this, SLOT(on_actionReload_triggered()));
        window->setWidget(chLvlWin);
        GraphicsWorkspace* gr = static_cast<GraphicsWorkspace *>(chLvlWin->getGraphicsView());
        connect(gr, SIGNAL(zoomValueChanged(QString)), zoom, SLOT(setText(QString)));

        //ui->centralWidget->activeSubWindow()->close();
        LevelEdit *child = chLvlWin;//createLvlChild();
        LvlMusPlay::setNoMusic();
        setMusic(false);
        if ((bool) (child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts)))
        {
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file reloaded"), 2000);
            LvlMusPlay::musicForceReset=true; //reset musics
            updateMenus(true);

            child->setFocus();
            //Restore saved section positions
            for(int i=0; i<child->LvlData.sections.size(); i++)
            {
                if(sectionPoss.contains(child->LvlData.sections[i].id))
                {
                    child->LvlData.sections[i].PositionX=sectionPoss[child->LvlData.sections[i].id].first;
                    child->LvlData.sections[i].PositionY=sectionPoss[child->LvlData.sections[i].id].second;
                }
            }

            SetCurrentLevelSection(lastSection);

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
        } else {
                WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
                WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
                WriteToLog(QtDebugMsg, ">>Option set");
            ui->centralWidget->activeSubWindow()->close();
                WriteToLog(QtDebugMsg, ">>Windows closed");
        }
    }
    else
    if (activeChildWindow()==2)
    {
        filePath = activeNpcEditWin()->curFile;

        if(activeNpcEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if(activeNpcEditWin()->isModyfied)
        {
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

//        if (!fileIn.open(QIODevice::ReadOnly)) {
//        QMessageBox::critical(this, tr("File open error"),
//        tr("Can't open the file."), QMessageBox::Ok);
//            return;
//        }

        NPCConfigFile FileData = FileFormats::ReadNpcTXTFile(filePath);
        if( !FileData.ReadFileValid ) return;
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        activeNpcEditWin()->isModyfied = false;
        activeNpcEditWin()->close();
        //NpcEdit *child = createNPCChild();

        QMdiSubWindow *npcWindow = ui->centralWidget->activeSubWindow();
        NpcEdit *child = new NpcEdit(&configs, npcWindow);
        npcWindow->setWidget(child);

        if (child->loadFile(filePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config reloaded"), 2000);
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            updateMenus(true);
        } else {
            child->close();
            npcWindow->close();
        }
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData FileData;
        filePath = activeWldEditWin()->curFile;

        if(activeWldEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if (!QFileInfo(filePath).exists() ) {
            QMessageBox::critical(this, tr("File open error"),
            tr("Can't open the file.\nFile not exist."), QMessageBox::Ok);
                return;
        }

        if(activeWldEditWin()->WldData.modified){
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

        FileData = FileFormats::OpenWorldFile(filePath);

        if( !FileData.ReadFileValid ){
            formatErrorMsgBox(filePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.filename = QFileInfo(filePath).baseName();
        FileData.path = QFileInfo(filePath).absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeWldEditWin()->WldData.modified = false;

        QFile file(filePath+".meta");
        if(QFileInfo(filePath+".meta").exists())
        {
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw);
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        QMdiSubWindow *worldWindow = ui->centralWidget->activeSubWindow();
        long posX = activeWldEditWin()->scene->_viewPort->horizontalScrollBar()->value();
        long posY = activeWldEditWin()->scene->_viewPort->verticalScrollBar()->value();

        activeWldEditWin()->close();

        WorldEdit *child = new WorldEdit(worldWindow);
        connect(child, SIGNAL(forceReload()), this, SLOT(on_actionReload_triggered()));
        worldWindow->setWidget(child);
        GraphicsWorkspace* gr = static_cast<GraphicsWorkspace *>(child->getGraphicsView());

        connect(gr, SIGNAL(zoomValueChanged(QString)), zoom, SLOT(setText(QString)));
        if ( (bool)(child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            updateMenus(true);
            dock_WldSettingsBox->setCurrentWorldSettings();
            if(FileData.HubStyledWorld)
            {
                dock_WldSettingsBox->setVisible(true);
                dock_WldSettingsBox->raise();
            }

            child->scene->_viewPort->horizontalScrollBar()->setValue(posX);
            child->scene->_viewPort->verticalScrollBar()->setValue(posY);

            statusBar()->showMessage(tr("World map file loaded"), 2000);
        } else {
            WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
            WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==3) activeWldEditWin()->WldData.modified = false;
            WriteToLog(QtDebugMsg, ">>Option set");
            ui->centralWidget->activeSubWindow()->close();
            WriteToLog(QtDebugMsg, ">>Windows closed");
        }

    }

    dock_LvlItemBox->clearFilter();
}


