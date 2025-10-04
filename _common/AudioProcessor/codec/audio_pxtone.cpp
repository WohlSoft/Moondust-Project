/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "audio_pxtone.h"
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "pxtone/pxtnService.h"
#include "pxtone/pxtnError.h"


MDAudioPXTone::MDAudioPXTone()
{}

MDAudioPXTone::~MDAudioPXTone()
{
    MDAudioPXTone::close();
}

uint32_t MDAudioPXTone::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_LOOP_POINTS | SPEC_META_TAGS;
}

static bool _pxtn_r(void* user, void* p_dst, Sint32 size, Sint32 num)
{
    return SDL_RWread((SDL_RWops*)user, p_dst, size, num) == (size_t)num;
}

static bool _pxtn_w(void* user,const void* p_dst, Sint32 size, Sint32 num)
{
    return SDL_RWwrite((SDL_RWops*)user, p_dst, size, num) == (size_t)num;
}

static bool _pxtn_s(void* user, Sint32 mode, Sint32 size)
{
    return SDL_RWseek((SDL_RWops*)user, size, mode) >= 0;
}

static bool _pxtn_p(void* user, int32_t* p_pos)
{
    int i = SDL_RWtell((SDL_RWops*)user);
    if( i < 0 ) {
        return false;
    }
    *p_pos = i;
    return true;
}

bool MDAudioPXTone::openRead(SDL_RWops *file)
{
    pxtnERR ret;
    close();

    tempo = getArgD("t=", 1.0);
    gain = getArgF("g=", 1.0f);

    pxtn = new pxtnService(_pxtn_r, _pxtn_w, _pxtn_s, _pxtn_p);

    ret = pxtn->init();
    if (ret != pxtnOK) {
        close();
        m_lastError = std::string("PXTONE: Failed to initialize the library: ") + pxtnError_get_string(ret);
        return false;
    }

    m_spec.m_channels = m_specWanted.getChannels(2, 2);
    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_S16SYS);
    m_spec.m_sample_rate = m_specWanted.getSampleRate(44100);

    /* LOAD MUSIC DATA */
    ret = pxtn->read(file);
    if (ret != pxtnOK) {
        close();
        m_lastError = std::string("PXTONE: Failed to load a music data:") + pxtnError_get_string(ret);
        return false;
    }

    // FIXME: Finish this!


    return true;
}

bool MDAudioPXTone::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioPXTone::close()
{
    if(pxtn)
    {
        if(evals_loaded)
            pxtn->evels->Release();
        evals_loaded = false;
        delete pxtn;
        pxtn = nullptr;
    }

    return true;
}

bool MDAudioPXTone::readRewind()
{
    pxtnVOMITPREPARATION prep;
    if(!pxtn)
        return false;

    SDL_memset(&prep, 0, sizeof(pxtnVOMITPREPARATION));
    prep.flags = flags;
    prep.start_pos_sample = 0;
    prep.master_volume   = 1.0f;

    if(!pxtn->moo_preparation(&prep, tempo))
        return false;

    return true;
}

size_t MDAudioPXTone::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    return 0;
}

size_t MDAudioPXTone::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
