#include <string.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

#include "audio_qoa.h"

#include <SDL2/SDL_stdinc.h>
#define QOA_IMPLEMENTATION
#define QOA_NO_STDIO
#define QOA_MALLOC  SDL_malloc
#define QOA_FREE    SDL_free
#define inline      SDL_INLINE
#include "qoa/qoa.h"
#undef inline


#pragma pack(push, 1)
struct XQOAHeaderRAW
{
    uint8_t  magic[4];
    uint32_t head_size;
    uint32_t qoa_data_size;
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t multitrack_chans;
    uint32_t multitrack_tracks;
};
#pragma pack(pop)


uint32_t MDAudioQOA::decode_frame()
{
    unsigned int samples_decoded = 0;
    size_t got_bytes, encoded_frame_size;

    encoded_frame_size = qoa_max_frame_size(&info);

    got_bytes = SDL_RWread(m_file, m_decode_buffer.data(), 1, encoded_frame_size);

    qoa_decode_frame(m_decode_buffer.data(), got_bytes, &info,
                    (int16_t*)m_sample_buffer.data(), &samples_decoded);

    sample_data_pos = 0;
    sample_data_len = samples_decoded;

    return samples_decoded;
}

uint32_t MDAudioQOA::encode_frame()
{
    unsigned int samples_encoded = 0;
    size_t got_bytes;

    got_bytes = qoa_encode_frame((int16_t*)m_sample_buffer.data(),
                                 &info, sample_data_pos, m_encode_buffer.data());

    if(SDL_RWwrite(m_file, m_encode_buffer.data(), 1, got_bytes) != got_bytes)
    {
        m_lastError = "QOA: Failed to write frame data to the file";
        return 0;
    }

    samples_encoded = sample_data_pos;

    written_samples += sample_data_pos;
    written_bytes += got_bytes;

    sample_data_pos = 0;

    return samples_encoded;
}

MDAudioQOA::MDAudioQOA(bool encodeXQOA) :
    MDAudioFile(),
    m_encodeXQOA(encodeXQOA)
{}

MDAudioQOA::~MDAudioQOA()
{
    MDAudioQOA::close();
}

uint32_t MDAudioQOA::getCodecSpec() const
{
    uint32_t ret = SPEC_READ | SPEC_WRITE;

    if(m_encodeXQOA)
        ret |= SPEC_LOOP_POINTS | SPEC_META_TAGS;

    return ret;
}

static bool readTag(std::string &dst, uint8_t *&label_pos, const uint8_t *label_end)
{
    uint32_t label_size;

    label_size = SDL_SwapBE32(*(Uint32*)label_pos);
    label_pos += 4;

    if(label_size > 0)
    {
        if(label_pos + label_size > label_end)
            return false;

        dst.resize(label_size);
        SDL_memcpy(&dst[0], label_pos, label_size);
        label_pos += label_size;
    }

    return true;
}


