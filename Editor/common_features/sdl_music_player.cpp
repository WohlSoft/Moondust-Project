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

#include <QtDebug>

#include "sdl_music_player.h"

void PGE_MusPlayer::setVolume(int volume)
{
    MUS_changeVolume(volume);
}


Mix_Music *PGE_MusPlayer::play_mus = NULL;

int PGE_MusPlayer::volume=100;

bool PGE_MusPlayer::isMediaPlayer=false;
QMediaPlayer * PGE_MusPlayer::musicPlayer=NULL;
QMediaPlaylist * PGE_MusPlayer::playList=NULL;

void PGE_MusPlayer::MUS_stopMusic()
{
    if(isMediaPlayer)
        musicPlayer->stop();
    else
        Mix_HaltMusic();
}

void PGE_MusPlayer::MUS_playMusic()
{
    if(isMediaPlayer)
        musicPlayer->play();
    else
    {
        if(play_mus)
        {
            if(Mix_PlayMusic(play_mus, -1)==-1)
            {
                qDebug() << QString("Mix_PlayMusic: %1").arg(Mix_GetError());
                // well, there's no music, but most games don't break without music...
            }

            qDebug() << QString("Music is %1").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            qDebug() << QString("Play nothing: Mix_PlayMusic: %1").arg(Mix_GetError());
        }
    }
}

void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    volume = vlm;
    if(isMediaPlayer)
    {
        if(volume>100) volume=100;
        musicPlayer->setVolume(volume);
    }
    else
        Mix_VolumeMusic(volume);

}

void PGE_MusPlayer::MUS_openFile(QString musFile)
{
    if(play_mus!=NULL)
    {
        Mix_FreeMusic(play_mus);
        play_mus=NULL;
    }

    if(isMediaPlayer) if(musicPlayer!=NULL)
    {
        musicPlayer->stop();
        delete musicPlayer;
        delete playList;
        isMediaPlayer=false;
    }

    //Play MP3-filed with QMediaPlayer
    if(musFile.endsWith(".mp3", Qt::CaseInsensitive))
    {
        isMediaPlayer=true;
        musicPlayer = new QMediaPlayer();
        playList = new QMediaPlaylist();
        playList->clear();
        playList->addMedia(QUrl::fromLocalFile( musFile ));
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        musicPlayer->setPlaylist(playList);
    }
    else
    {
        isMediaPlayer=false;

        play_mus = Mix_LoadMUS( musFile.toUtf8() );
        if(!play_mus) {
            qDebug() << QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError());
        }

        //Print memory address of pointer
        //qDebug() << "Pointer is " << static_cast<void*>(&play_mus);

        Mix_MusicType type=Mix_GetMusicType(play_mus);
        qDebug() << QString("Music type: %1").arg(
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


