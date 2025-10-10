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
#include <SDL2/SDL_endian.h>
#include "audio_detect.h"

int midiplayer_current = MIDI_ANY;

static AudioFormats xmi_compatible_midi_player()
{
    int is_compatible = 0;

    if(midiplayer_current == MIDI_Native)
        is_compatible |= 1;

    if(midiplayer_current == MIDI_ADLMIDI)
        is_compatible |= 1;

    if(midiplayer_current == MIDI_OPNMIDI)
        is_compatible |= 1;

    if(midiplayer_current == MIDI_Fluidsynth)
        is_compatible |= 1;

    if(midiplayer_current == MIDI_EDMIDI)
        is_compatible |= 1;


    if(is_compatible)
        return FORMAT_MIDI;
    else
        return FORMAT_MIDI_ADL;
}

static int detect_mp3(Uint8 *magic, SDL_RWops *src, Sint64 start, Sint64 offset)
{
    const Uint32 null = 0;
    Uint8 mp3_magic[4];
    Uint8 mp3_read_buffer[2048];
    const size_t mp3_read_buffer_size = 2048;
    size_t i = 0, got;
    bool found;
    bool at_end = false;
    Sint64 end_file_pos = 0;
    const int max_search = 10240;
    int bytes_read = 0;

    SDL_memcpy(mp3_magic, magic, 4);

    /* Attempt to quickly detect MP3 file if possible */
    /* see: https://bugzilla.libsdl.org/show_bug.cgi?id=5322 */
    if((magic[0] == 0xFF) && (magic[1] & 0xE6) == 0xE2)
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 1;
    }

    /* If no success, try the deep scan of first 10 kilobytes of the file
     * to detect the first valid MP3 frame */

    SDL_RWseek(src, 0, RW_SEEK_END);
    end_file_pos = SDL_RWtell(src);
    SDL_RWseek(src, start + offset, RW_SEEK_SET);

    /* If first 4 bytes are not zero */
    if(SDL_memcmp(mp3_magic, &null, 4) != 0)
        goto readHeader;

digMoreBytes:
    /* Find the nearest 0xFF byte */
    found = SDL_FALSE;
    do
    {
        SDL_memset(mp3_read_buffer, 0, mp3_read_buffer_size);
        got = SDL_RWread(src, mp3_read_buffer, 1, mp3_read_buffer_size);

        at_end = got < mp3_read_buffer_size;

        if (got == 0) {
            SDL_RWseek(src, start, RW_SEEK_SET); /* Reached the end of the file */
            return 0;
        }

        bytes_read += got;

        for(i = 0; i < got; ++i)
        {
            if(mp3_read_buffer[i] == 0xFF)
            {
                found = SDL_TRUE;
                bytes_read -= (Sint64)(got - i);
                SDL_RWseek(src, -(Sint64)(got - i), RW_SEEK_CUR);
                break;
            }
        }
    } while(!found && bytes_read < max_search);

    /* Can't read 4 bytes of the frame header */
    if(SDL_RWread(src, mp3_magic, 1, 4) != 4)
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 0;
    }

    /* Go back to 3 bytes */
    SDL_RWseek(src, -3, RW_SEEK_CUR);

    /* Got the end of search zone, however, found nothing */
    if(bytes_read > max_search)
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 0;
    }

    /* Got the end of file, however, found nothing */
    if(SDL_RWtell(src) >= (end_file_pos - 1))
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 0;
    }

readHeader:
    if (
        ((mp3_magic[0] & 0xff) != 0xff) || ((mp3_magic[1] & 0xe6) != 0xe2) || /*  No sync bits */
        ((mp3_magic[2] & 0xf0) == 0x00) || /*  Bitrate is 0 */
        ((mp3_magic[2] & 0xf0) == 0xf0) || /*  Bitrate is 15 */
        ((mp3_magic[2] & 0x0c) == 0x0c) || /*  Frequency is 3 */
        ((mp3_magic[1] & 0x06) == 0x00)    /*  Layer is 4 */
        )
    {
        /* printf("WRONG BITS\n"); */
        if(at_end)
        {
            SDL_RWseek(src, start, RW_SEEK_SET); /* Reached the end of the file */
            return 0;
        }

        goto digMoreBytes;
    }

    SDL_RWseek(src, start, RW_SEEK_SET);
    return 1;
}

