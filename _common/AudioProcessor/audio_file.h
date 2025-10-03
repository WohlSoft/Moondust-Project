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

#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H

#include <stdint.h>
#include <string>
#include <map>

struct SDL_RWops;

struct MDAudioFileSpec
{
    int64_t  m_total_length = 0;
    int64_t  m_loop_start = 0;
    int64_t  m_loop_end = 0;
    int64_t  m_loop_len = 0;

    uint32_t m_multitrack_chans = 0;
    uint32_t m_multitrack_tracks = 0;

    int      m_channels = 0;
    int      m_sample_format = 0;
    int      m_sample_rate = 0;

    std::string m_meta_title;
    std::string m_meta_artist;
    std::string m_meta_album;
    std::string m_meta_copyright;

    // Encoding settings
    bool vbr = false;
    int quality = -1;
    int bitrate = -1;
    int profile = -1;
};

class MDAudioFile
{
protected:
    SDL_RWops       *m_file = nullptr;
    MDAudioFileSpec m_spec;
    std::string     m_lastError;
    int             m_argTrack = 0; //!< GME only track number
    typedef std::map<std::string, std::string> ArgsMap;
    ArgsMap m_args;

    static bool isLoopTag(const char* tag);
    static int64_t parseTime(char *time, long samplerate_hz);
    static std::string parseidiMetaTag(const char *src);

    int getArgI(const std::string &key, int def);
    bool getArgB(const std::string &key, bool def);
    float getArgF(const std::string &key, float def);
    double getArgD(const std::string &key, double def);
    std::string getArgS(const std::string &key, const std::string def);

public:
    MDAudioFile();
    virtual ~MDAudioFile();

    enum CodecSpec
    {
        //! Codec can only read
        SPEC_READ = 0x01,
        //! Codec can only write
        SPEC_WRITE = 0x02,
        //! Codec supports only one of allowed sample rates
        SPEC_FIXED_SAMPLE_RATE = 0x04,
        //! Codec supports loop points
        SPEC_LOOP_POINTS = 0x08,
        //! Codec supports meta-tags
        SPEC_META_TAGS = 0x10,
        //! Codec supports multi-track format
        SPEC_MULTI_TRACK = 0x20,
        //! Codec can generate output with any desired sample rate
        SPEC_SOURCE_ANY_RATE = 0x40,
        //! Codec can generate output with any desired sample format
        SPEC_SOURCE_ANY_FORMAT = 0x80
    };

    virtual uint32_t getCodecSpec() const = 0;

    std::string getLastError();

    void setArgs(const std::string &args);

    virtual bool openRead(SDL_RWops *file) = 0;
    virtual bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) = 0;

    virtual bool close() = 0;

    virtual bool readRewind() = 0;

    virtual size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) = 0;
    virtual size_t writeChunk(uint8_t *in, size_t inSize) = 0;

    const MDAudioFileSpec &getSpec() const;
};

#endif // AUDIO_FILE_H
