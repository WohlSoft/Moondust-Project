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
#include "music_player.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <iostream>

namespace PGE_MusicPlayer
{
Mix_Music *play_mus = NULL;

    void MUS_stopMusic()
    {
        Mix_HaltMusic();
    }

    void MUS_playMusic()
    {
        if(play_mus)
        {
            if(Mix_PlayMusic(play_mus, -1)==-1)
            {
                qDebug() << QString("Mix_PlayMusic: %1\n").arg(Mix_GetError());
                // well, there's no music, but most games don't break without music...
            }

            qDebug() << QString("Music is %1").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            qDebug() << QString("Play nothing: Mix_PlayMusic: %1\n").arg(Mix_GetError());
        }
    }

    void MUS_changeVolume(int volume)
    {
        Mix_VolumeMusic(volume);
    }

    void MUS_openFile(QString musFile)
    {
        if(play_mus!=NULL) {Mix_FreeMusic(play_mus);play_mus=NULL;}
        play_mus = Mix_LoadMUS( musFile.toLocal8Bit() );
        if(!play_mus) {
            qDebug() << QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError());
        }

        //Print memory address of pointer
        //qDebug() << "Pointer is " << static_cast<void*>(&play_mus);

        Mix_MusicType type=Mix_GetMusicType(play_mus);
        qDebug() << QString("Music type: %1\n").arg(
                type==MUS_NONE?"MUS_NONE":
                type==MUS_CMD?"MUS_CMD":
                type==MUS_WAV?"MUS_WAV":
                /*type==MUS_MOD_MODPLUG?"MUS_MOD_MODPLUG":*/
                type==MUS_MOD?"MUS_MOD":
                type==MUS_MID?"MUS_MID":
                type==MUS_OGG?"MUS_OGG":
                type==MUS_MP3?"MUS_MP3":
                type==MUS_MP3_MAD?"MUS_MP3_MAD":
                type==MUS_FLAC?"MUS_FLAC":
                "Unknown");
    }
}

//QMediaPlaylist GlobalMusicPlayer::CurrentMusic;


void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
    LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentData().toInt();
    setMusic(checked);
}


