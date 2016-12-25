/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_audio.h"
#include <data_configs/config_manager.h>
#include <audio/play_music.h>
#include <audio/play_sfx.h>

static struct AudioState
{
    bool        isLoaded;
    int         sRate;

    SDL_mutex*  sampleCountMutex;
    Uint64      sCount;
    Uint64      musSCount;
} p_audioState = {false, 44100, NULL, 0, 0};

static void postMixCallback(void *udata,
                            Uint8 *stream,
                            int len)
{
    if(!p_audioState.isLoaded)
        return;

    Q_UNUSED(udata);
    Q_UNUSED(stream);
    if(SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        // This post mix callback has a simple purpose: count audio samples.
        p_audioState.sCount += len/4;

        // (Approximate) sample count for only when music is playing
        if( (Mix_PlayingMusic() == 1) && (Mix_PausedMusic() == 0) )
        {
            p_audioState.musSCount += len/4;
        }
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }
}

int PGE_Audio::init(int sampleRate,
                    int allocateChannels,
                    int bufferSize)
{
    int ret = 0;
    p_audioState.sRate = sampleRate;

    MIX_Timidity_addToPathList(QString(ApplicationPath + "/timidity/").toLocal8Bit().data());

    if(p_audioState.isLoaded)
        Mix_CloseAudio();

    ret = Mix_OpenAudio(p_audioState.sRate, AUDIO_S16, 2, bufferSize);

    if(ret == -1)
        return ret;

    Mix_AllocateChannels(allocateChannels);

    // Reset the audio sample count and set the post mix callback
    if(p_audioState.sampleCountMutex == NULL)
    {
        p_audioState.sampleCountMutex = SDL_CreateMutex();
    }

    // Reset music sample count
    if (SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        p_audioState.sCount = 0;
        p_audioState.musSCount = 0;
        Mix_SetPostMix(postMixCallback, NULL);
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }

    p_audioState.isLoaded = true;

    return ret;
}

int PGE_Audio::quit()
{
    if(!p_audioState.isLoaded)
        return -1;

    PGE_MusPlayer::freeStream();
    Mix_SetPostMix(NULL, NULL);
    PGE_SfxPlayer::clearSoundBuffer();
    Mix_CloseAudio();
    p_audioState.isLoaded = false;
    return 0;
}

const bool &PGE_Audio::isLoaded()
{
    return p_audioState.isLoaded;
}

void PGE_Audio::resetMusicSampleCounter()
{
    if (SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        p_audioState.musSCount = 0;
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }
}


Uint64 PGE_Audio::sampleCount()
{
    if(!p_audioState.isLoaded)
        return 0;

    Uint64 ret = 0;

    // Make sure we don't have a race condition with the callback
    if(SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        ret = p_audioState.sCount;
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }

    return ret;
}

Uint64 PGE_Audio::sampleCountMus()
{
    if(!p_audioState.isLoaded)
        return 0;

    Uint64 ret = 0;

    // Make sure we don't have a race condition with the callback
    if(SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        ret = p_audioState.musSCount;
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }

    return ret;
}




void PGE_Audio::playSound(long soundID)
{
    if(soundID <= 0)
        return;

    if(soundID > ConfigManager::main_sfx_index.size())
        return;

    ConfigManager::main_sfx_index[soundID - 1].play();
}

void PGE_Audio::playSoundByRole(obj_sound_role::roles role)
{
    long id = ConfigManager::getSoundByRole(role);
    playSound(id);
}