static int detect_riff_mp3(SDL_RWops *src, Sint64 start)
{
    Uint32 magic;
    Uint32 chunk_type;
    Uint32 chunk_length;
    Uint16 encoding;

    magic = SDL_ReadLE32(src);

    if(magic != 0x46464952 /*RIFF*/ && magic != 0x45564157 /*WAVE*/)
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 0;
    }

    /* Skip 8 bytes */
    SDL_RWseek(src, 8, RW_SEEK_CUR);

    /* Read the chunks */
    for (; ;)
    {
        chunk_type = SDL_ReadLE32(src);
        chunk_length = SDL_ReadLE32(src);

        if(chunk_length == 0)
            break;

        switch(chunk_type)
        {
        case 0x20746D66: /* Do find only fmt chunk */
            if(chunk_length < 16)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                return 0;
            }

            encoding = SDL_ReadLE16(src);
            SDL_RWseek(src, start, RW_SEEK_SET);
            return encoding == 0x0055; /* If encoding is MPEG Layer 3*/

        default: /* All other chunks just skip until finding a "fmt" */
            SDL_RWseek(src, chunk_length, RW_SEEK_CUR);
            break;
        }

        /* RIFF chunks have a 2-byte alignment. Skip padding byte. */
        if(chunk_length & 1)
        {
            if(SDL_RWseek(src, 1, RW_SEEK_CUR) < 0)
                return 0;
        }
    }

    SDL_RWseek(src, start, RW_SEEK_SET);

    return 0;
}


#define TAGS_INPUT_BUFFER_SIZE 128

#define ID3v2_HEADER_SIZE               10
#define ID3v2_FIELD_TAG_LENGTH          6

static SDL_bool is_id3v2(const Uint8 *data, size_t length)
{
    /* ID3v2 header is 10 bytes:  http://id3.org/id3v2.4.0-structure */
    /* bytes 0-2: "ID3" identifier */
    if(length < ID3v2_HEADER_SIZE || SDL_memcmp(data, "ID3",3) != 0)
        return SDL_FALSE;

    /* bytes 3-4: version num (major,revision), each byte always less than 0xff. */
    if(data[3] == 0xff || data[4] == 0xff)
        return SDL_FALSE;

    /* bytes 6-9 are the ID3v2 tag size: a 32 bit 'synchsafe' integer, i.e. the
     * highest bit 7 in each byte zeroed.  i.e.: 7 bit information in each byte ->
     * effectively a 28 bit value.  */
    if(data[6] >= 0x80 || data[7] >= 0x80 || data[8] >= 0x80 || data[9] >= 0x80)
        return SDL_FALSE;

    return SDL_TRUE;
}

static SDL_INLINE Sint32 id3v2_synchsafe_decode(const Uint8 *data)
{
    return ((data[0] << 21) + (data[1] << 14) + (data[2] << 7) + data[3]);
}

static long get_id3v2_len(const Uint8 *data, long length)
{
    /* size is a 'synchsafe' integer (see above) */
    long size = id3v2_synchsafe_decode(data + ID3v2_FIELD_TAG_LENGTH);
    size += ID3v2_HEADER_SIZE; /* header size */

    /* ID3v2 header[5] is flags (bits 4-7 only, 0-3 are zero).
     * bit 4 set: footer is present (a copy of the header but
     * with "3DI" as ident.)  */
    if (data[5] & 0x10)
        size += ID3v2_HEADER_SIZE; /* footer size */

    /* optional padding (always zeroes) */
    while (size < length && data[size] == 0)
        ++size;

    return size;
}

long get_id3v2_length(SDL_RWops *src)
{
    Uint8 buf[TAGS_INPUT_BUFFER_SIZE];
    size_t readsize;
    Sint64 start;

    if(!src)
        return 0;

    start = SDL_RWtell(src);
    readsize = SDL_RWread(src, buf, 1, TAGS_INPUT_BUFFER_SIZE);
    SDL_RWseek(src, start, RW_SEEK_SET);

    if(!readsize)
        return 0;

    if(!is_id3v2(buf, readsize))
        return 0;

    return get_id3v2_len(buf, (long)readsize);
}

