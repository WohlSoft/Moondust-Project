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

#include <editing/_dialogs/levelfilelist.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/mainwinconnect.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_settings_box.h"
#include "ui_wld_settings_box.h"

WorldSettingsBox::WorldSettingsBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WorldSettingsBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    world_settings_lock_fields=false;

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    #ifdef Q_OS_WIN
    setFloating(true);
    #endif
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    mw()->docks_world.
          addState(this, &GlobalSettings::WorldSettingsToolboxVis);
}

WorldSettingsBox::~WorldSettingsBox()
{
    delete ui;
}

void WorldSettingsBox::re_translate()
{
    ui->retranslateUi(this);
    setCurrentWorldSettings();
}




void WorldSettingsBox::on_WorldSettingsBox_visibilityChanged(bool visible)
{
    mw()->ui->actionWorld_settings->setChecked(visible);
}


void MainWindow::on_actionWorld_settings_triggered(bool checked)
{
    dock_WldSettingsBox->setVisible(checked);
    if(checked) dock_WldSettingsBox->raise();
}



void WorldSettingsBox::setCurrentWorldSettings()
{
    world_settings_lock_fields=true;
    int WinType = mw()->activeChildWindow();
    if(WinType==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;
        WriteToLog(QtDebugMsg, "-> Set Worldmap settings");

        WriteToLog(QtDebugMsg, "-> setTitle");
        ui->WLD_Title->setText( edit->WldData.EpisodeTitle );

        WriteToLog(QtDebugMsg, "-> setText");
        ui->WLD_AutostartLvl->setText( edit->WldData.IntroLevel_file );

        ui->WLD_Stars->setValue( edit->WldData.stars );

        ui->WLD_NoWorldMap->setChecked( edit->WldData.HubStyledWorld );
        mw()->ui->actionWLDDisableMap->setChecked( edit->WldData.HubStyledWorld );
        ui->WLD_RestartLevel->setChecked( edit->WldData.restartlevel );
        mw()->ui->actionWLDFailRestart->setChecked( edit->WldData.restartlevel );

        ui->WLD_Credirs->setText( edit->WldData.authors );

        WriteToLog(QtDebugMsg, "-> Character List");
        //clear character list
        while(!WLD_CharacterCheckBoxes.isEmpty())
        {
            QMap<QCheckBox *, int>::iterator it = WLD_CharacterCheckBoxes.begin();
            delete it.key();
            WLD_CharacterCheckBoxes.erase(it);
        }

        WriteToLog(QtDebugMsg, "-> Clear Menu");
        //ui->WLD_DisableCharacters

        //Create absence data
        if(edit->WldData.nocharacter.size() < mw()->configs.characters.size())
        {
            for(int i=0; i<=mw()->configs.characters.size()-edit->WldData.nocharacter.size(); i++ )
                edit->WldData.nocharacter.push_back(false);
        }

        for(int i = 0; i < mw()->configs.characters.size(); ++i){
            QCheckBox* cur = new QCheckBox(mw()->configs.characters[i].name);
            if(i < edit->WldData.nocharacter.size()){
                cur->setChecked(edit->WldData.nocharacter[i]);
            }
            WLD_CharacterCheckBoxes[cur] = mw()->configs.characters[i].id;
            connect(cur, SIGNAL(clicked(bool)), this, SLOT(characterActivated(bool)));
            ui->WLD_DisableCharacters->layout()->addWidget(cur);
        }

        WriteToLog(QtDebugMsg, "-> Done");
    }
    world_settings_lock_fields=false;
}


void WorldSettingsBox::characterActivated(bool checked)
{
    if(mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;

        QCheckBox* ch = qobject_cast<QCheckBox*>(sender());
        if(!ch)
            return;
        int ind = mw()->configs.getCharacterI(WLD_CharacterCheckBoxes[ch]);
        if(ind==-1)
            return;
        QList<QVariant> chData;
        chData << WLD_CharacterCheckBoxes[ch] << checked;
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_CHARACTER, chData);
        edit->WldData.nocharacter[ind] = checked;
    }
}

void MainWindow::on_actionWLDProperties_triggered()
{
    ui->actionWorld_settings->trigger();
}

