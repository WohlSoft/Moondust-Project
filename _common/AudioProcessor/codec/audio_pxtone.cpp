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


MDAudioPXTone::MDAudioPXTone() :
    MDAudioFile()
{
    m_read_buffer.resize(4096);
}

MDAudioPXTone::~MDAudioPXTone()
{
    MDAudioPXTone::close();
}

uint32_t MDAudioPXTone::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_SOURCE_ANY_CHANNELS | SPEC_LOOP_POINTS | SPEC_META_TAGS;
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
    const char *name;
    int32_t name_len;
    const char *comment;
    int32_t comment_len;
    char *temp_string;
    pxtnERR ret;
    close();

    tempo = m_args.getArgD("t=", 1.0);
    gain = m_args.getArgF("g=", 1.0f);

    pxtn = new pxtnService(_pxtn_r, _pxtn_w, _pxtn_s, _pxtn_p);

    ret = pxtn->init();
    if(ret != pxtnOK)
    {
        close();
        m_lastError = std::string("PXTONE: Failed to initialize the library: ") + pxtnError_get_string(ret);
        return false;
    }

    m_spec.m_channels = m_specWanted.getChannels(2, 2);
    m_spec.m_sample_format = AUDIO_S16SYS;
    m_spec.m_sample_rate = m_specWanted.getSampleRate(44100);

    if(!pxtn->set_destination_quality(m_spec.m_channels, m_spec.m_sample_rate))
    {
        close();
        m_lastError = std::string("PXTONE: Failed to set the destination quality");
        return false;
    }

    /* LOAD MUSIC DATA */
    ret = pxtn->read(file);
    if(ret != pxtnOK)
    {
        close();
        m_lastError = std::string("PXTONE: Failed to load a music data:") + pxtnError_get_string(ret);
        return false;
    }

    ret = pxtn->tones_ready();
    if(ret != pxtnOK)
    {
        close();
        m_lastError = std::string("PXTONE: Failed to initialize tones: ") + pxtnError_get_string(ret);
        return false;
    }

    evals_loaded = true;

    /* PREPARATION PLAYING MUSIC */
    pxtn->moo_get_total_sample();

    pxtnVOMITPREPARATION prep;
    SDL_memset(&prep, 0, sizeof(pxtnVOMITPREPARATION));
    prep.flags = pxtnVOMITPREPFLAG_unit_mute;
    prep.start_pos_float = 0;
    prep.master_volume   = 1.0f;

    if(!pxtn->moo_preparation(&prep, tempo))
    {
        close();
        m_lastError = std::string("PXTONE: Failed to initialize the output (Moo)");
        return false;
    }

    /* Attempt to load metadata */

    name = pxtn->text->get_name_buf(&name_len);
    if(name)
    {
        temp_string = SDL_iconv_string("UTF-8", "Shift-JIS", name, name_len + 1);
        m_spec.m_meta_title = std::string(temp_string);
        SDL_free(temp_string);
    }

    comment = pxtn->text->get_comment_buf(&comment_len);
    if(comment)
    {
        temp_string = SDL_iconv_string("UTF-8", "Shift-JIS", comment, comment_len + 1);
        m_spec.m_meta_copyright = std::string(temp_string);
        SDL_free(temp_string);
    }

    m_spec.m_loop_start = pxtn->moo_get_sampling_repeat();
    m_spec.m_loop_end = pxtn->moo_get_sampling_end();
    m_spec.m_loop_len = (m_spec.m_loop_start >= 0 && m_spec.m_loop_end >= 0) ? (m_spec.m_loop_end - m_spec.m_loop_start) : 0;
    int32_t duration = pxtn->moo_get_total_sample();
    m_spec.m_total_length = duration > 0 ? duration / tempo: 0;

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
    prep.flags = pxtnVOMITPREPFLAG_unit_mute;
    prep.start_pos_sample = 0;
    prep.master_volume   = 1.0f;

    if(!pxtn->moo_preparation(&prep, tempo))
        return false;

    return true;
}

size_t MDAudioPXTone::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    if(outSize > m_read_buffer.size())
        m_read_buffer.resize(outSize);

    if(!pxtn->Moo(m_read_buffer.data(), outSize))
        return 0;

    copyGained(gain, m_read_buffer.data(), out, outSize);

    return outSize;
}

size_t MDAudioPXTone::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
