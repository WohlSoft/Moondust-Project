#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#define DR_WAV_NO_WCHAR
#include "wav/dr_wav.h"

#include "audio_wav.h"

#define M_WAV reinterpret_cast<drwav*>(m_wav)

struct waveEncodeMeta
{
    std::vector<drwav_cue_point> m_cuePoints;
    std::vector<drwav_cue> m_cue;
    std::vector<drwav_smpl_loop> m_loops;
    std::vector<drwav_smpl> m_smpl;

    std::vector<drwav_list_info_text> m_infotext;
    std::vector<drwav_metadata_type> m_infotextType;

    std::vector<drwav_metadata> m_meta;

    void add_string(std::string &str, drwav_metadata_type type)
    {
        drwav_list_info_text info;
        if(!str.empty())
        {
            info.pString = &str[0];
            info.stringLength = str.size();
            m_infotext.push_back(info);
            m_infotextType.push_back(type);
        }
    }

    void initMeta(MDAudioFileSpec &spec)
    {
        drwav_metadata meta;
        drwav_cue_point cue_point;
        drwav_cue cue;
        drwav_smpl smpl;
        drwav_smpl_loop loop;

        m_cue.clear();
        m_smpl.clear();
        m_infotext.clear();
        m_meta.clear();
        m_loops.clear();

        cue_point.id = 0;
        SDL_memcpy(cue_point.dataChunkId, "data", 4);
        cue_point.chunkStart = 0;
        cue_point.blockStart = 0;
        cue_point.sampleOffset = 0;
        m_cuePoints.push_back(cue_point);

        cue.pCuePoints = m_cuePoints.data();
        cue.cuePointCount = m_cuePoints.size();
        m_cue.push_back(cue);

        if(spec.m_loop_len > 0 && spec.m_loop_end > spec.m_loop_start)
        {
            loop.cuePointId = 0;
            loop.type = drwav_smpl_loop_type_forward;
            loop.firstSampleOffset = spec.m_loop_start;
            loop.lastSampleOffset = spec.m_loop_end - 1;
            loop.sampleFraction = 0;
            loop.playCount = 0;
            m_loops.push_back(loop);
        }

        smpl.manufacturerId = 0;
        smpl.productId = 0;
        smpl.samplePeriodNanoseconds = 0;
        smpl.midiUnityNote = 0;
        smpl.midiPitchFraction = 0;
        smpl.smpteFormat = 0;
        smpl.smpteOffset = 0;
        smpl.samplerSpecificDataSizeInBytes = 0;
        smpl.pLoops = m_loops.data();
        smpl.sampleLoopCount = m_loops.size();
        m_smpl.push_back(smpl);

        add_string(spec.m_meta_title, drwav_metadata_type_list_info_title);
        add_string(spec.m_meta_album, drwav_metadata_type_list_info_album);
        add_string(spec.m_meta_artist, drwav_metadata_type_list_info_artist);
        add_string(spec.m_meta_copyright, drwav_metadata_type_list_info_copyright);

        for(auto &c : m_cue)
        {
            SDL_zero(meta);
            meta.type = drwav_metadata_type_cue;
            meta.data.cue = c;
            m_meta.push_back(meta);
        }

        for(auto &c : m_smpl)
        {
            SDL_zero(meta);
            meta.type = drwav_metadata_type_smpl;
            meta.data.smpl = c;
            m_meta.push_back(meta);
        }

        for(size_t i = 0; i < m_infotext.size(); ++i)
        {
            SDL_zero(meta);
            meta.type = m_infotextType[i];
            meta.data.infoText= m_infotext[i];
            m_meta.push_back(meta);
        }
    }
};