void WorldSettingsBox::on_WLD_Title_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_Title->isModified()) return;
    ui->WLD_Title->setModified(false);

    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;

        QList<QVariant> var;
        var << edit->WldData.EpisodeTitle << ui->WLD_Title->text();
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_WORLDTITLE, var);
        edit->WldData.EpisodeTitle = ui->WLD_Title->text();
        edit->WldData.modified = true;
        edit->setWindowTitle(ui->WLD_Title->text() =="" ? edit->userFriendlyCurrentFile() : ui->WLD_Title->text() );
    }
}

void WorldSettingsBox::on_WLD_NoWorldMap_clicked(bool checked)
{
    if(world_settings_lock_fields) return;
    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_HUB, checked);

        mw()->ui->actionWLDDisableMap->setChecked(checked);
        edit->WldData.HubStyledWorld = checked;
        edit->WldData.modified = true;
    }
}

void MainWindow::on_actionWLDDisableMap_triggered(bool checked)
{
    if(dock_WldSettingsBox->world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        WorldEdit * edit = activeWldEditWin();
        if(!edit) return;
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_HUB, checked);

        dock_WldSettingsBox->ui->WLD_NoWorldMap->setChecked(checked);
        edit->WldData.HubStyledWorld = checked;
        edit->WldData.modified = true;
    }
}


void WorldSettingsBox::on_WLD_RestartLevel_clicked(bool checked)
{
    if(world_settings_lock_fields) return;
    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;

        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_RESTARTAFTERFAIL, checked);

        mw()->ui->actionWLDFailRestart->setChecked(checked);
        edit->WldData.restartlevel = checked;
        edit->WldData.modified = true;
    }
}
void MainWindow::on_actionWLDFailRestart_triggered(bool checked)
{
    if(dock_WldSettingsBox->world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        WorldEdit * edit = activeWldEditWin();
        if(!edit) return;
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_RESTARTAFTERFAIL, checked);

        dock_WldSettingsBox->ui->WLD_RestartLevel->setChecked(checked);
        edit->WldData.restartlevel = checked;
        edit->WldData.modified = true;
    }
}

/*
void MainWindow::on_WLD_AutostartLvl_textEdited(const QString &arg1)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->WldData.autolevel = arg1;
        activeWldEditWin()->WldData.modified = true;
    }
}
*/


void WorldSettingsBox::on_WLD_AutostartLvl_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_AutostartLvl->isModified()) return;
    ui->WLD_AutostartLvl->setModified(false);

    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QVariant> var;
        var << edit->WldData.IntroLevel_file << ui->WLD_AutostartLvl->text();
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_INTROLEVEL, var);
        edit->WldData.IntroLevel_file = ui->WLD_AutostartLvl->text();
        edit->WldData.modified = true;
    }
}


void WorldSettingsBox::on_WLD_AutostartLvlBrowse_clicked()
{
    if(world_settings_lock_fields) return;

    QString dirPath;
    if(mw()->activeChildWindow()==3)
    {
        dirPath = mw()->activeWldEditWin()->WldData.path;
    }
    else
        return;

    LevelFileList levelList(dirPath, ui->WLD_AutostartLvl->text());
    if( levelList.exec() == QDialog::Accepted )
    {
        if(mw()->activeChildWindow()==3){
            ui->WLD_AutostartLvl->setText(levelList.SelectedFile);
            ui->WLD_AutostartLvl->setModified(true);
            on_WLD_AutostartLvl_editingFinished();
        }
    }

}

void WorldSettingsBox::on_WLD_Stars_valueChanged(int arg1)
{
    if(world_settings_lock_fields) return;
    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QVariant> var;
        var << edit->WldData.stars << arg1;
        edit->scene->addChangeWorldSettingsHistory(HistorySettings::SETTING_TOTALSTARS, var);
        edit->WldData.stars = arg1;
        edit->WldData.modified = true;
    }
}


void WorldSettingsBox::on_WLD_Credirs_textChanged()
{
    if(world_settings_lock_fields) return;

    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;

        edit->WldData.authors = ui->WLD_Credirs->toPlainText();
        edit->WldData.modified = true;
    }
}