bool MDAudioQOA::openRead(SDL_RWops *file)
{
    uint8_t header[QOA_MIN_FILESIZE];
    std::vector<uint8_t> xqoa_header;
    uint32_t xqoa_head_size;
    Sint64 file_size;

    close();

    m_write = false;
    m_file = file;

    if(SDL_RWread(m_file, header, 1, QOA_MIN_FILESIZE) != QOA_MIN_FILESIZE)
    {
        m_lastError = "QOA: Can't read header.";
        close();
        return false;
    }

    file_size = SDL_RWsize(m_file);
    qoa_file_size = file_size;
    qoa_file_begin = 0;

    m_spec.m_sample_format = AUDIO_S16SYS;

    if(SDL_memcmp(header, "XQOA", 4) == 0)
    {
        XQOAHeaderRAW *head_raw;
        uint8_t *label_pos;
        uint8_t *label_end;
        // Read XQOA header

        // 0x00 XQOA - 4 bytes magic number
        SDL_RWseek(m_file, 4, SEEK_SET);
        // 0x04 UINT32 - BE Header size (a sum of meta-data sizes), also should work like an offcet to the data
        xqoa_head_size = SDL_ReadBE32(m_file);

        if((Sint64)xqoa_head_size >= file_size)
        {
            m_lastError = "XQOA: Header size is larger than the file!";
            close();
            return false;
        }

        xqoa_header.resize(xqoa_head_size);

        qoa_file_begin = xqoa_head_size;

        SDL_RWseek(m_file, 0, SEEK_SET);
        if(!SDL_RWread(m_file, xqoa_header.data(), xqoa_head_size, 1))
        {
            m_lastError = "XQOA: Can't read the header!";
            close();
            return false;
        }

        head_raw = reinterpret_cast<XQOAHeaderRAW*>(xqoa_header.data());

        // 0x08 UINT32 - BE Data size (total size of QOA data, file is valid when sum of header size and data size is equal to the actual file size)
        qoa_file_size = SDL_SwapBE32(head_raw->qoa_data_size);

        // 0x0C UINT32 - BE Loop start
        m_spec.m_loop_start = SDL_SwapBE32(head_raw->loop_start);
        // 0x10 UINT32 - BE Loop end - if 0 - no loop
        m_spec.m_loop_end = SDL_SwapBE32(head_raw->loop_end);

        if(m_spec.m_loop_end <= m_spec.m_loop_start && (m_spec.m_loop_end > 0 || m_spec.m_loop_start > 0))
        {
            // Invalid loop points
            m_spec.m_loop_start = 0;
            m_spec.m_loop_end = 0;
        }

        m_spec.m_loop_len = m_spec.m_loop_end - m_spec.m_loop_start;

        m_spec.m_multitrack_chans = SDL_SwapBE32(head_raw->multitrack_chans);
        m_spec.m_multitrack_tracks = SDL_SwapBE32(head_raw->multitrack_tracks);

        label_pos = xqoa_header.data() + sizeof(XQOAHeaderRAW);
        label_end = xqoa_header.data() + xqoa_header.size();

        // Title
        if(!readTag(m_spec.m_meta_title, label_pos, label_end))
        {
            m_lastError = "XQOA: Size of TITLE tag is bigger than supposed to be!";
            close();
            return false;
        }

        // Artist
        if(!readTag(m_spec.m_meta_artist, label_pos, label_end))
        {
            m_lastError = "XQOA: Size of ARTIST tag is bigger than supposed to be!";
            close();
            return false;
        }

        // Album
        if(!readTag(m_spec.m_meta_album, label_pos, label_end))
        {
            m_lastError = "XQOA: Size of ALBUM tag is bigger than supposed to be!";
            close();
            return false;
        }

        // Copyright
        if(!readTag(m_spec.m_meta_copyright, label_pos, label_end))
        {
            m_lastError = "XQOA: Size of COPYRIGHT tag is bigger than supposed to be!";
            close();
            return false;
        }

        SDL_RWseek(m_file, qoa_file_begin, SEEK_SET);

        // Read the normal QOA header
        if(SDL_RWread(m_file, header, 1, QOA_MIN_FILESIZE) != QOA_MIN_FILESIZE)
        {
            m_lastError = "QOA: Can't read header.";
            close();
            return false;
        }
    }

    if(SDL_memcmp(header, "qoaf", 4) != 0)
    {
        m_lastError = "QOA: Invalid file format.";
        close();
        return false;
    }

    first_frame_pos = qoa_decode_header(header, QOA_MIN_FILESIZE, &info);
    if(!first_frame_pos)
    {
        m_lastError = "QOA: Can't parse header.";
        close();
        return false;
    }

    SDL_RWseek(m_file, qoa_file_begin + first_frame_pos, SEEK_SET);
    sample_pos = 0;
    sample_data_len = 0;
    sample_data_pos = 0;

    m_decode_buffer.resize(qoa_max_frame_size(&info));
    m_sample_buffer.resize(info.channels * QOA_FRAME_LEN * sizeof(Sint16) * 2);

    m_spec.m_channels = info.channels;
    m_spec.m_sample_rate = info.samplerate;
    m_spec.m_total_length = info.samples;

    return true;
}

static bool writeMetaTag(SDL_RWops *m_file, const std::string &tag)
{
    size_t ret;

    ret = SDL_WriteBE32(m_file, (Uint32)tag.size()) * sizeof(Uint32);
    if(!tag.empty())
        ret += SDL_RWwrite(m_file, tag.c_str(), 1, tag.size());

    return ret == (sizeof(Uint32) + tag.size());
}

