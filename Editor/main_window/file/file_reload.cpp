/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/util.h>

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
    int activeWindow = activeChildWindow(LastActiveSubWindow);

    if( activeWindow == 1 )
    {
        LevelEdit* lvlEdit = activeLvlEditWin(LastActiveSubWindow);
        LevelData FileData;
        filePath = lvlEdit->curFile;

        if(lvlEdit->isUntitled)
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

        if(lvlEdit->LvlData.modified)
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
        if( !FileFormats::OpenLevelFile( filePath, FileData) )
        {
            formatErrorMsgBox(filePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;
        }
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        lvlEdit->LvlData.modified = false;

        //Remember last section ID and positions!
        int lastSection=0;
        QMap<int, QPair<long, long> > sectionPoss;
        lastSection = lvlEdit->LvlData.CurSection;
        SetCurrentLevelSection(lastSection);//Need to remember position of current section
        for(int i=0; i<lvlEdit->LvlData.sections.size(); i++)
        {
            LevelSection sct = lvlEdit->LvlData.sections[i];
            sectionPoss[sct.id]=QPair<long, long >(sct.PositionX, sct.PositionY);
        }

        lvlEdit->close();//Close old widget without closing of sub-window

        //Get pointer to current sub-window
        //QMdiSubWindow *window = LastActiveSubWindow;

        //Get geometry of current subwindow
        wnGeom = LastActiveSubWindow->geometry();
        //delete window->widget();
        LevelEdit *chLvlWin = new LevelEdit(LastActiveSubWindow);
        connect(chLvlWin, SIGNAL(forceReload()), this, SLOT(on_actionReload_triggered()));
        LastActiveSubWindow->setWidget(chLvlWin);
        GraphicsWorkspace* gr = static_cast<GraphicsWorkspace *>(chLvlWin->getGraphicsView());
        connect(gr, SIGNAL(zoomValueChanged(QString)), zoom, SLOT(setText(QString)));

        //LastActiveSubWindow->close();
        LevelEdit *child = chLvlWin;//createLvlChild();
        LvlMusPlay::setNoMusic();
        setMusic();
        if ((bool) (child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts)))
        {
            child->show();
            LastActiveSubWindow->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file reloaded"), 2000);
            LvlMusPlay::musicForceReset=true; //reset musics
            updateMenus(LastActiveSubWindow, true);

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
                LogDebug(">>File loading aborted");
            //child->show();
                LogDebug(">>Window showed");
            child->LvlData.modified = false;
                LogDebug(">>Option set");
            LastActiveSubWindow->close();
                LogDebug(">>Windows closed");
        }
    }
    else
    if( activeWindow == 2 )
    {
        NpcEdit* npcEdit = activeNpcEditWin(LastActiveSubWindow);
        filePath = npcEdit->curFile;

        if(npcEdit->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if(npcEdit->isModyfied)
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

        NPCConfigFile FileData;
        if( !FileFormats::ReadNpcTXTFileF(filePath, FileData) )
        {
            QMessageBox::critical(this, QObject::tr("File open error"), tr("Can't read the file"), QMessageBox::Ok);
            return;
        }
        wnGeom = LastActiveSubWindow->geometry();
        npcEdit->isModyfied = false;
        npcEdit->close();
        //NpcEdit *child = createNPCChild();
        //QMdiSubWindow *npcWindow = LastActiveSubWindow;
        NpcEdit *child = new NpcEdit(&configs, LastActiveSubWindow);
        LastActiveSubWindow->setWidget(child);

        if (child->loadFile(filePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config reloaded"), 2000);
            child->show();
            LastActiveSubWindow->setGeometry(wnGeom);
            updateMenus(LastActiveSubWindow, true);
        } else {
            child->close();
            LastActiveSubWindow->close();
        }
    }
    else
    if( activeWindow == 3 )
    {
        WorldEdit* wldEdit = activeWldEditWin(LastActiveSubWindow);
        WorldData FileData;
        filePath = wldEdit->curFile;

        if(wldEdit->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if (!QFileInfo(filePath).exists() )
        {
            QMessageBox::critical(this, tr("File open error"),
            tr("Can't open the file.\nFile not exist."), QMessageBox::Ok);
                return;
        }

        if(wldEdit->WldData.modified)
        {
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

        if( !FileFormats::OpenWorldFile( filePath, FileData ) )
        {
            formatErrorMsgBox(filePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;
        }

        QFileInfo finfo(filePath);
        FileData.filename = util::getBaseFilename(finfo.fileName());
        FileData.path = finfo.absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        wldEdit->WldData.modified = false;

        wnGeom = LastActiveSubWindow->geometry();
        QMdiSubWindow *worldWindow = LastActiveSubWindow;
        long posX = wldEdit->scene->_viewPort->horizontalScrollBar()->value();
        long posY = wldEdit->scene->_viewPort->verticalScrollBar()->value();

        wldEdit->close();

        WorldEdit *child = new WorldEdit(worldWindow);
        connect(child, SIGNAL(forceReload()), this, SLOT(on_actionReload_triggered()));
        worldWindow->setWidget(child);
        GraphicsWorkspace* gr = static_cast<GraphicsWorkspace *>(child->getGraphicsView());

        connect(gr, SIGNAL(zoomValueChanged(QString)), zoom, SLOT(setText(QString)));
        if ( (bool)(child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts)) )
        {
            child->show();
            LastActiveSubWindow->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            updateMenus(LastActiveSubWindow, true);
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
            LogDebug(">>File loading aborted");
            //child->show();
            LogDebug(">>Window showed");
            child->WldData.modified = false;
            LogDebug(">>Option set");
            LastActiveSubWindow->close();
            LogDebug(">>Windows closed");
        }

    }

    dock_LvlItemBox->clearFilter();
}