static int detect_imf(SDL_RWops *in, Sint64 start)
{
    size_t chunksize;
    Uint32 sum1 = 0,  sum2 = 0, passed_length = 0;
    Uint16 buff, word;

    if(!in)
        return 0;

    SDL_RWseek(in, start, RW_SEEK_SET);

    if(SDL_RWread(in, &word, 1, 2) != 2)
    {
        SDL_RWseek(in, start, RW_SEEK_SET);
        return 0;
    }

    chunksize = SDL_SwapLE16(word);

    if(chunksize & 3)
    {
        SDL_RWseek(in, start, RW_SEEK_SET);
        return 0;
    }

    if(chunksize == 0) /* IMF Type 0 (unlimited file length) */
    {
        SDL_RWseek(in, 0, RW_SEEK_END);
        chunksize = (Uint16)SDL_RWtell(in);
        SDL_RWseek(in, start, RW_SEEK_SET);
        if (chunksize & 3) {
            return 0;
        }
    }

    while(passed_length < chunksize)
    {
        if(SDL_RWread(in, &word, 1, 2) != 2)
        {
            SDL_RWseek(in, start, RW_SEEK_SET);
            break;
        }

        passed_length += 2;
        buff = SDL_SwapLE16(word);
        sum1 += buff;

        if(SDL_RWread(in, &word, 1, 2) != 2)
        {
            SDL_RWseek(in, start, RW_SEEK_SET);
            break;
        }

        passed_length += 2;
        buff = SDL_SwapLE16(word);
        sum2 += buff;
    }
    SDL_RWseek(in, start, RW_SEEK_SET);

    if(passed_length != chunksize)
        return 0;

    return (sum1 > sum2);
}

static int detect_ea_rsxx(SDL_RWops *in, Sint64 start, Uint8 magic_byte)
{
    int res = SDL_FALSE;
    Uint8 sub_magic[6];
    size_t file_size;

    SDL_RWseek(in, 0, RW_SEEK_END);
    file_size = (size_t)SDL_RWtell(in);
    SDL_RWseek(in, start, RW_SEEK_SET);

    if(magic_byte < 0x5D || file_size <= magic_byte)
        return res;

    SDL_RWseek(in, start + (magic_byte - 0x10), RW_SEEK_SET);

    if(SDL_RWread(in, &sub_magic, 1, 6) != 6)
    {
        SDL_RWseek(in, start, RW_SEEK_SET);
        return 0;
    }

    if(SDL_memcmp(sub_magic, "rsxx}u", 6) == 0)
        res = SDL_TRUE;

    SDL_RWseek(in, start, RW_SEEK_SET);

    return res;
}

static int detect_mod(SDL_RWops *in, Sint64 start)
{
    int res = SDL_FALSE;
    int i;
    Uint8 mod_magic[4];
    const char * const mod_known_magics[] =
    {
        "M.K.", "M!K!", "M&K!", "N.T.", "6CHN", "8CHN",
        "CD61", "CD81", "TDZ1", "TDZ2", "TDZ3", "TDZ4",
        "FA04", "FA06", "FA08", "LARD", "NSMS", "FLT4",
        NULL
    };

    if(SDL_RWseek(in, start + 1080, RW_SEEK_SET) < 0)
        goto fail;

    if(SDL_RWread(in, mod_magic, 1, 4) != 4)
        goto fail;

    for(i = 0; mod_known_magics[i] ; ++i)
    {
        if(SDL_memcmp(mod_magic, mod_known_magics[i], 4) == 0)
        {
            res = SDL_TRUE;
            break;
        }
    }

fail:
    SDL_RWseek(in, start, RW_SEEK_SET);
    return res;
}