void MDAudioWAV::initChPermute()
{
    switch(m_spec.m_channels)
    {
    case 1: /* 1.0 mono   */
        m_spec.m_channels_permute.assign({0});
        break;
    case 2: /* 2.0 stereo */
        m_spec.m_channels_permute.assign({0, 1});
        break;
    case 3: /* 3.0 channel ('wide') stereo */
        m_spec.m_channels_permute.assign({0, 2, 1});
        break;
    case 4: /* 4.0 discrete quadraphonic */
        m_spec.m_channels_permute.assign({0, 1, 2, 3});
        break;
    case 5: /* 5.0 surround */
        m_spec.m_channels_permute.assign({0, 2, 1, 3, 4});
        break;
    case 6: /* 5.1 surround */
        if(M_WAV->container == drwav_container_aiff)
            m_spec.m_channels_permute.assign({0, 1, 2, 3, 4, 5});
        else
            m_spec.m_channels_permute.assign({0, 2, 1, 4, 5, 3});
        break;
    case 7: /* 6.1 surround */
        m_spec.m_channels_permute.assign({0, 2, 1, 5, 6, 4, 3});
        break;
    case 8: /* 7.1 surround (classic theater 8-track) */
        m_spec.m_channels_permute.assign({0, 2, 1, 6, 7, 4, 5, 3});
        break;
    default:
        m_spec.m_channels_permute.clear();
        break;
    }
}

template<typename T>
static void reorder_channels_fmt_read(uint8_t *buff, size_t outSize, size_t channels, const int *permute_matrix)
{
    T chs[8];
    size_t frames = outSize / (sizeof(T) * channels);
    T *cur = reinterpret_cast<T*>(buff);

    for(size_t i = 0; i < frames; ++i)
    {
        SDL_memcpy(chs, cur, channels * sizeof(T));

        for(size_t c = 0; c < channels; ++c)
            cur[c] = chs[permute_matrix[c]];

        cur += channels;
    }
}

template<typename T>
static void reorder_channels_fmt_write(uint8_t *buff, size_t outSize, size_t channels, const int *permute_matrix)
{
    size_t frames = outSize / (sizeof(T) * channels);
    T chs[8];
    T *cur = reinterpret_cast<T*>(buff);

    for(size_t i = 0; i < frames; ++i)
    {
        SDL_memcpy(chs, cur, channels * sizeof(T));

        for(size_t c = 0; c < channels; ++c)
            cur[permute_matrix[c]] = chs[c];

        cur += channels;
    }
}

void MDAudioWAV::reorderChannels(uint8_t *buff, size_t outSize)
{
    if(!buff || outSize == 0 || m_spec.m_channels < 3 || m_spec.m_channels > 8)
        return; // Nothing to do

    switch(m_spec.m_sample_format)
    {
    case AUDIO_U8:
    case AUDIO_S8:
        if(m_write)
            reorder_channels_fmt_write<uint8_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        else
            reorder_channels_fmt_read<uint8_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        break;

    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        if(m_write)
            reorder_channels_fmt_write<uint16_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        else
            reorder_channels_fmt_read<uint16_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        break;

    case AUDIO_S32LSB:
    case AUDIO_S32MSB:
    case AUDIO_F32LSB:
    case AUDIO_F32MSB:
        if(m_write)
            reorder_channels_fmt_write<uint32_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        else
            reorder_channels_fmt_read<uint32_t>(buff, outSize, m_spec.m_channels, m_spec.m_channels_permute.data());
        break;
    }
}

MDAudioWAV::MDAudioWAV(EncodeContainer container, EncodeTarget target) :
    MDAudioFile(),
    m_wav(new drwav),
    m_target(target),
    m_container(container)
{}

MDAudioWAV::~MDAudioWAV()
{
    MDAudioWAV::close();
    delete M_WAV;
}

uint32_t MDAudioWAV::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_LOOP_POINTS | SPEC_META_TAGS;
}

static size_t sdl_wav_read(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    return SDL_RWread((SDL_RWops*)pUserData, pBufferOut, 1, bytesToRead);
}

static size_t sdl_wav_write(void* pUserData, const void* pBufferOut, size_t bytesToRead)
{
    return SDL_RWwrite((SDL_RWops*)pUserData, pBufferOut, 1, bytesToRead);
}

static drwav_bool32 sdl_wav_seek(void* pUserData, int offset, drwav_seek_origin origin)
{
    int sdl_origin = RW_SEEK_SET;

    switch(origin)
    {
    case DRWAV_SEEK_SET:
        sdl_origin = RW_SEEK_SET;
        break;
    case DRWAV_SEEK_CUR:
        sdl_origin = RW_SEEK_CUR;
        break;
    case DRWAV_SEEK_END:
        sdl_origin = RW_SEEK_END;
        break;
    }

    if(SDL_RWseek((SDL_RWops*)pUserData, offset, sdl_origin) < 0)
        return DRWAV_FALSE;

    return DRWAV_TRUE;
}