/********************************Star counter begin**************************************************/
long WorldSettingsBox::StarCounter_checkLevelFile(QString FilePath, QStringList &exists)
{
    QRegExp lvlext = QRegExp(".*\\.(lvl|lvlx)$");
    lvlext.setCaseSensitivity(Qt::CaseInsensitive);
    LevelData getLevelHead;
    long starCount = 0;
    getLevelHead.stars = 0;

    if( lvlext.exactMatch(FilePath) )
    {
        getLevelHead = FileFormats::OpenLevelFile(FilePath);
        if( !getLevelHead.ReadFileValid ) return 0;

        //qDebug() << "world "<< getLevelHead.stars << getLevelHead.filename;

        int foundStars=0;
        //Mark all stars
        for(int q=0; q< getLevelHead.npc.size(); q++)
        {
           int id = MainWinConnect::pMainWin->configs.getNpcI(getLevelHead.npc[q].id);
           if(id<0) continue;
           getLevelHead.npc[q].is_star = MainWinConnect::pMainWin->configs.main_npc[id].is_star;
           if((getLevelHead.npc[q].is_star)&&(!getLevelHead.npc[q].friendly))
                foundStars++;
        }
        starCount += foundStars;//getLevelHead.stars;

        for(int i=0;i<getLevelHead.doors.size(); i++)
        {
            if(!getLevelHead.doors[i].lname.isEmpty())
            {
                QString FilePath_W = getLevelHead.path+"/"+getLevelHead.doors[i].lname;

                if(!FilePath_W.endsWith(".lvl", Qt::CaseInsensitive)&&
                   !FilePath_W.endsWith(".lvlx", Qt::CaseInsensitive))
                   FilePath_W.append(".lvl");

                if(!QFileInfo(FilePath_W).exists()) continue;

                if(!exists.contains(FilePath_W))
                {
                    exists.push_back(FilePath_W);
                }
                else continue;
                //qDebug() << "warp "<<getLevelHead.stars << getLevelHead.filename;

                starCount += StarCounter_checkLevelFile(FilePath_W, exists);

                qApp->processEvents();
                //qDebug() << "starCount "<<starCount;
            }
        }
    }
    return starCount;
}

void WorldSettingsBox::on_WLD_DoCountStars_clicked()
{
    if(world_settings_lock_fields) return;

    //Count stars of all used levels on this world map

    QString dirPath;
    long starzzz=0;
    bool introCounted=false;

    if (mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;

        QString __backUP;
        __backUP = ui->WLD_DoCountStars->text();

        ui->WLD_DoCountStars->setEnabled(false);
        ui->WLD_DoCountStars->setText(tr("Counting..."));


        dirPath = edit->WldData.path;

        //Stop animations to increase performance
        edit->scene->stopAnimation();

        QProgressDialog progress(tr("Counting stars of placed levels"), tr("Abort"), 0, edit->WldData.levels.size(), mw());
             progress.setWindowTitle(tr("Counting stars..."));
             progress.setWindowModality(Qt::WindowModal);
             progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
             progress.setFixedSize(progress.size());
             progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
             progress.setMinimumDuration(0);
             progress.show();

        QStringList LevelAlreadyChecked;

        //qDebug() << "total " << starzzz;

        for(int i=0; i<edit->WldData.levels.size() || !introCounted; i++)
        {
            //Attempt to read stars quantity of level:

            QString FilePath;

            if(introCounted)
            {
                FilePath = dirPath+"/"+edit->WldData.levels[i].lvlfile;
                if(edit->WldData.levels[i].lvlfile.isEmpty()) continue;
            }
            else
            {
                FilePath = dirPath+"/"+edit->WldData.IntroLevel_file;
                i--;
                introCounted=true;
                if(FilePath.isEmpty()) continue;
            }

            if(!FilePath.endsWith(".lvl", Qt::CaseInsensitive)&&
               !FilePath.endsWith(".lvlx", Qt::CaseInsensitive))
               FilePath.append(".lvl");

            if(!QFileInfo(FilePath).exists()) continue;

            if(!LevelAlreadyChecked.contains(FilePath))
            {
                LevelAlreadyChecked.push_back(FilePath);
            }
            else continue;

            progress.setValue(i<0?1:i);
            starzzz += StarCounter_checkLevelFile(FilePath, LevelAlreadyChecked);
            //qDebug() << "starzzz " << starzzz;

            if(progress.wasCanceled()) break;
            qApp->processEvents();
        }

        //Start animations again
        if(edit->scene->opts.animationEnabled)
            edit->scene->startAnimation();

        ui->WLD_DoCountStars->setEnabled(true);
        ui->WLD_DoCountStars->setText(__backUP);

        if(progress.wasCanceled()) return;
        ui->WLD_Stars->setValue(starzzz);
        progress.close();
    }
}
/********************************Star counter End**************************************************/