AudioFormats audio_detect_format(SDL_RWops *src, std::string &error)
{
    Uint8 magic[100];
    Sint64 start = SDL_RWtell(src);
    Uint8 submagic[4] = {0, 0, 0, 0};
    long id3len = 0;
    size_t readlen = 0;

    SDL_memset(magic, 0, 100);
    if(SDL_RWread(src, magic, 1, 99) < 24)
    {
        error = "Couldn't read any first 24 bytes of audio data";
        return FORMAT_UNKNOWN;
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    magic[99]       = '\0';

    /* Drop out some known but not supported file types (Archives, etc.) */
    if(SDL_memcmp(magic, "PK\x03\x04", 3) == 0)
        return FORMAT_UNKNOWN;

    if(SDL_memcmp(magic, "\x37\x7A\xBC\xAF\x27\x1C", 6) == 0)
        return FORMAT_UNKNOWN;

    /* Ogg Vorbis files have the magic four bytes "OggS" */
    if(SDL_memcmp(magic, "OggS", 4) == 0)
    {
        SDL_RWseek(src, 28, RW_SEEK_CUR);
        SDL_RWread(src, magic, 1, 8);
        SDL_RWseek(src,-36, RW_SEEK_CUR);
        if(SDL_memcmp(magic, "OpusHead", 8) == 0)
            return FORMAT_OPUS;

        if(magic[0] == 0x7F && SDL_memcmp(magic + 1, "FLAC", 4) == 0)
            return FORMAT_FLAC;

        return FORMAT_OGG_VORBIS;
    }

    /* FLAC files have the magic four bytes "fLaC" */
    if(SDL_memcmp(magic, "fLaC", 4) == 0)
        return FORMAT_FLAC;

    /* WavPack files have the magic four bytes "wvpk" */
    if(SDL_memcmp(magic, "wvpk", 4) == 0)
        return FORMAT_WAVPACK;

    /* MIDI files have the magic four bytes "MThd" */
    if(SDL_memcmp(magic, "MThd", 4) == 0)
        return FORMAT_MIDI;

    /* RIFF MIDI files have the magic four bytes "RIFF" and then "RMID" */
    if((SDL_memcmp(magic, "RIFF", 4) == 0) && (SDL_memcmp(magic + 8, "RMID", 4) == 0))
        return FORMAT_MIDI;

    if(SDL_memcmp(magic, "MUS\x1A", 4) == 0)
    {
        return xmi_compatible_midi_player();
    }

    if((SDL_memcmp(magic, "FORM", 4) == 0) && (SDL_memcmp(magic + 8, "XDIR", 4) == 0))
    {
        return xmi_compatible_midi_player();
    }

    /* WAVE files have the magic four bytes "RIFF"
           AIFF files have the magic 12 bytes "FORM" XXXX "AIFF" */
    if(((SDL_memcmp(magic, "RIFF", 4) == 0) && (SDL_memcmp((magic + 8), "WAVE", 4) == 0)) ||
       ((SDL_memcmp(magic, "FORM", 4) == 0) && (SDL_memcmp((magic + 8), "XDIR", 4) != 0)))
    {
        /* Some WAV files may contain MP3-encoded streams */
        if(detect_riff_mp3(src, start))
            return FORMAT_MP3;

        return FORMAT_WAV;
    }

    if(SDL_memcmp(magic, "GMF\x1", 4) == 0)
        return FORMAT_MIDI_ADL;

    if(SDL_memcmp(magic, "CTMF", 4) == 0)
        return FORMAT_MIDI_ADL;

    /* GME Specific files */
    if(SDL_memcmp(magic, "ZXAY", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "GBS\x01", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "GYMX", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "HESM", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "KSCC", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "KSSX", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "NESM", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "NSFE", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "SAP\x0D", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "SNES", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "Vgm ", 4) == 0)
        return FORMAT_GME;
    if(SDL_memcmp(magic, "\x1f\x8b", 2) == 0)
        return FORMAT_GME;

    /* PXTone Collage files */
    if(SDL_memcmp(magic, "PTTUNE", 6) == 0)
        return FORMAT_PXTONE;
    if(SDL_memcmp(magic, "PTCOLLAGE", 9) == 0)
        return FORMAT_PXTONE;

    /* Quite OK Audio files */
    if(SDL_memcmp(magic, "qoaf", 4) == 0)
        return FORMAT_QOA; /* Official files */
    if(SDL_memcmp(magic, "XQOA", 4) == 0)
        return FORMAT_XQOA; /* Files with custom headers that supports loop points */

    /* Detect some module files */
    if(SDL_memcmp(magic, "Extended Module", 15) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "ASYLUM Music Format V", 22) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "DIGI Booster module", 19) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "OKTASONG", 8) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "Extreme", 7) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "\xc1\x83\x2a\x9e", 4) == 0) /* UMX */
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "IMPM", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "DBM0", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "DDMF", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "DSM\x10", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "DMML", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "KRIS", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic + 6, "Music   ", 8) == 0) /* ABK */
        return FORMAT_TRACKER;
    /*  SMF files have the magic four bytes "RIFF" */
    if((SDL_memcmp(magic, "RIFF", 4) == 0) &&
        (SDL_memcmp(magic + 8,  "DSMF", 4) == 0) &&
        (SDL_memcmp(magic + 12, "SONG", 4) == 0))
        return FORMAT_TRACKER;
    if((SDL_memcmp(magic, "FORM", 4) == 0) && /* EMOD */
        (SDL_memcmp(magic + 8,  "EMOD", 4) == 0) &&
        (SDL_memcmp(magic + 12, "EMIC", 4) == 0))
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MAS_UTrack_V00", 14) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "GF1PATCH110", 11) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "FAR=", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "\x00MGT", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "\xbdMCS", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MTM", 3) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MMD", 3) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MED\x2", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MED\x3", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MED\x2", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "PSM\x20", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "PTMF", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "MT20", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "OKTA", 4) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic + 44, "PTMF", 4) == 0) /* PTM */
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic + 44, "SCRM", 4) == 0) /* S3M */
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "JN", 2) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "if", 2) == 0)
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "\x69\x66", 2) == 0) /* 669 */
        return FORMAT_TRACKER;
    if(SDL_memcmp(magic, "\x4a\x4e", 2) == 0) /* 669 */
        return FORMAT_TRACKER;

    if(SDL_memcmp(magic, "\x1A\x45\xDF\xA3", 4) == 0)
        return FORMAT_FFMPEG;
    if(SDL_memcmp(magic, "\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9\x00\xAA\x00\x62\xCE\x6C", 16) == 0) /* WMA */
        return FORMAT_FFMPEG;
    if(SDL_memcmp(magic + 4, "\x66\x74\x79\x70\x69\x73\x6F\x6D", 8) == 0) /* AAC */
        return FORMAT_FFMPEG;
    if(SDL_memcmp(magic + 4, "\x66\x74\x79\x70\x6D\x70\x34\x32", 8) == 0) /* AAC */
        return FORMAT_FFMPEG;
    if(SDL_memcmp(magic + 4, "ftypM4A ", 8) == 0) /* AAC */
        return FORMAT_FFMPEG;

    if(SDL_memcmp(magic, "ID3", 3) == 0)
    {
        id3len = get_id3v2_length(src);

        /* Check if there is something not an MP3, however, also has ID3 tag */
        if(id3len > 0)
        {
            SDL_RWseek(src, id3len, RW_SEEK_CUR);
            readlen = SDL_RWread(src, submagic, 1, 4);
            SDL_RWseek(src, start, RW_SEEK_SET);

            if(readlen == 4)
            {
                if (SDL_memcmp(submagic, "fLaC", 4) == 0)
                    return FORMAT_FLAC;
            }
        }
    }

    if(detect_mod(src, start))
        return FORMAT_TRACKER;

    /* Detect MP3 format by frame header [needs scanning of bigger part of the file] */
    if(detect_mp3(submagic, src, start, id3len))
        return FORMAT_MP3;

    /* Detect id Software Music Format file */
    if(detect_imf(src, start))
        return FORMAT_MIDI_ADL;

    /* Detect EA MUS (RSXX) format */
    if(detect_ea_rsxx(src, start, magic[0]))
    {
        if(midiplayer_current != MIDI_Timidity)
            return FORMAT_MIDI;
        else
            return FORMAT_MIDI_ADL;

    }

    /* Reset position to zero! */
    SDL_RWseek(src, start, RW_SEEK_SET);

    /* Assume MOD format.
     *
     * Apparently there is no way to check if the file is really a MOD,
     * or there are too many formats supported by libmodplug or libxmp.
     * The mod library does this check by itself. */
    return FORMAT_TRACKER;
}