bool MDAudioQOA::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    uint8_t header[QOA_MIN_FILESIZE];
    std::vector<uint8_t> xqoa_header;
    uint32_t xqoa_head_size;
    uint32_t head_size;

    close();

    m_write = true;
    m_file = file;

    m_spec = dstSpec;

    written_bytes = 0;
    written_samples = 0;
    qoa_file_begin = 0;

    m_spec.m_sample_format = AUDIO_S16SYS; // Can only work with the PCM16

    info.channels = m_spec.m_channels;
    info.samplerate = m_spec.m_sample_rate;
    info.samples = m_spec.m_total_length;

    m_encode_buffer.resize(qoa_max_frame_size(&info));
    sample_data_len = QOA_FRAME_LEN;
    m_sample_buffer.resize(info.channels * QOA_FRAME_LEN * sizeof(Sint16) * 2);

    for(uint32_t c = 0; c < info.channels; c++)
    {
        /* Set the initial LMS weights to {0, 0, -1, 2}. This helps with the
        prediction of the first few ms of a file. */
        info.lms[c].weights[0] = 0;
        info.lms[c].weights[1] = 0;
        info.lms[c].weights[2] = -(1<<13);
        info.lms[c].weights[3] =  (1<<14);

        /* Explicitly set the history samples to 0, as we might have some
        garbage in there. */
        for(int i = 0; i < QOA_LMS_LEN; i++)
            info.lms[c].history[i] = 0;
    }

    // Create XQOA header
    if(m_encodeXQOA)
    {
        xqoa_header.resize(sizeof(XQOAHeaderRAW));
        XQOAHeaderRAW *head_raw = reinterpret_cast<XQOAHeaderRAW *>(xqoa_header.data());
        SDL_memcpy(head_raw->magic, "XQOA", 4);

        xqoa_head_size = sizeof(XQOAHeaderRAW) +
                         (sizeof(Uint32) * 4) +
                         m_spec.m_meta_title.size() +
                         m_spec.m_meta_album.size() +
                         m_spec.m_meta_artist.size() +
                         m_spec.m_meta_copyright.size();

        head_raw->head_size = SDL_SwapBE32(xqoa_head_size);
        head_raw->qoa_data_size = 0; // Will be filled later
        head_raw->loop_start = SDL_SwapBE32(m_spec.m_loop_start);
        head_raw->loop_end = SDL_SwapBE32(m_spec.m_loop_end);
        head_raw->multitrack_chans = SDL_SwapBE32(m_spec.m_multitrack_chans);
        head_raw->multitrack_tracks = SDL_SwapBE32(m_spec.m_multitrack_tracks);

        if(SDL_RWwrite(m_file, xqoa_header.data(), 1, xqoa_header.size()) != xqoa_header.size())
        {
            m_lastError = "XQOA: Failed to write the header data.";
            close();
            return false;
        }

        qoa_file_begin = xqoa_head_size;

        if(!writeMetaTag(m_file, m_spec.m_meta_title))
        {
            m_lastError = "XQOA: Failed to write the TITLE tag data.";
            close();
            return false;
        }

        if(!writeMetaTag(m_file, m_spec.m_meta_artist))
        {
            m_lastError = "XQOA: Failed to write the ARTIST tag data.";
            close();
            return false;
        }

        if(!writeMetaTag(m_file, m_spec.m_meta_album))
        {
            m_lastError = "XQOA: Failed to write the ALBUM tag data.";
            close();
            return false;
        }

        if(!writeMetaTag(m_file, m_spec.m_meta_copyright))
        {
            m_lastError = "XQOA: Failed to write the COPYRIGHT tag data.";
            close();
            return false;
        }
    }

    // Write initial header
    SDL_RWseek(m_file, qoa_file_begin, SEEK_SET);
    head_size = qoa_encode_header(&info, header);
    SDL_RWwrite(m_file, header, 1, head_size);

    written_bytes += head_size;

    return true;
}

