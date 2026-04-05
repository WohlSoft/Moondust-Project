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

#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

#include <xmp.h>

#include "audio_tracker_xmp.h"

struct MDAudioXMPPriv
{
    int num_channels;
    struct xmp_module_info mi;
    struct xmp_frame_info fi;
    xmp_context ctx;
};

static const char *libxmp_set_error(int e)
{
    switch (e)
    {
    case -XMP_ERROR_INTERNAL:
        return "Internal error in libxmp";
    case -XMP_ERROR_FORMAT:
        return "Unrecognized file format";
    case -XMP_ERROR_LOAD:
        return "Error loading file";
    case -XMP_ERROR_DEPACK:
        return "Error depacking file";
    case -XMP_ERROR_SYSTEM:
        return "System error in libxmp";
    case -XMP_ERROR_INVALID:
        return "Invalid parameter";
    case -XMP_ERROR_STATE:
        return "Invalid player state";
    default:
        return "Unknown error";
    }
}

MDAudioXMP::MDAudioXMP():
    m_p(new MDAudioXMPPriv)
{
}

MDAudioXMP::~MDAudioXMP()
{
    MDAudioXMP::close();
    delete m_p;
}

uint32_t MDAudioXMP::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | /*SPEC_LOOP_POINTS | */SPEC_META_TAGS | SPEC_FIXED_CHANNELS;
    // Loop points handling is not yet supported, it's no proper way to obtain location
    // of row break and pattern jump events, so, it's not yet possible to accurately obtain global loop points
    // so they aren't supported for a moment.
}

static unsigned long xmp_fread(void *dst, unsigned long len, unsigned long nmemb, void *src)
{
    return SDL_RWread((SDL_RWops*)src, dst, len, nmemb);
}

static int xmp_fseek(void *src, long offset, int whence)
{
    return (SDL_RWseek((SDL_RWops*)src, offset, whence) < 0) ? -1 : 0;
}

static long xmp_ftell(void *src)
{
    return SDL_RWtell((SDL_RWops*)src);
}

bool MDAudioXMP::openRead(SDL_RWops *file)
{
    int err = 0;
    struct xmp_callbacks file_callbacks =
    {
        xmp_fread, xmp_fseek, xmp_ftell, NULL
    };

    close();

    m_file = file;

    m_spec.m_channels = 2;
    m_spec.m_sample_format = AUDIO_S16SYS;
    m_spec.m_sample_rate = m_specWanted.getSampleRate(48000);
    m_spec.updateFrameSize();

    m_p->ctx = xmp_create_context();

    if(!m_p->ctx)
    {
        m_lastError = "XMP: Failed to initialise the context";
        close();
        return false;
    }

    ++init_level;

    err = xmp_load_module_from_callbacks(m_p->ctx, m_file, file_callbacks);

    if(err < 0)
    {
        m_lastError = "XMP: Failed to open the file: ";
        m_lastError.append(libxmp_set_error(err));
        close();
        return false;
    }

    ++init_level;

    err = xmp_start_player(m_p->ctx, m_spec.m_sample_rate, 0);
    if(err < 0)
    {
        m_lastError = "XMP: Failed to start playback of the file: ";
        m_lastError.append(libxmp_set_error(err));
        close();
        return false;
    }

    ++init_level;

    xmp_get_module_info(m_p->ctx, &m_p->mi);

    if(m_p->mi.mod->name[0])
        m_spec.m_meta_title = std::string(m_p->mi.mod->name);

    if(m_p->mi.comment)
        m_spec.m_meta_copyright = std::string(m_p->mi.comment);

    xmp_get_frame_info(m_p->ctx, &m_p->fi);

    m_spec.m_total_length = static_cast<int64_t>((m_p->fi.total_time / 1000.0) * m_spec.m_sample_rate);

    ++init_level;

    return true;
}

bool MDAudioXMP::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioXMP::close()
{
    if(m_p->ctx)
    {
        if(init_level > 3)
            xmp_stop_module(m_p->ctx);

        if(init_level > 2)
            xmp_end_player(m_p->ctx);

        if(init_level > 1)
            xmp_release_module(m_p->ctx);

        if(init_level > 0)
            xmp_free_context(m_p->ctx);

        m_p->ctx = nullptr;
        init_level = 0;
    }

    if(m_file)
        m_file = nullptr;

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioXMP::readRewind()
{
    if(!m_p->ctx)
        return false;

    xmp_seek_time(m_p->ctx, 0);
    xmp_play_buffer(m_p->ctx, NULL, 0, 0);

    return true;
}

size_t MDAudioXMP::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    int ret;

    if(spec_changed)
        *spec_changed = false;

    ret = xmp_play_buffer(m_p->ctx, out, outSize, 1);

    if(ret == -XMP_END)
        return 0;

    return outSize;
}

size_t MDAudioXMP::writeChunk(uint8_t *, size_t)
{
    return 0;
}