static drwav_bool32 sdl_wav_tell(void* pUserData, drwav_int64* pCursor)
{
    Sint64 off = SDL_RWtell((SDL_RWops*)pUserData);

    if(off < 0)
        return DRWAV_FALSE;

    if(pCursor)
        *pCursor = off;

    return DRWAV_TRUE;
}

static void* sdl_wav_malloc(size_t size, void *)
{
    return SDL_malloc(size);
}

static void* sdl_wav_realloc(void *p, size_t size, void *)
{
    return SDL_realloc(p, size);
}

static void sdl_wav_free(void *p, void *)
{
    SDL_free(p);
}

bool MDAudioWAV::openRead(SDL_RWops *file)
{
    drwav_allocation_callbacks allocs;
    drwav *wav = M_WAV;

    close();

    m_write = false;
    m_file = file;
    m_cvt_needed = false;

    SDL_zero(allocs);
    allocs.onMalloc = sdl_wav_malloc;
    allocs.onRealloc = sdl_wav_realloc;
    allocs.onFree = sdl_wav_free;

    if(!drwav_init_with_metadata(wav, sdl_wav_read, sdl_wav_seek, sdl_wav_tell, m_file, 0, &allocs))
    {
        m_lastError = "Failed to initialize the WAV reader";
        return false;
    }

    m_spec.m_total_length = wav->totalPCMFrameCount;
    m_spec.m_channels = wav->channels;
    m_spec.m_sample_rate = wav->sampleRate;

    drwav_uint16 format = drwav_fmt_get_format(&wav->fmt);
    bool isAiff = wav->container == drwav_container_aiff;
    bool isBE = isAiff ? !wav->aiff.isLE : false;
    bool isUnsigned8 = isAiff && wav->aiff.isUnsigned || !isAiff;

    initChPermute();

    switch(format)
    {
    case DR_WAVE_FORMAT_PCM:
        if(wav->bitsPerSample == 8)
            m_spec.m_sample_format = isUnsigned8 ? AUDIO_U8 : AUDIO_S8;
        else if(wav->bitsPerSample == 16)
            m_spec.m_sample_format = isBE ? AUDIO_S16MSB : AUDIO_S16LSB;
        else if(wav->bitsPerSample == 24)
        {
            m_cvt_needed = true;
            m_spec.m_sample_format = isBE ? AUDIO_S32MSB : AUDIO_S32LSB;
        }
        else if(wav->bitsPerSample == 32)
            m_spec.m_sample_format = isBE ? AUDIO_S32MSB : AUDIO_S32LSB;
        else
        {
            m_lastError = "Unknown bits per PCM sample of WAV: " + std::to_string(wav->bitsPerSample);
            close();
            return false;
        }
        break;

    case DR_WAVE_FORMAT_IEEE_FLOAT:
        if(wav->bitsPerSample == 32)
            m_spec.m_sample_format = isBE ? AUDIO_F32MSB : AUDIO_F32LSB;
        else if(wav->bitsPerSample == 64)
        {
            m_cvt_needed = true;
            m_spec.m_sample_format = isBE ? AUDIO_F32MSB : AUDIO_F32LSB;
        }
        else
        {
            m_lastError = "Unknown bits per Float sample of WAV: " + std::to_string(wav->bitsPerSample);
            close();
            return false;
        }
        break;

    case DR_WAVE_FORMAT_ALAW:
    case DR_WAVE_FORMAT_MULAW:
    case DR_WAVE_FORMAT_ADPCM:
    case DR_WAVE_FORMAT_DVI_ADPCM:
        m_cvt_needed = true;
        if(m_spec.m_sample_format != AUDIO_S16SYS && m_spec.m_sample_format != AUDIO_F32SYS)
            m_spec.m_sample_format = AUDIO_S16SYS;
        break;

    default:
        m_lastError = "Unknown sample format of WAV: " + std::to_string(format);
        close();
        return false;
    }

    for(drwav_uint32 i = 0; i < wav->metadataCount; ++i)
    {
        const drwav_metadata &d = wav->pMetadata[i];

        switch(d.type)
        {
        case drwav_metadata_type_list_info_title:
            m_spec.m_meta_title = std::string(d.data.infoText.pString, d.data.infoText.stringLength);
            break;
        case drwav_metadata_type_list_info_artist:
            m_spec.m_meta_artist = std::string(d.data.infoText.pString, d.data.infoText.stringLength);
            break;
        case drwav_metadata_type_list_info_album:
            m_spec.m_meta_album = std::string(d.data.infoText.pString, d.data.infoText.stringLength);
            break;
        case drwav_metadata_type_list_info_copyright:
            m_spec.m_meta_copyright = std::string(d.data.infoText.pString, d.data.infoText.stringLength);
            break;
        case drwav_metadata_type_smpl:
            for(size_t j = 0; j < d.data.smpl.sampleLoopCount; ++j)
            {
                const drwav_smpl_loop &l = d.data.smpl.pLoops[j];
                if(l.type != drwav_smpl_loop_type_forward)
                    continue;

                if(m_spec.m_loop_len == 0)
                {
                    m_spec.m_loop_start = l.firstSampleOffset;
                    m_spec.m_loop_end = l.lastSampleOffset;
                }
                else
                {
                    if(m_spec.m_loop_start > l.firstSampleOffset)
                        m_spec.m_loop_start = l.firstSampleOffset;

                    if(m_spec.m_loop_end < l.lastSampleOffset)
                        m_spec.m_loop_end = l.lastSampleOffset;
                }

                m_spec.m_loop_len = m_spec.m_loop_end - m_spec.m_loop_start;
            }
            break;
        default:
            break;
        }
    }

    /* Ignore invalid loop tag */
    if(m_spec.m_loop_start < 0 || m_spec.m_loop_len < 0 || m_spec.m_loop_end < 0)
    {
        m_spec.m_loop_start = 0;
        m_spec.m_loop_len = 0;
        m_spec.m_loop_end = 0;
    }

    return true;
}