bool MDAudioQOA::close()
{
    if(m_write)
    {
        uint8_t header[QOA_MIN_FILESIZE];
        uint32_t head_size;

        if(m_file)
        {
            if(sample_data_pos > 0)
                encode_frame(); // Finish the data that was left

            // Finalize the written data and update the header
            SDL_RWseek(m_file, qoa_file_begin, SEEK_SET);
            info.samples = written_samples;
            head_size = qoa_encode_header(&info, header);
            SDL_RWwrite(m_file, header, 1, head_size);

            if(m_encodeXQOA) // Also, save the size of the written data to the XQOA header
            {
                uint8_t size[4];
                SDL_RWseek(m_file, 0x08, SEEK_SET);
                *(uint32_t*)size = SDL_SwapBE32(written_bytes);
                SDL_RWwrite(m_file, size, 1, 4);
            }

            SDL_RWseek(m_file, 0, SEEK_SET);

            m_encode_buffer.clear();
            m_sample_buffer.clear();
        }

        written_bytes = 0;
        written_samples = 0;
        sample_data_pos = 0;
        sample_data_len = 0;

        m_file = nullptr;
    }
    else if(m_file)
    {
        first_frame_pos = 0;
        sample_pos = 0;
        sample_data_pos = 0;
        sample_data_len = 0;
        qoa_file_begin = 0;
        qoa_file_size = 0;

        m_decode_buffer.clear();
        m_sample_buffer.clear();

        SDL_RWseek(m_file, 0, SEEK_SET);

        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioQOA::readRewind()
{
    if(m_write)
    {
        m_lastError = "Function readRewind called when write is open.";
        return false; // This function works for read only!
    }

    SDL_RWseek(m_file, qoa_file_begin + first_frame_pos, SEEK_SET);
    sample_pos = 0;
    sample_data_len = 0;
    sample_data_pos = 0;

    return true;
}

size_t MDAudioQOA::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    int src_index = sample_data_pos * info.channels;
    int frame_size = (sizeof(int16_t) * info.channels);
    int samples_written = 0;
    int i, to_copy, samples_left;
    int16_t *out_samples = reinterpret_cast<int16_t*>(out);
    int16_t *sample_buffer = reinterpret_cast<int16_t*>(m_sample_buffer.data());
    int num_samples = outSize / frame_size;

    if(spec_changed)
        *spec_changed = false;

    for(i = 0; i < num_samples; )
    {
        /* Do we have to decode more samples? */
        if(sample_data_len - sample_data_pos == 0)
        {
            if(!decode_frame())
                break; /* Reached end of file */
            src_index = 0;
        }

        samples_left = (int)sample_data_len - sample_data_pos;

        if(samples_left <= num_samples - i)
            to_copy = samples_left;
        else
            to_copy = num_samples - i;

        if(to_copy <= 0)
            break; /* Something went wrong... */

        SDL_memcpy(out_samples, sample_buffer + src_index, to_copy * frame_size);
        out_samples += to_copy * info.channels;
        src_index += to_copy * info.channels;
        sample_data_pos += to_copy;
        sample_pos += to_copy;
        samples_written += to_copy;

        i += to_copy;
    }

    return samples_written * frame_size;
}

size_t MDAudioQOA::writeChunk(uint8_t *in, size_t inSize)
{
    int dst_index = sample_data_pos * info.channels;
    int frame_size = (sizeof(int16_t) * info.channels);
    int16_t *in_samples = reinterpret_cast<int16_t*>(in);
    int16_t *sample_buffer = reinterpret_cast<int16_t*>(m_sample_buffer.data());
    int num_samples = inSize / frame_size;
    int samples_written = 0;
    int i, to_copy, samples_left;

    for(i = 0; i < num_samples; )
    {
        /* Do we have to encode more samples? */
        if(sample_data_len - sample_data_pos == 0)
        {
            if(!encode_frame())
                break; /* Error of the write occurred */
            dst_index = 0;
        }

        samples_left = (int)sample_data_len - sample_data_pos;

        if(samples_left <= num_samples - i)
            to_copy = samples_left;
        else
            to_copy = num_samples - i;

        if(to_copy <= 0)
            break; /* Something went wrong... */

        SDL_memcpy(sample_buffer + dst_index, in_samples, to_copy * frame_size);
        in_samples += to_copy * info.channels;
        dst_index += to_copy * info.channels;
        sample_data_pos += to_copy;
        sample_pos += to_copy;
        samples_written += to_copy;

        i += to_copy;
    }

    return samples_written;
}
