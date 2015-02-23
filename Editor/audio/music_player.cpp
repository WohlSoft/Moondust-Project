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

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <common_features/mainwinconnect.h>

#include "sdl_music_player.h"
#include "music_player.h"
#include <main_window/dock/lvl_sctc_props.h>

QString LvlMusPlay::currentCustomMusic;


QString LvlMusPlay::currentMusicPath;
QString LvlMusPlay::lastMusicPath;


long LvlMusPlay::currentMusicId=0;
long LvlMusPlay::currentWldMusicId=0;
long LvlMusPlay::currentSpcMusicId=0;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset=false;
int LvlMusPlay::musicType=LvlMusPlay::LevelMusic;


void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    setMusic(checked);
}

void LvlMusPlay::setMusic(LvlMusPlay::MusicType mt, unsigned long id, QString cmus)
{
    QString root = ".";
    MainWindow  *mw = MainWinConnect::pMainWin;
    if(!mw) return;

    if(id==0)
    {
        setNoMusic();
        return;
    }

    if(mw->activeChildWindow()==1)
    {
        if(mw->activeLvlEditWin()!=NULL)
            root = mw->activeLvlEditWin()->LvlData.path+"/";
    }
    else
    if(mw->activeChildWindow()==3)
    {
        if(mw->activeWldEditWin()!=NULL)
            root = mw->activeWldEditWin()->WldData.path+"/";
    }

    //Force correction of Windows paths into UNIX style
    cmus.replace('\\', '/');

    switch(mt)
    {
        case LevelMusic:
            if(id==mw->configs.music_custom_id)
            {
                WriteToLog(QtDebugMsg, QString("get Custom music path"));
                currentMusicPath = root + cmus;
            }
            else
            {
                WriteToLog(QtDebugMsg, QString("get standart music path (level)"));
                QString musicFile;
                long j = mw->configs.getMusLvlI(id);
                if(j>=0)
                {
                    if(id==mw->configs.main_music_lvl[j].id)
                    {
                        musicFile = mw->configs.main_music_lvl[j].file;
                    }
                }
                currentMusicPath = mw->configs.dirs.music + musicFile;
            }
            break;
        case WorldMusic:
            if(id==mw->configs.music_w_custom_id)
            {
                WriteToLog(QtDebugMsg, QString("get Custom music path"));
                currentMusicPath = root + cmus;
            }
            else
            {
                WriteToLog(QtDebugMsg, QString("get standart music path (world)"));
                QString musicFile;
                long j = mw->configs.getMusWldI(id);
                if(j>=0)
                {
                    if(id==mw->configs.main_music_wld[j].id)
                    {
                        musicFile = mw->configs.main_music_wld[j].file;
                    }
                }
                currentMusicPath = mw->configs.dirs.music + musicFile;
            }
            break;
        case SpecialMusic:
            {
                WriteToLog(QtDebugMsg, QString("get standart music path (special)"));
                QString musicFile;
                long j = mw->configs.getMusSpcI(id);
                if(j>=0)
                {
                    if(id==mw->configs.main_music_spc[j].id)
                    {
                        musicFile = mw->configs.main_music_spc[j].file;
                    }
                }
                currentMusicPath = mw->configs.dirs.music + musicFile;
            }
            break;
    }

    WriteToLog(QtDebugMsg, QString("path is %1").arg(currentMusicPath));

    QFileInfo mus(currentMusicPath);
    if((!mus.exists())||(!mus.isFile()))
        currentMusicPath.clear();
}

void LvlMusPlay::setNoMusic()
{
    currentMusicPath.clear();
}

void LvlMusPlay::updateMusic()
{
    int w_id = MainWinConnect::pMainWin->activeChildWindow();

    if(w_id==1)
    {
        MainWinConnect::pMainWin->dock_LvlSectionProps->loadMusic();
    }
    else
    if(w_id==3)
    {
        WorldEdit *w = MainWinConnect::pMainWin->activeWldEditWin();
        if(!w) return;
        setMusic(LvlMusPlay::WorldMusic, w->currentMusic, "");
        updatePlayerState(true);
    }
}

void LvlMusPlay::updatePlayerState(bool playing)
{
    if(!LvlMusPlay::musicForceReset)
    {
        if(
            (!lastMusicPath.isNull()) &&
            (lastMusicPath == currentMusicPath)&&
            (musicButtonChecked == playing)
                )
                return;

    }
    else
    {
        musicForceReset=false;
    }

    if((playing==false) || (currentMusicPath.isNull()))
    {
        PGE_MusPlayer::MUS_stopMusic();
    }
    else
    {
        PGE_MusPlayer::MUS_stopMusic();
        PGE_MusPlayer::MUS_openFile( currentMusicPath );
        PGE_MusPlayer::MUS_changeVolume(MainWinConnect::pMainWin->musicVolume());
        PGE_MusPlayer::MUS_playMusic();
    }

    lastMusicPath = currentMusicPath;
    musicButtonChecked  = playing;
}

void LvlMusPlay::stopMusic()
{
    setNoMusic();
    updatePlayerState(false);
}

/*******************************************************************************/
void MainWindow::setMusic(bool checked)
{
    checked = ui->actionPlayMusic->isChecked();

    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    LvlMusPlay::updatePlayerState(checked);
}

void MainWindow::setMusicButton(bool checked)
{
    ui->actionPlayMusic->setChecked(checked);
}

int MainWindow::musicVolume()
{
    if(muVol)
        return muVol->value();
    else
        return 128;
}