void MainWindow::setMusic(bool checked=false)
{
    //using namespace PGE_MusicPlayer;
    QString dirPath;
    QString musicFile;
    QString musicFilePath;
    bool silent = true;
    unsigned int CurMusNum;
    checked = ui->actionPlayMusic->isChecked();

    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    if(!LvlMusPlay::musicForceReset)
    {
        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
        {
            if( ( LvlMusPlay::currentMusicId == ui->LVLPropsMusicNumber->currentData().toInt() ) &&
                    (LvlMusPlay::currentCustomMusic == ui->LVLPropsMusicCustom->text()) &&
                    (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()) ) return;
        }
        else
        if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
        {
            if(activeChildWindow()==3)
            {
                if((LvlMusPlay::currentWldMusicId == activeWldEditWin()->currentMusic)&&
                   (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()))
                    return;
            }

        }

    } else LvlMusPlay::musicForceReset=false;


    WriteToLog(QtDebugMsg, "-> New MediaPlayList");
    CurrentMusic.clear();


    if((LvlMusPlay::musicType==LvlMusPlay::LevelMusic)&&(activeChildWindow()!=1))
    {
        if(checked)
        {
            if(
                    (
                        (LvlMusPlay::currentMusicId>0)&&
                        (((unsigned long)LvlMusPlay::currentMusicId!=configs.music_custom_id))
                    )||
                     (
                         ((unsigned long)LvlMusPlay::currentMusicId == configs.music_custom_id)
                         &&(LvlMusPlay::currentCustomMusic!="")
                     )
              )
            {
                //MusicPlayer->play();
                PGE_MusicPlayer::MUS_playMusic();
                silent=false;
            }
            else
            {
                silent=true;
            }
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
                //MusicPlayer->stop();
            PGE_MusicPlayer::MUS_stopMusic();
            silent=true;
        }
        return;
    }

    if((LvlMusPlay::musicType==LvlMusPlay::WorldMusic)&&(activeChildWindow()!=3))
    {
        if(checked)
        {
            if(
                    (
                        (LvlMusPlay::currentWldMusicId>0)&&
                        (((unsigned long)LvlMusPlay::currentWldMusicId!=configs.music_w_custom_id))
                    )||
                     (
                         ((unsigned long)LvlMusPlay::currentWldMusicId == configs.music_w_custom_id)
                         &&(LvlMusPlay::currentCustomMusic!="")
                     )
              )
            {
                //MusicPlayer->play();
                PGE_MusicPlayer::MUS_playMusic();
                silent=false;
            }
            else
            {
                silent=true;
            }
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
                //MusicPlayer->stop();
                PGE_MusicPlayer::MUS_stopMusic();
            silent=true;
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        WriteToLog(QtDebugMsg, "Get Level File Path");
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else
    if(activeChildWindow()==3)
    {
        WriteToLog(QtDebugMsg, "Get World File Path");
        dirPath = activeWldEditWin()->WldData.path;
    }
    else
        return;

    WriteToLog(QtDebugMsg, "Check for Sielent");

    if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
    {
        silent = ((ui->LVLPropsMusicNumber->currentData().toInt() <= 0) // Music is sielent
                ||
           //Mute, if music ID not exist
          (ui->LVLPropsMusicNumber->currentData().toInt() > configs.main_music_lvl.size() ));
    }
    else
    if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
    {
        if(activeChildWindow()==3)
        {
            silent = ( activeWldEditWin()->currentMusic <=0 );
        }
    }
    else
        silent=true;


    WriteToLog(QtDebugMsg, "ifNot Sielent, play Music");

    if(!silent)
    {
        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
        {
            CurMusNum = ui->LVLPropsMusicNumber->currentData().toInt();
        }
        else
        {
            CurMusNum = (activeChildWindow()==3) ? activeWldEditWin()->currentMusic : 0;
        }

        musicFile="";

        if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
        {
            if(CurMusNum==configs.music_custom_id)
            {
                    WriteToLog(QtDebugMsg, QString("get Custom music path"));
                musicFile = ui->LVLPropsMusicCustom->text();
                musicFilePath = dirPath + "/" + musicFile;
            }
            else
            {
                    WriteToLog(QtDebugMsg, QString("get standart music path"));
                foreach(obj_music mus, configs.main_music_lvl)
                {
                    if(CurMusNum==mus.id)
                    {
                        musicFile = mus.file;
                        break;
                    }
                }
                dirPath = configs.dirs.music;
                musicFilePath = configs.dirs.music + musicFile;
                //QMessageBox::information(this, "test", "music is \n"+musicFile+"\n"+QString::number());
            }
        }
        if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
        {
            foreach(obj_music mus, configs.main_music_wld)
            {
                if(CurMusNum==mus.id)
                {
                    musicFile = mus.file;
                    break;
                }
            }
            dirPath = configs.dirs.music;
            musicFilePath = configs.dirs.music + musicFile;
        }




        if(checked)
        {
            if( (QFile::exists(musicFilePath)) && (QFileInfo(musicFilePath)).isFile() )
            {
                WriteToLog(QtDebugMsg, QString("Set music player -> addMedia"));
                //CurrentMusic.addMedia(QUrl::fromLocalFile( musicFilePath ));
                //CurrentMusic.setPlaybackMode(QMediaPlaylist::Loop);
                WriteToLog(QtDebugMsg, QString("Set music player -> stop Current"));
                //MusicPlayer->stop();
                PGE_MusicPlayer::MUS_stopMusic();
                WriteToLog(QtDebugMsg, QString("Set music player -> set PlayList"));
                //MusicPlayer->setPlaylist(&(CurrentMusic));
                PGE_MusicPlayer::MUS_openFile( musicFilePath );
                WriteToLog(QtDebugMsg, QString("Set music player -> setVolme and play"));
                //MusicPlayer->setVolume(muVol->value());
                PGE_MusicPlayer::MUS_changeVolume(muVol->value());
                PGE_MusicPlayer::MUS_playMusic();
                //MusicPlayer->play();
            }
            else
                PGE_MusicPlayer::MUS_stopMusic();
                //MusicPlayer->stop();
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by checker"));
            //MusicPlayer->stop();
            PGE_MusicPlayer::MUS_stopMusic();
        }
    }
    else
    {
        WriteToLog(QtDebugMsg, QString("Set music player -> Stop by sielent"));
        //MusicPlayer->stop();
        PGE_MusicPlayer::MUS_stopMusic();
    }

    if(LvlMusPlay::musicType==LvlMusPlay::LevelMusic)
    {
        LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
        LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentData().toInt();
    }
    else
    if(LvlMusPlay::musicType==LvlMusPlay::WorldMusic)
    {
        LvlMusPlay::currentWldMusicId = (activeChildWindow()==3) ? activeWldEditWin()->currentMusic : 0;
    }
    LvlMusPlay::musicButtonChecked  = ui->actionPlayMusic->isChecked();
}

void MainWindow::setMusicButton(bool checked)
{
    ui->actionPlayMusic->setChecked(checked);
}
