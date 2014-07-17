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

QMediaPlaylist GlobalMusicPlayer::CurrentMusic;


void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
    LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentData().toInt();
    setMusic(checked);
}


void MainWindow::setMusic(bool checked)
{
    QString dirPath;
    QString musicFile;
    QString musicFilePath;
    bool silent;
    unsigned int CurMusNum;

    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    if(!LvlMusPlay::musicForceReset)
    {
    if( ( LvlMusPlay::currentMusicId == ui->LVLPropsMusicNumber->currentData().toInt() ) &&
            (LvlMusPlay::currentCustomMusic == ui->LVLPropsMusicCustom->text()) &&
            (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()) ) return;
    } else LvlMusPlay::musicForceReset=false;


    WriteToLog(QtDebugMsg, "-> New MediaPlayList");
    GlobalMusicPlayer::CurrentMusic.clear();


    if(activeChildWindow()!=1)
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
                MusicPlayer->play();
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
                MusicPlayer->stop();
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
        return;

    WriteToLog(QtDebugMsg, "Check for Sielent");

    if((ui->LVLPropsMusicNumber->currentData().toInt() <= 0) // Music is sielent
            ||
       //Mute, if music ID not exist
      (ui->LVLPropsMusicNumber->currentData().toInt() > configs.main_music_lvl.size() ))
        silent=true;
    else
        silent=false;

    WriteToLog(QtDebugMsg, "ifNot Sielent, play Music");

    if(!silent)
    {
        CurMusNum = ui->LVLPropsMusicNumber->currentData().toInt();
        musicFile="";

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

        if(checked)
        {
            if( (QFile::exists(musicFilePath)) && (QFileInfo(musicFilePath)).isFile() )
            {
                WriteToLog(QtDebugMsg, QString("Set music player -> addMedia"));
                GlobalMusicPlayer::CurrentMusic.addMedia(QUrl::fromLocalFile( musicFilePath ));
                GlobalMusicPlayer::CurrentMusic.setPlaybackMode(QMediaPlaylist::Loop);
                WriteToLog(QtDebugMsg, QString("Set music player -> stop Current"));
                MusicPlayer->stop();
                WriteToLog(QtDebugMsg, QString("Set music player -> set PlayList"));
                MusicPlayer->setPlaylist(&(GlobalMusicPlayer::CurrentMusic));
                WriteToLog(QtDebugMsg, QString("Set music player -> setVolme and play"));
                MusicPlayer->setVolume(muVol->value());
                MusicPlayer->play();
            }
            else
                MusicPlayer->stop();
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by checker"));
            MusicPlayer->stop();
        }
    }
    else
    {
        WriteToLog(QtDebugMsg, QString("Set music player -> Stop by sielent"));
        MusicPlayer->stop();
    }

    LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
    LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentData().toInt();
    LvlMusPlay::musicButtonChecked  = ui->actionPlayMusic->isChecked();
}

void MainWindow::setMusicButton(bool checked)
{
    ui->actionPlayMusic->setChecked(checked);
}
