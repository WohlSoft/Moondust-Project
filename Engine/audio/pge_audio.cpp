/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
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

    (void)(udata);
    (void)(stream);
    if(SDL_LockMutex(p_audioState.sampleCountMutex) == 0)
    {
        // This post mix callback has a simple purpose: count audio samples.
        p_audioState.sCount += len/4;

        // (Approximate) sample count for only when music is playing
        if(PGE_MusPlayer::isPlaying() && !PGE_MusPlayer::isPaused())
        {
            p_audioState.musSCount += len/4;
        }
        SDL_UnlockMutex(p_audioState.sampleCountMutex);
    }
}

int PGE_Audio::init(Uint32 sampleRate,
                    Uint32 allocateChannels,
                    Uint32 bufferSize)
{
    int ret = 0;
    p_audioState.sRate = int(sampleRate);

    Mix_Timidity_addToPathList(std::string(ApplicationPathSTD + "timidity/").c_str());

    if(p_audioState.isLoaded)
        Mix_CloseAudio();

    ret = Mix_OpenAudio(p_audioState.sRate, AUDIO_S16, 2, int(bufferSize));

    if(ret == -1)
        return ret;

    Mix_AllocateChannels(int(allocateChannels));

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




void PGE_Audio::playSound(size_t soundID)
{
    if(soundID == 0)
        return;
    if((soundID > ConfigManager::main_sfx_index.size()))
        return;
    ConfigManager::main_sfx_index[soundID - 1].play();
}

void PGE_Audio::playSoundByRole(obj_sound_role::roles role)
{
    size_t id = ConfigManager::getSoundByRole(role);
    if(id > 0)
        playSound(id);
}

