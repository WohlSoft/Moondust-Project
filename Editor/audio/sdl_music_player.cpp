/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/app_path.h>


Mix_Music *PGE_MusPlayer::play_mus =    nullptr;
int        PGE_MusPlayer::volume =      100;
int        PGE_MusPlayer::sRate =       44100;
QString    PGE_MusPlayer::currentMusic = QString();

Mix_Chunk *PGE_SfxPlayer::sound = nullptr;
QString    PGE_SfxPlayer::currentSound = QString();

static     SDL_AudioSpec g_audioSpec;

void MixerX::initAudio(int frequency, Uint16 format, int channels, int chunksize)
{
    SDL_memset(&g_audioSpec, 0, sizeof(SDL_AudioSpec));
    g_audioSpec.freq = frequency;
    g_audioSpec.format = format;
    g_audioSpec.channels = static_cast<Uint8>(channels);
    g_audioSpec.size = static_cast<Uint32>(chunksize);
    qDebug() << "Set sample rate to:" << frequency;
    PGE_MusPlayer::sRate = frequency;

    if(Mix_OpenAudio(g_audioSpec.freq, AUDIO_S16, g_audioSpec.channels, static_cast<int>(g_audioSpec.size)) < 0)
        LogWarning(QString("Can't open audio: %1").arg(Mix_GetError()));

    if(Mix_AllocateChannels(16) < 0)
        LogWarning(QString("Can't allocate channels: %1").arg(Mix_GetError()));

    // TODO: Implement an ability to choose a timidity config
    Mix_SetTimidityCfg(QString(ApplicationPath + "/timidity/").toLocal8Bit().data());
}

int MixerX::sampleRate()
{
    return g_audioSpec.freq;
}

void MixerX::closeAudio()
{
    Mix_CloseAudio();
}


// //////////////////////// Music Player //////////////////////////////////////
void PGE_MusPlayer::changeVolume(int vlm)
{
    qDebug() << QString("Volume changed %1").arg(vlm);
    volume = vlm;
    Mix_VolumeMusicStream(play_mus, volume);
}

int PGE_MusPlayer::currentVolume()
{
    return volume;
}

void PGE_MusPlayer::setVolume(int volume)
{
    changeVolume(volume);
}



void PGE_MusPlayer::openFile(QString musFile)
{
    if(currentMusic == musFile)
    {
        if(Mix_PlayingMusicStream(play_mus) == 1)
            return;
    }

    if(play_mus != nullptr)
    {
        Mix_HaltMusicStream(play_mus);
        Mix_FreeMusic(play_mus);
        play_mus = nullptr;
    }

    play_mus = Mix_LoadMUS(musFile.toUtf8().data());
    if(!play_mus)
        qDebug() << QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError());
    else
        currentMusic = musFile;

    Mix_MusicType type = Mix_GetMusicType(play_mus);
    qDebug() << QString("Music type: %1").arg(
                 type == MUS_NONE ? "MUS_NONE" :
                 type == MUS_CMD ? "MUS_CMD" :
                 type == MUS_WAV ? "MUS_WAV" :
                 type == MUS_MOD ? "MUS_MOD" :
                 type == MUS_MID ? "MUS_MID" :
                 type == MUS_OGG ? "MUS_OGG" :
                 type == MUS_MP3 ? "MUS_MP3" :
                 type == MUS_FLAC ? "MUS_FLAC" :
#ifdef SDL_MIXER_X
                 type == MUS_GME ? "MUS_GME" :
                 type == MUS_ADLMIDI ? "MUS_ADLMIDI" :
#endif
                 "Unknown");
}

void PGE_MusPlayer::closeFile()
{
    Mix_HaltMusicStream(play_mus);
    Mix_FreeMusic(play_mus);
    play_mus = nullptr;
}


void PGE_MusPlayer::stop()
{
    Mix_HaltMusicStream(play_mus);
}

void PGE_MusPlayer::play()
{
    if(play_mus)
    {
        qDebug() << QString("Play Music (SDL2_mixer)");
        if(Mix_PlayingMusicStream(play_mus) == 0)
        {
            if(Mix_PlayMusic(play_mus, -1) == -1)
            {
                qDebug() << QString("Mix_PlayMusic: %1").arg(Mix_GetError());
                // well, there's no music, but most games don't break without music...
            }
        }
        qDebug() << QString("Music is %1").arg(Mix_PlayingMusicStream(play_mus) == 1 ? "Playing" : "Silence");
    }
    else
    {
        qDebug() << QString("Play nothing: Mix_PlayMusic: %1").arg(Mix_GetError());
    }
}


// //////////////////////// Sound Player //////////////////////////////////////

void PGE_SfxPlayer::playFile(QString sndFile)
{
    if(currentSound != sndFile)
    {
        if(sound)
        {
            Mix_FreeChunk(sound);
            sound = nullptr;
        }
        sound = Mix_LoadWAV(sndFile.toUtf8().data());
        if(!sound)
            qDebug() << QString("Mix_LoadWAV: %1").arg(SDL_GetError());
        else
            Mix_VolumeChunk(sound, MIX_MAX_VOLUME);
        currentSound = sndFile;
    }

    LogDebug(QString("Play Sound (SDL2_mixer)"));
    if(Mix_PlayChannel(-1, sound, 0) == -1)
    {
        qDebug() << QString("Mix_PlayChannel: %1").arg(SDL_GetError());
    }
}

void PGE_SfxPlayer::freeBuffer()
{
    if(sound)
    {
        Mix_FreeChunk(sound);
        sound = nullptr;
    }
}
