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
#include <ui_lvl_sctc_props.h>

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



void MainWindow::setMusic(bool checked=false)
{
//    QString dirPath;
//    QString musicFile;
//    QString musicFilePath;
//    bool silent = true;
//    unsigned int CurMusNum;
    checked = ui->actionPlayMusic->isChecked();

    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    if(!LvlMusPlay::musicForceReset)
    {
//        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
//        {
//            if( ( LvlMusPlay::currentMusicId == dock_LvlSectionProps->ui->LVLPropsMusicNumber->currentData().toInt() ) &&
//                    (LvlMusPlay::currentCustomMusic == dock_LvlSectionProps->ui->LVLPropsMusicCustom->text()) &&
//                    (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()) ) return;
//        }
//        else
//        if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
//        {
//            if(activeChildWindow()==3)
//            {
//                if((LvlMusPlay::currentWldMusicId == activeWldEditWin()->currentMusic)&&
//                   (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()))
//                    return;
//            }

//        }
        if(
            (!LvlMusPlay::lastMusicPath.isNull()) &&
            (LvlMusPlay::lastMusicPath==LvlMusPlay::currentMusicPath)&&
            (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked())
                )
                return;

    } else LvlMusPlay::musicForceReset=false;


    if((checked==false) || (LvlMusPlay::currentMusicPath.isNull()))
    {
        PGE_MusPlayer::MUS_stopMusic();
    }
    else
    {
        PGE_MusPlayer::MUS_stopMusic();
        PGE_MusPlayer::MUS_openFile( LvlMusPlay::currentMusicPath );
        PGE_MusPlayer::MUS_changeVolume(muVol->value());
        PGE_MusPlayer::MUS_playMusic();
    }

//    if((LvlMusPlay::musicType==LvlMusPlay::LevelMusic)&&(activeChildWindow()!=1))
//    {
//        if(checked)
//        {
//            if(
//                    (
//                        (LvlMusPlay::currentMusicId>0)&&
//                        (((unsigned long)LvlMusPlay::currentMusicId!=configs.music_custom_id))
//                    )||
//                     (
//                         ((unsigned long)LvlMusPlay::currentMusicId == configs.music_custom_id)
//                         &&(LvlMusPlay::currentCustomMusic!="")
//                     )
//              )
//            {
//                //MusicPlayer->play();
//                PGE_MusPlayer::MUS_playMusic();
//                silent=false;
//            }
//            else
//            {
//                silent=true;
//            }
//        }
//        else
//        {
//            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
//            PGE_MusPlayer::MUS_stopMusic();
//            silent=true;
//        }
//        return;
//    }

//    if((LvlMusPlay::musicType==LvlMusPlay::WorldMusic)&&(activeChildWindow()!=3))
//    {
//        if(checked)
//        {
//            if(
//                    (
//                        (LvlMusPlay::currentWldMusicId>0)&&
//                        (((unsigned long)LvlMusPlay::currentWldMusicId!=configs.music_w_custom_id))
//                    )||
//                     (
//                         ((unsigned long)LvlMusPlay::currentWldMusicId == configs.music_w_custom_id)
//                         &&(LvlMusPlay::currentCustomMusic!="")
//                     )
//              )
//            {
//                //MusicPlayer->play();
//                PGE_MusPlayer::MUS_playMusic();
//                silent=false;
//            }
//            else
//            {
//                silent=true;
//            }
//        }
//        else
//        {
//            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
//            PGE_MusPlayer::MUS_stopMusic();
//            silent=true;
//        }
//        return;
//    }

//    if(activeChildWindow()==1)
//    {
//        WriteToLog(QtDebugMsg, "Get Level File Path");
//        dirPath = activeLvlEditWin()->LvlData.path;
//    }
//    else
//    if(activeChildWindow()==3)
//    {
//        WriteToLog(QtDebugMsg, "Get World File Path");
//        dirPath = activeWldEditWin()->WldData.path;
//    }
//    else
//        return;

//    WriteToLog(QtDebugMsg, "Check for Sielent");

//    if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
//    {
//        silent = ((dock_LvlSectionProps->ui->LVLPropsMusicNumber->currentData().toInt() <= 0) // Music is sielent
//                ||
//           //Mute, if music ID not exist
//          (dock_LvlSectionProps->ui->LVLPropsMusicNumber->currentData().toInt() > configs.main_music_lvl.size() ));
//    }
//    else
//    if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
//    {
//        if(activeChildWindow()==3)
//        {
//            silent = ( activeWldEditWin()->currentMusic <=0 );
//        }
//    }
//    else
//        silent=true;


//    WriteToLog(QtDebugMsg, "ifNot Sielent, play Music");

//    if(!silent)
//    {
//        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
//        {
//            CurMusNum = dock_LvlSectionProps->ui->LVLPropsMusicNumber->currentData().toInt();
//        }
//        else
//        {
//            CurMusNum = (activeChildWindow()==3) ? activeWldEditWin()->currentMusic : 0;
//        }

//        musicFile="";

//        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
//        {
//            if(CurMusNum==configs.music_custom_id)
//            {
//                    WriteToLog(QtDebugMsg, QString("get Custom music path"));
//                musicFile = dock_LvlSectionProps->ui->LVLPropsMusicCustom->text();
//                musicFilePath = dirPath + "/" + musicFile;
//            }
//            else
//            {
//                    WriteToLog(QtDebugMsg, QString("get standart music path"));
//                foreach(obj_music mus, configs.main_music_lvl)
//                {
//                    if(CurMusNum==mus.id)
//                    {
//                        musicFile = mus.file;
//                        break;
//                    }
//                }
//                dirPath = configs.dirs.music;
//                musicFilePath = configs.dirs.music + musicFile;
//                //QMessageBox::information(this, "test", "music is \n"+musicFile+"\n"+QString::number());
//            }
//        }
//        if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
//        {
//            foreach(obj_music mus, configs.main_music_wld)
//            {
//                if(CurMusNum==mus.id)
//                {
//                    musicFile = mus.file;
//                    break;
//                }
//            }
//            dirPath = configs.dirs.music;
//            musicFilePath = configs.dirs.music + musicFile;
//        }




//        if(checked)
//        {
//            if( (QFile::exists(musicFilePath)) && (QFileInfo(musicFilePath)).isFile() )
//            {
//                //PGE_MusPlayer::MUS_stopMusic();
//                WriteToLog(QtDebugMsg, QString("Set music player -> set MusicFile"));
//                PGE_MusPlayer::MUS_openFile( musicFilePath );
//                WriteToLog(QtDebugMsg, QString("Set music player -> setVolme and play"));
//                PGE_MusPlayer::MUS_changeVolume(muVol->value());
//                PGE_MusPlayer::MUS_playMusic();
//            }
//            else
//                PGE_MusPlayer::MUS_stopMusic();
//        }
//        else
//        {
//            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by checker"));
//            PGE_MusPlayer::MUS_stopMusic();
//        }
//    }
//    else
//    {
//        WriteToLog(QtDebugMsg, QString("Set music player -> Stop by sielent"));
//        PGE_MusPlayer::MUS_stopMusic();
//    }

//    if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
//    {
//        LvlMusPlay::currentCustomMusic = dock_LvlSectionProps->ui->LVLPropsMusicCustom->text();
//        LvlMusPlay::currentMusicId = dock_LvlSectionProps->ui->LVLPropsMusicNumber->currentData().toInt();
//    }
//    else
//    if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
//    {
//        LvlMusPlay::currentWldMusicId = (activeChildWindow()==3) ? activeWldEditWin()->currentMusic : 0;
//    }

    LvlMusPlay::lastMusicPath=LvlMusPlay::currentMusicPath;

    LvlMusPlay::musicButtonChecked  = ui->actionPlayMusic->isChecked();
}

void MainWindow::setMusicButton(bool checked)
{
    ui->actionPlayMusic->setChecked(checked);
}