bool MDAudioWAV::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    drwav_allocation_callbacks allocs;
    drwav_data_format format;
    drwav *wav = M_WAV;

    close();

    m_write = true;
    m_file = file;

    m_spec = dstSpec;

    m_io_buffer.resize(4096);

    m_meta = new waveEncodeMeta;

    SDL_zero(allocs);
    allocs.onMalloc = sdl_wav_malloc;
    allocs.onRealloc = sdl_wav_realloc;
    allocs.onFree = sdl_wav_free;

    SDL_zero(format);
    format.channels = m_spec.m_channels;
    format.sampleRate = m_spec.m_sample_rate;

    switch(m_container)
    {
    default:
    case CONTAINER_RIFF:
        format.container = drwav_container_riff;
        break;
    case CONTAINER_AIFF:
        format.container = drwav_container_aiff;
        break;
    }

    switch(m_target)
    {
    case ENCODE_DEFAULT:
        if(m_spec.m_sample_format == AUDIO_U16LSB)
            m_spec.m_sample_format = AUDIO_S16LSB;
        else if(m_spec.m_sample_format == AUDIO_U16MSB)
            m_spec.m_sample_format = AUDIO_S16MSB;

        switch(m_spec.m_sample_format)
        {
        case AUDIO_F32LSB:
        case AUDIO_F32MSB:
            format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
            break;
        default:
            format.format = DR_WAVE_FORMAT_PCM;
            break;
        }

        format.bitsPerSample = SDL_AUDIO_BITSIZE(m_spec.m_sample_format);
        break;

    // case ENCODE_MS_ADPCM:
    //     m_spec.m_sample_format = AUDIO_S16SYS;
    //     format.format = DR_WAVE_FORMAT_ADPCM;
    //     format.bitsPerSample = 4;
    //     break;

    // case ENCODE_IMA_ADPCM:
    //     m_spec.m_sample_format = AUDIO_S16SYS;
    //     format.format = DR_WAVE_FORMAT_DVI_ADPCM;
    //     format.bitsPerSample = 4;
    //     break;

    // case ENCODE_MULAW:
    //     m_spec.m_sample_format = AUDIO_S16SYS;
    //     format.format = DR_WAVE_FORMAT_MULAW;
    //     format.bitsPerSample = 8;
    //     break;

    // case ENCODE_ALAW:
    //     m_spec.m_sample_format = AUDIO_S16SYS;
    //     format.format = DR_WAVE_FORMAT_ALAW;
    //     format.bitsPerSample = 8;
    //     break;

    case ENCODE_FLOAT64:
        m_spec.m_sample_format = AUDIO_F32SYS;
        format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
        format.bitsPerSample = 64;
        break;
    }

    initChPermute();

    m_meta->initMeta(m_spec);

    if(!drwav_init_write_with_metadata(wav, &format, sdl_wav_write, sdl_wav_seek, m_file, &allocs, m_meta->m_meta.data(), m_meta->m_meta.size()))
    {
        m_lastError = "Failed to initialize the WAV writer";
        return false;
    }

    return true;
}

