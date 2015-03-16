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

#include <QtDebug>

#include "sdl_music_player.h"
#include <common_features/logger.h>

// //////////////////////// Music Player //////////////////////////////////////

void PGE_MusPlayer::setVolume(int volume)
{
    MUS_changeVolume(volume);
}

#ifdef USE_QMEDIAPLAYER
bool PGE_MusPlayer::isMediaPlayer=false;
QMediaPlayer * PGE_MusPlayer::musicPlayer=NULL;
QMediaPlaylist * PGE_MusPlayer::playList=NULL;
#elif USE_SDL_MIXER
Mix_Music *PGE_MusPlayer::play_mus = NULL;
#endif

int PGE_MusPlayer::volume=100;
int PGE_MusPlayer::sRate=44100;
QString PGE_MusPlayer::current="";



void PGE_MusPlayer::MUS_stopMusic()
{
    #ifdef USE_QMEDIAPLAYER
        musicPlayer->stop();
    #elif USE_SDL_MIXER
        Mix_HaltMusic();
    #endif
}

void PGE_MusPlayer::MUS_playMusic()
{
    #ifdef USE_QMEDIAPLAYER
        qDebug() << QString("Play Music (QMediaPlayer)");
        musicPlayer->play();
    #elif USE_SDL_MIXER
        if(play_mus)
        {
            qDebug() << QString("Play Music (SDL2_mixer)");
            if(Mix_PlayingMusic()==0)
            {
                if(Mix_PlayMusic(play_mus, -1)==-1)
                {
                    qDebug() << QString("Mix_PlayMusic: %1").arg(Mix_GetError());
                    // well, there's no music, but most games don't break without music...
                }
            }
            qDebug() << QString("Music is %1").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            qDebug() << QString("Play nothing: Mix_PlayMusic: %1").arg(Mix_GetError());
        }
    #endif
}

void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    qDebug() << QString("Volume changed %1").arg(vlm);
    volume = vlm;
    #ifdef USE_QMEDIAPLAYER
        if(isMediaPlayer)
        {
            if(volume>100) volume=100;
            musicPlayer->setVolume(volume);
        }
    #elif USE_SDL_MIXER
        Mix_VolumeMusic(volume);
    #endif
}

void PGE_MusPlayer::setSampleRate(int sampleRate=44100)
{
    sRate=sampleRate;
    #ifdef USE_SDL_MIXER
    qDebug() << "Set sample rate to:"<<sampleRate;
    //Mix_CloseAudio();
    if(Mix_OpenAudio(sRate, AUDIO_S16, 2, 4096)<0)
    {
        WriteToLog(QtWarningMsg, QString("Can't open audio: %1").arg(Mix_GetError()));
    }
    if(Mix_AllocateChannels(16)<0)
    {
        WriteToLog(QtWarningMsg, QString("Can't allocate channels: %1").arg(Mix_GetError()));
    }
    #endif
}

int PGE_MusPlayer::sampleRate()
{
    return sRate;
}

int PGE_MusPlayer::currentVolume()
{
    return volume;
}


void PGE_MusPlayer::MUS_openFile(QString musFile)
{
    if(current==musFile)
    {
        #ifdef USE_SDL_MIXER
        if(Mix_PlayingMusic()==1)
                return;
        #endif
    }

    #ifdef USE_QMEDIAPLAYER
        if(musicPlayer!=NULL)
        {
            musicPlayer->stop();
            delete musicPlayer;
            delete playList;
            isMediaPlayer=false;
        }
        musicPlayer = new QMediaPlayer();
        playList = new QMediaPlaylist();
        playList->clear();
        playList->addMedia(QUrl::fromLocalFile( musFile ));
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        musicPlayer->setPlaylist(playList);
        current = musFile;
    #elif USE_SDL_MIXER
        if(play_mus!=NULL)
        {
            Mix_HaltMusic();
            Mix_FreeMusic(play_mus);
            play_mus=NULL;
        }

        play_mus = Mix_LoadMUS( musFile.toUtf8() );
        if(!play_mus)
            qDebug() << QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError());
        else
            current = musFile;

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
                type==9?"MUS_SPC":
                "Unknown");
#endif
}

void PGE_MusPlayer::MUS_freeStream()
{
    #ifdef USE_SDL_MIXER
    Mix_HaltMusic();
    Mix_FreeMusic(play_mus);
    play_mus=NULL;
    #endif
}

// //////////////////////// Sound Player //////////////////////////////////////



#ifdef USE_QMEDIAPLAYER
QMediaPlayer * PGE_Sounds::mp3Play=NULL;
#elif USE_SDL_MIXER
Mix_Chunk *PGE_Sounds::sound = NULL;
#endif

QString PGE_Sounds::current = "";

void PGE_Sounds::SND_PlaySnd(QString sndFile)
{
    if(current!=sndFile)
    {
        #ifdef USE_QMEDIAPLAYER
        if(mp3Play) { mp3Play->stop(); delete mp3Play; mp3Play=NULL; }
        #elif USE_SDL_MIXER
        if(sound) { Mix_FreeChunk(sound); sound=NULL; }
        sound = Mix_LoadWAV(sndFile.toUtf8() );
        if(!sound)
            qDebug() << QString("Mix_LoadWAV: %1").arg(SDL_GetError());
        else
            Mix_VolumeChunk(sound, MIX_MAX_VOLUME);
        #endif
        current = sndFile;
    }

    #ifdef USE_QMEDIAPLAYER
    qDebug() << QString("Play Sound (QMediaPlayer)");
    if(!mp3Play)
    {
        qDebug() << QString("QMediaPlayer is null");
    }
    else
        mp3Play->play();
    #elif USE_SDL_MIXER

    WriteToLog(QtDebugMsg, QString("Play Sound (SDL2_mixer)"));
    if(Mix_PlayChannel( -1, sound, 0 )==-1)
    {
        qDebug() << QString("Mix_PlayChannel: %1").arg(SDL_GetError());
    }

    #endif
}