bool MDAudioWAV::close()
{
    if(m_file)
    {
        drwav_uninit(M_WAV);
        m_file = nullptr;
    }

    if(m_meta)
        delete m_meta;

    m_meta = nullptr;
    m_write = false;
    m_io_buffer.clear();

    return true;
}

bool MDAudioWAV::readRewind()
{
    return drwav_seek_to_pcm_frame(M_WAV, 0);
}

size_t MDAudioWAV::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    size_t ret;
    drwav_uint64 frame_size;

    if(m_cvt_needed)
    {
        switch(m_spec.m_sample_format)
        {
        default:
        case AUDIO_S16LSB:
            frame_size = (sizeof(int16_t) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_s16le(M_WAV, outSize / frame_size, reinterpret_cast<drwav_int16*>(out));
            break;
        case AUDIO_S16MSB:
            frame_size = (sizeof(int16_t) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_s16be(M_WAV, outSize / frame_size, reinterpret_cast<drwav_int16*>(out));
            break;
        case AUDIO_S32LSB:
            frame_size = (sizeof(int32_t) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_s32le(M_WAV, outSize / frame_size, reinterpret_cast<drwav_int32*>(out));
            break;
        case AUDIO_S32MSB:
            frame_size = (sizeof(int32_t) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_s32be(M_WAV, outSize / frame_size, reinterpret_cast<drwav_int32*>(out));
            break;
        case AUDIO_F32LSB:
            frame_size = (sizeof(float) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_f32le(M_WAV, outSize / frame_size, reinterpret_cast<float*>(out));
            break;
        case AUDIO_F32MSB:
            frame_size = (sizeof(float) * m_spec.m_channels);
            ret = drwav_read_pcm_frames_f32be(M_WAV, outSize / frame_size, reinterpret_cast<float*>(out));
            break;
        }

        reorderChannels(out, ret * frame_size);
        return ret * frame_size;
    }
    else
    {
        ret = drwav_read_raw(M_WAV, outSize, out);
        reorderChannels(out, ret);
        return ret;
    }
}

size_t MDAudioWAV::writeChunk(uint8_t *in, size_t inSize)
{
    size_t frame_size, ret = 0;

    if(m_io_buffer.size() < inSize)
        m_io_buffer.resize(inSize);

    SDL_memcpy(m_io_buffer.data(), in, inSize);

    reorderChannels(m_io_buffer.data(), inSize);

    switch(m_target)
    {
    case ENCODE_DEFAULT:
        frame_size = (M_WAV->fmt.bitsPerSample / 8) * m_spec.m_channels;
        ret = drwav_write_pcm_frames(M_WAV, inSize / frame_size, in) * frame_size;
        break;

    // case ENCODE_MS_ADPCM:
    // case ENCODE_IMA_ADPCM:
    // case ENCODE_MULAW:
    // case ENCODE_ALAW:
        // frame_size = sizeof(int16_t) * m_spec.m_channels;
        // ret = drwav_write_pcm_frames(M_WAV, inSize / frame_size, in) * frame_size;
        break;

    case ENCODE_FLOAT64:
    {
        float *f = reinterpret_cast<float*>(in);
        double *d;
        frame_size = sizeof(float) * m_spec.m_channels;
        size_t samples = inSize / sizeof(float);

        if(m_double_cvt.size() < samples)
            m_double_cvt.resize(samples);

        d = m_double_cvt.data();

        for(size_t i = 0; i < samples; ++i)
            *(d++) = static_cast<double>(*(f++));

        ret = drwav_write_pcm_frames(M_WAV, inSize / frame_size, m_double_cvt.data()) * frame_size;
        break;
    }
    }

    return ret;
}
