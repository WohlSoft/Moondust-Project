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
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_assert.h>
#include "audio_ffmpeg.h"

#define inline SDL_INLINE
#define av_always_inline SDL_INLINE
#define __STDC_CONSTANT_MACROS

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/mem.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavutil/dict.h>
#include <libswresample/swresample.h>
}

#if LIBAVFORMAT_VERSION_MAJOR >= 61
typedef const uint8_t av8write_buf;
#else
typedef uint8_t av8write_buf;
#endif

#define AUDIO_INBUF_SIZE 4096

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(59, 24, 100)
#define AVCODEC_NEW_CHANNEL_LAYOUT
#endif

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(59, 0, 100)
#define AVFORMAT_NEW_avcodec_find_decoder
#endif

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(60, 12, 100)
#define AVFORMAT_NEW_avio_alloc_context
#endif

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(59, 0, 100)
#define AVFORMAT_NEW_avformat_open_input
#define AVFORMAT_NEW_av_find_best_stream
#endif

#if LIBSWRESAMPLE_VERSION_INT >= AV_VERSION_INT(4, 14, 100)
#define AVFORMAT_NEW_swr_convert
#endif

static inline std::string mix_av_make_error_string(int errnum)
{
    std::string ret;
    ret.resize(AV_ERROR_MAX_STRING_SIZE);
    av_strerror(errnum, &ret[0], ret.size());

    size_t cut = ret.find('\0');
    if(cut != std::string::npos)
        ret.erase(cut);

    return ret;
}

#define mix_av_err2str(errnum) \
    mix_av_make_error_string(errnum)


struct MDAudioFFMPEG_private
{
    // DEcoding
    Sint64 src_start = 0;
    AVFormatContext *fmt_ctx = nullptr;
    AVIOContext     *avio_in = nullptr;
#ifdef AVFORMAT_NEW_avcodec_find_decoder
    const AVCodec *codec = nullptr;
#else
    AVCodec *codec = nullptr;
#endif
    AVCodecContext *audio_dec_ctx = nullptr;
    AVStream *audio_stream = nullptr;
    AVCodecParserContext *parser = nullptr;
    AVPacket *pkt = nullptr;
    AVFrame *decoded_frame = nullptr;
    enum AVSampleFormat sfmt = AV_SAMPLE_FMT_NONE;
    bool planar = false;
    enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_NONE;
    SwrContext *swr_ctx = nullptr;
    std::vector<uint8_t> merge_buffer;
    bool stream_init = false;

    int srate = 0;
    int schannels = 0;
    int stream_index = 0;

    double time_duration = 0;

    Uint8 *in_buffer = nullptr;
    size_t in_buffer_size = 0;

    // ENcoding
    AVCodecContext *audio_enc_ctx = nullptr;
    uint64_t src_ch_layout = 0;
    uint64_t dst_ch_layout = 0;
    int dst_channels = 0;
    AVFrame *encode_frame = nullptr;
    AVIOContext *avio_out = nullptr;

    void find_supported_fmt()
    {
        const enum AVSampleFormat *cur = codec->sample_fmts;

        // First, check if desired sample format is okay
        while(*cur != AV_SAMPLE_FMT_NONE)
        {
            if(*cur == dst_sample_fmt)
                return; // Found! Keep as-is
            ++cur;
        }

        // Then, find closest one across supported
        cur = codec->sample_fmts;
        while(*cur != AV_SAMPLE_FMT_NONE)
        {
            switch(dst_sample_fmt)
            {
            case AV_SAMPLE_FMT_U8:
                if(*cur == AV_SAMPLE_FMT_U8 || *cur == AV_SAMPLE_FMT_U8P)
                {
                    dst_sample_fmt = *cur;
                    return;
                }
                break;

            case AV_SAMPLE_FMT_S16:
                if(*cur == AV_SAMPLE_FMT_S16 || *cur == AV_SAMPLE_FMT_S16P)
                {
                    dst_sample_fmt = *cur;
                    return;
                }
                break;
            case AV_SAMPLE_FMT_S32:
                if(*cur == AV_SAMPLE_FMT_S32 || *cur == AV_SAMPLE_FMT_S32P)
                {
                    dst_sample_fmt = *cur;
                    return;
                }
                break;
            case AV_SAMPLE_FMT_FLT:
                if(*cur == AV_SAMPLE_FMT_FLT || *cur == AV_SAMPLE_FMT_FLTP ||
                   *cur == AV_SAMPLE_FMT_DBL || *cur == AV_SAMPLE_FMT_DBLP)
                {
                    dst_sample_fmt = *cur;
                    return;
                }
                break;
            default:
                break;
            }
            ++cur;
        }
    }

    void find_supported_rate()
    {
        const int *p;
        int found_rate = 0;
        int rate_diff = 0;
        int rate_diff_cur = 0;

        if(!codec->supported_samplerates)
            return; // Any rates supported

        p = codec->supported_samplerates;
        while(*p)
        {
            found_rate = FFMAX(*p, found_rate);
            rate_diff_cur = SDL_abs(found_rate - srate);

            if(rate_diff == 0)
                rate_diff = rate_diff_cur;
            else if(rate_diff > rate_diff_cur)
            {
                rate_diff = rate_diff_cur;
                if(rate_diff == 0)
                {
                    srate = found_rate;
                    return;
                }
            }

            p++;
        }

        srate = found_rate;
    }


    void select_chgannel_layout()
    {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
        const AVChannelLayout *p;
#else
        const uint64_t *p;
#endif

        switch(schannels)
        {
        case 0:
        case 1:
            src_ch_layout = AV_CH_LAYOUT_MONO;
            break;
        case 2:
            src_ch_layout = AV_CH_LAYOUT_STEREO;
            break;
        case 3:
            src_ch_layout = AV_CH_LAYOUT_2_1;
            break;
        case 4:
            src_ch_layout = AV_CH_LAYOUT_QUAD;
            break;
        case 5:
            src_ch_layout = AV_CH_LAYOUT_4POINT1;
            break;
        case 6:
            src_ch_layout = AV_CH_LAYOUT_5POINT1;
            break;
        case 7:
            src_ch_layout = AV_CH_LAYOUT_7POINT0;
            break;
        case 8:
            src_ch_layout = AV_CH_LAYOUT_7POINT1;
            break;
        default:
            src_ch_layout = AV_CH_LAYOUT_SURROUND;
            break;
        }

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
        p = codec->ch_layouts;
#else
        p = codec->channel_layouts;
#endif
        dst_ch_layout = 0;
        dst_channels = schannels;

        if(!p)
            dst_ch_layout = src_ch_layout;
        else
        {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
            while(p)
#else
            while(*p)
#endif
            {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                if(p->u.mask == src_ch_layout)
#else
                if(*p == src_ch_layout)
#endif
                {
                    dst_ch_layout = src_ch_layout;
                    break;
                }

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                int nb_channels = p->nb_channels;
#else
                int nb_channels = av_get_channel_layout_nb_channels(*p);
#endif
                if(nb_channels == dst_channels)
                {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                    dst_ch_layout = p->u.mask;
#else
                    dst_ch_layout = *p;
#endif
                    break;
                }

                p++;
            }

            if(dst_ch_layout == 0) // Found nothing!
            {
                int best_channels = 0;
                uint64_t last_dst = 0;
                int last_ch = 0;

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                p = codec->ch_layouts;
#else
                p = codec->channel_layouts;
#endif

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                while(p)
#else
                while(*p)
#endif
                {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                    last_ch = p->nb_channels;
                    last_dst = p->u.mask;
#else
                    last_ch = av_get_channel_layout_nb_channels(*p);
                    last_dst = *p;
#endif

                    if(last_ch > best_channels)
                        best_channels = last_ch;

                    if(best_channels >= dst_channels)
                    {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
                        dst_ch_layout = p->u.mask;
#else
                        dst_ch_layout = *p;
#endif
                        break;
                    }

                    p++;
                }

                if(dst_ch_layout == 0)
                {
                    dst_channels = last_ch;
                    dst_ch_layout = last_dst;
                }
            }
        }
    }
};

bool MDAudioFFMPEG::updateStream(bool *spec_changed)
{
    SDL_assert(p->audio_stream->codecpar);
    enum AVSampleFormat sfmt = (enum AVSampleFormat)p->audio_stream->codecpar->format;
    int srate = p->audio_stream->codecpar->sample_rate;

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
    int channels = p->audio_stream->codecpar->ch_layout.nb_channels;
#else
    int channels = p->audio_stream->codecpar->channels;
#endif

    int fmt = 0;

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
    AVChannelLayout layout;
#else
    int layout;
#endif

    if(srate == 0 || channels == 0)
    {
        m_lastError = "FFMPEG: Sample rate or channels number is zero";
        return false;
    }

    if(sfmt != p->sfmt || srate != p->srate || channels != p->schannels || !p->stream_init)
    {
        p->planar = SDL_FALSE;

        switch(sfmt)
        {
        case AV_SAMPLE_FMT_U8P:
            p->planar = true;
            p->dst_sample_fmt = AV_SAMPLE_FMT_U8;
            /*fallthrough*/
        case AV_SAMPLE_FMT_U8:
            fmt = AUDIO_U8;
            break;

        case AV_SAMPLE_FMT_S16P:
            p->planar = true;
            p->dst_sample_fmt = AV_SAMPLE_FMT_S16;
            /*fallthrough*/
        case AV_SAMPLE_FMT_S16:
            fmt = AUDIO_S16SYS;
            break;

        case AV_SAMPLE_FMT_S32P:
            p->planar = true;
            p->dst_sample_fmt = AV_SAMPLE_FMT_S32;
            /*fallthrough*/
        case AV_SAMPLE_FMT_S32:
            fmt = AUDIO_S32SYS;
            break;

        case AV_SAMPLE_FMT_FLTP:
            p->planar = true;
            p->dst_sample_fmt = AV_SAMPLE_FMT_FLT;
            /*fallthrough*/
        case AV_SAMPLE_FMT_FLT:
            fmt = AUDIO_F32SYS;
            break;

        default:
            m_lastError = "FFMPEG: Unsupported audio format: " + std::to_string(sfmt);
            return false; /* Unsupported audio format */
        }

        p->merge_buffer.clear();

        if(p->swr_ctx)
        {
            swr_free(&p->swr_ctx);
            p->swr_ctx = nullptr;
        }

        p->stream_init = true;

        if(p->planar)
        {
            p->swr_ctx = swr_alloc();
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
            layout = p->audio_stream->codecpar->ch_layout;
#else
            layout = p->audio_stream->codecpar->channel_layout;
#endif

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
            if(layout.u.mask == 0)
            {
                layout.order = AV_CHANNEL_ORDER_NATIVE;
                layout.nb_channels = channels;

                if(channels > 8)
                    layout.u.mask = AV_CH_LAYOUT_SURROUND;
                else if(channels == 8)
                    layout.u.mask = AV_CH_LAYOUT_7POINT1;
                else if(channels == 7)
                    layout.u.mask = AV_CH_LAYOUT_7POINT0;
                else if(channels == 6)
                    layout.u.mask = AV_CH_LAYOUT_5POINT1;
                else if(channels == 5)
                    layout.u.mask = AV_CH_LAYOUT_4POINT1;
                else if(channels == 4)
                    layout.u.mask = AV_CH_LAYOUT_QUAD;
                else if(channels == 3)
                    layout.u.mask = AV_CH_LAYOUT_2_1;
                else if(channels == 2)
                    layout.u.mask = AV_CH_LAYOUT_STEREO;
                else if(channels == 1)
                    layout.u.mask = AV_CH_LAYOUT_MONO;
            }

            av_opt_set_chlayout(p->swr_ctx, "in_chlayout",  &layout, 0);
            av_opt_set_chlayout(p->swr_ctx, "out_chlayout", &layout, 0);
#else
            if(layout == 0)
            {
                if(channels > 8)
                    layout = AV_CH_LAYOUT_SURROUND;
                else if(channels == 8)
                    layout = AV_CH_LAYOUT_7POINT1;
                else if(channels == 7)
                    layout = AV_CH_LAYOUT_7POINT0;
                else if(channels == 6)
                    layout = AV_CH_LAYOUT_5POINT1;
                else if(channels == 5)
                    layout = AV_CH_LAYOUT_4POINT1;
                else if(channels == 4)
                    layout = AV_CH_LAYOUT_QUAD;
                else if(channels == 3)
                    layout = AV_CH_LAYOUT_2_1;
                else if(channels == 2)
                    layout = AV_CH_LAYOUT_STEREO;
                else if(channels == 1)
                    layout = AV_CH_LAYOUT_MONO;
            }

            av_opt_set_int(p->swr_ctx, "in_channel_layout",  layout, 0);
            av_opt_set_int(p->swr_ctx, "out_channel_layout", layout, 0);
#endif
            av_opt_set_int(p->swr_ctx, "in_sample_rate",     srate, 0);
            av_opt_set_int(p->swr_ctx, "out_sample_rate",    srate, 0);
            av_opt_set_sample_fmt(p->swr_ctx, "in_sample_fmt",  sfmt, 0);
            av_opt_set_sample_fmt(p->swr_ctx, "out_sample_fmt", p->dst_sample_fmt,  0);
            swr_init(p->swr_ctx);

#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
            av_channel_layout_uninit(&layout);
#endif

            p->merge_buffer.resize(channels * av_get_bytes_per_sample(sfmt) * 4096);
        }

        p->sfmt = sfmt;
        p->srate = srate;
        p->schannels = channels;

        m_spec.m_channels = channels;
        m_spec.m_sample_format = fmt;
        m_spec.m_sample_rate = srate;

        if(spec_changed)
            *spec_changed = true;
    }

    return true;
}

int MDAudioFFMPEG::_rw_read_buffer(void *opaque, uint8_t *buf, int buf_size)
{
    MDAudioFFMPEG *music = reinterpret_cast<MDAudioFFMPEG*>(opaque);
    size_t ret = SDL_RWread(music->m_file, buf, 1, buf_size);

    if(ret == 0)
        return AVERROR_EOF;

    return ret;
}

int64_t MDAudioFFMPEG::_rw_seek(void *opaque, int64_t offset, int whence)
{
    MDAudioFFMPEG *music = reinterpret_cast<MDAudioFFMPEG*>(opaque);
    int rw_whence;

    switch(whence)
    {
    default:
    case SEEK_SET:
        rw_whence = RW_SEEK_SET;
        offset += music->p->src_start;
        break;
    case SEEK_CUR:
        rw_whence = RW_SEEK_CUR;
        break;
    case SEEK_END:
        rw_whence = RW_SEEK_END;
        break;
    case AVSEEK_SIZE:
        return SDL_RWsize(music->m_file);
    }

    return SDL_RWseek(music->m_file, offset, rw_whence);
}

int md_audio_ffmpeg_rw_write_cb(void *opaque, av8write_buf *data, int size)
{
    MDAudioFFMPEG *music = reinterpret_cast<MDAudioFFMPEG*>(opaque);
    return (int)SDL_RWwrite(music->m_file, data, 1, size);
}

int MDAudioFFMPEG::decode_packet(const AVPacket *pkt, bool *got_some, bool *spec_changed)
{
    int ret = 0;
    size_t unpadded_linesize;
    size_t sample_size;

    *got_some = SDL_FALSE;

    ret = avcodec_send_packet(p->audio_dec_ctx, pkt);
    if(ret < 0)
    {
        if(ret == AVERROR_EOF)
            return ret;

        m_lastError = "ERROR: Error submitting a packet for decoding:";
        m_lastError += mix_av_err2str(ret);
        return ret;
    }

    /* get all the available frames from the decoder */
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(p->audio_dec_ctx, p->decoded_frame);
        if(ret < 0)
        {
            /* those two return values are special and mean there is no output */
            /* frame available, but there were no errors during decoding */
            if(ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            m_lastError = "FFMPEG: Error during decoding: ";
            m_lastError += mix_av_err2str(ret);
            return ret;
        }

        updateStream(spec_changed);

        if(p->planar)
        {
            sample_size = av_get_bytes_per_sample((enum AVSampleFormat)p->decoded_frame->format);
            unpadded_linesize = sample_size * p->decoded_frame->nb_samples * p->schannels;

            if(unpadded_linesize > p->merge_buffer.size())
                p->merge_buffer.resize(unpadded_linesize);

            Uint8 *s = p->merge_buffer.data();

            swr_convert(p->swr_ctx,
                        &s, p->decoded_frame->nb_samples,
                        (const Uint8**)p->decoded_frame->extended_data, p->decoded_frame->nb_samples);

            if(m_io_buffer.size() < (size_t)unpadded_linesize)
                m_io_buffer.resize(unpadded_linesize);

            SDL_memcpy(m_io_buffer.data(), p->merge_buffer.data(), unpadded_linesize);
            m_buffer_left = unpadded_linesize;
            m_buffer_pos = 0;
        }
        else
        {
            unpadded_linesize = p->decoded_frame->nb_samples * av_get_bytes_per_sample((enum AVSampleFormat)p->decoded_frame->format);

            if(m_io_buffer.size() < (size_t)unpadded_linesize)
                m_io_buffer.resize(unpadded_linesize);

            SDL_memcpy(m_io_buffer.data(), p->decoded_frame->extended_data[0], unpadded_linesize);
            m_buffer_left = unpadded_linesize;
            m_buffer_pos = 0;
        }

        av_frame_unref(p->decoded_frame);

        *got_some = SDL_TRUE;

        if (ret < 0)
            return ret;
    }

    return 0;
}

MDAudioFFMPEG::MDAudioFFMPEG(EncodeTarget encode) :
    MDAudioFile(),
    p(new MDAudioFFMPEG_private),
    m_target(encode)
{}

MDAudioFFMPEG::~MDAudioFFMPEG()
{
    MDAudioFFMPEG::close();
    delete p;
}

uint32_t MDAudioFFMPEG::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_META_TAGS;
}

bool MDAudioFFMPEG::openRead(SDL_RWops *file)
{
    const AVDictionaryEntry *tag = NULL;
    char proto[] = "file:///sdl_rwops";
    int ret;

    close();

    m_write = false;
    m_file = file;

    p->in_buffer = (uint8_t *)av_malloc(AUDIO_INBUF_SIZE);
    p->in_buffer_size = AUDIO_INBUF_SIZE;

    if(!p->in_buffer)
    {
        m_lastError = "FFMPEG: Out of memory";
        close();
        return false;
    }

    p->fmt_ctx = avformat_alloc_context();
    if(!p->fmt_ctx)
    {
        m_lastError = "FFMPEG: Failed to allocate format context";
        close();
        return false;
    }

    p->avio_in = avio_alloc_context(p->in_buffer,
                                    p->in_buffer_size,
                                    0,
                                    this,
                                    _rw_read_buffer,
                                    NULL,
                                    _rw_seek);
    if(!p->avio_in)
    {
        m_lastError = "FFMPEG: Unhandled file format";
        close();
        return false;
    }

    p->fmt_ctx->pb = p->avio_in;
    p->fmt_ctx->url = proto;

    ret = avformat_open_input(&p->fmt_ctx, NULL, NULL, NULL);
    if(ret < 0)
    {
        m_lastError = "FFMPEG: Failed to open the input: ";
        m_lastError += mix_av_err2str(ret);
        close();
        return false;
    }

    ret = avformat_find_stream_info(p->fmt_ctx, NULL);
    if(ret < 0)
    {
        m_lastError = "FFMPEG: Could not find stream information:";
        m_lastError += mix_av_err2str(ret);
        close();
        return false;
    }

    ret = av_find_best_stream(p->fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &p->codec, 0);
    if(ret < 0)
    {
        m_lastError = "FFMPEG: Could not find audio stream in input file: ";
        m_lastError  += mix_av_err2str(ret);
        close();
        return false;
    }

    p->stream_index = ret;
    p->audio_stream = p->fmt_ctx->streams[p->stream_index];

    if(!p->codec)
    {
        p->codec = avcodec_find_decoder(p->audio_stream->codecpar->codec_id);
        if(!p->codec)
        {
            m_lastError = "FFMPEG: Failed to find audio codec";
            close();
            return false;
        }
    }

    p->audio_dec_ctx = avcodec_alloc_context3(p->codec);
    if(!p->audio_dec_ctx)
    {
        m_lastError = "FFMPEG: Failed allocate the codec context";
        close();
        return false;
    }

    if(!p->audio_stream->codecpar)
    {
        m_lastError = "FFMPEG: codec parameters aren't recognised";
        close();
        return false;
    }

    if(avcodec_parameters_to_context(p->audio_dec_ctx, p->audio_stream->codecpar) < 0)
    {
        m_lastError = "FFMPEG: Failed to copy codec parameters to context";
        close();
        return false;
    }

    ret = avcodec_open2(p->audio_dec_ctx, p->codec, NULL);
    if(ret < 0)
    {
        m_lastError = "FFMPEG: Failed to initialise the decoder: ";
        m_lastError += mix_av_err2str(ret);
        close();
        return false;
    }

    p->decoded_frame = av_frame_alloc();
    if(!p->decoded_frame)
    {
        m_lastError = "FFMPEG: Failed to allocate the frame";
        close();
        return false;
    }

    p->pkt = av_packet_alloc();
    if(!p->pkt)
    {
        m_lastError = "FFMPEG: Failed to allocate the packet";
        close();
        return false;
    }

    if(!updateStream())
    {
        m_lastError = "FFMPEG: Failed to initialise the stream";
        close();
        return false;
    }

    m_spec.m_total_length = 0;

    if(p->audio_stream->nb_frames > 0)
        m_spec.m_total_length = p->audio_stream->nb_frames;


    if(p->fmt_ctx->duration != AV_NOPTS_VALUE && p->fmt_ctx->duration != 0)
    {
        p->time_duration = (double)p->fmt_ctx->duration / AV_TIME_BASE;

        if(m_spec.m_total_length == 0)
            m_spec.m_total_length = (int64_t)(p->time_duration * p->audio_stream->codecpar->sample_rate);
    }
    else if(p->audio_stream->nb_frames > 0)
    {
        /* NOTE: This method may compute an inaccurate time depending on a format. Used as fallback when "duration" is invalid */
        p->time_duration = (double)p->audio_stream->nb_frames / p->audio_stream->codecpar->sample_rate;
    }
    else
    {
        /* Unknown duration */
        p->time_duration = -1.0;
    }

    av_dump_format(p->fmt_ctx, p->stream_index, "<SDL_RWops context 1>", 0);

    /* Attempt to load metadata */

    if(p->fmt_ctx->metadata)
    {
        tag = av_dict_get(p->fmt_ctx->metadata, "title", tag, AV_DICT_MATCH_CASE);
        if(tag)
            m_spec.m_meta_title = tag->value;

        tag = av_dict_get(p->fmt_ctx->metadata, "artist", tag, AV_DICT_MATCH_CASE);
        if(tag)
            m_spec.m_meta_artist = tag->value;
        else
        {
            /* Try to search for "author" instead */
            tag = av_dict_get(p->fmt_ctx->metadata, "author", tag, AV_DICT_MATCH_CASE);
            if(tag)
                m_spec.m_meta_artist = tag->value;
        }

        tag = av_dict_get(p->fmt_ctx->metadata, "album", tag, AV_DICT_MATCH_CASE);
        if(tag)
            m_spec.m_meta_album = tag->value;

        tag = av_dict_get(p->fmt_ctx->metadata, "copyright", tag, AV_DICT_MATCH_CASE);
        if(tag)
            m_spec.m_meta_copyright = tag->value;
    }

    return true;
}

bool MDAudioFFMPEG::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    AVCodecID tCodec = AV_CODEC_ID_NONE;
    close();

    m_write = true;
    m_file = file;

    m_spec = dstSpec;

    switch(m_target)
    {
    default:
    case ENCODE_UNDEFINED:
        m_lastError = "FFMPEG: Encoding is not configured!";
        return false;
    case ENCODE_WMAv1:
        tCodec = AV_CODEC_ID_WMAV1;
        break;
    case ENCODE_WMAv2:
        tCodec = AV_CODEC_ID_WMAV2;
        break;
    case ENCODE_WAV_MULAW:
        tCodec = AV_CODEC_ID_PCM_MULAW;
        break;
    case ENCODE_WAV_ALAW:
        tCodec = AV_CODEC_ID_PCM_ALAW;
        break;
    case ENCODE_WAV_ADPCM_MS:
        tCodec = AV_CODEC_ID_ADPCM_MS;
        break;
    case ENCODE_WAV_ADPCM_IMA:
        tCodec = AV_CODEC_ID_ADPCM_IMA_WAV;
        break;
    case ENCODE_AIFF:
        break;
    }

    p->codec = avcodec_find_encoder(tCodec);
    if(!p->codec)
    {
        m_lastError = "Can't find a codec: ";
        m_lastError += avcodec_get_name(tCodec);
        return false;
    }

    p->audio_enc_ctx = avcodec_alloc_context3(p->codec);
    if(!p->audio_enc_ctx)
    {
        m_lastError = "FFMPEG: Can't allocate audio codec context";
        close();
        return false;
    }

    // Input sample format
    switch(m_spec.m_sample_format)
    {
    case AUDIO_U8:
        p->sfmt = AV_SAMPLE_FMT_U8;
        break;
    case AUDIO_S8:
        m_spec.m_sample_format = AUDIO_U8; // Can't S8
        p->sfmt = AV_SAMPLE_FMT_U8;
        break;
    case AUDIO_U16MSB:
    case AUDIO_U16LSB:
    case AUDIO_S16MSB:
    case AUDIO_S16LSB:
        m_spec.m_sample_format = AUDIO_S16SYS; // Only S16 system endianess
        p->sfmt = AV_SAMPLE_FMT_S16;
        break;
    case AUDIO_S32MSB:
    case AUDIO_S32LSB:
        m_spec.m_sample_format = AUDIO_S32SYS; // Only S32 system endianess
        p->sfmt = AV_SAMPLE_FMT_S32;
        break;
    case AUDIO_F32MSB:
    case AUDIO_F32LSB:
        m_spec.m_sample_format = AUDIO_F32SYS; // Only F32 system endianess
        p->sfmt = AV_SAMPLE_FMT_FLT;
        break;
    }

    p->schannels = m_spec.m_channels;
    p->select_chgannel_layout();

    p->dst_sample_fmt = p->sfmt;
    p->audio_enc_ctx->bit_rate = m_spec.bitrate > 0 ? m_spec.bitrate : 128000;

    p->find_supported_fmt();
    p->audio_enc_ctx->sample_fmt = p->dst_sample_fmt; // Target sample format, if mismatches, then conversion is required

    p->srate = m_spec.m_sample_rate;
    p->find_supported_rate();

    if(p->srate != m_spec.m_sample_rate)
        m_spec.m_sample_rate = p->srate;

    p->audio_enc_ctx->sample_rate = p->srate;
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
    p->audio_enc_ctx->ch_layout.nb_channels = p->dst_channels;
    p->audio_enc_ctx->ch_layout.u.mask = p->dst_ch_layout;
#else
    p->audio_enc_ctx->channels = p->dst_channels;
    p->audio_enc_ctx->channel_layout = p->dst_ch_layout;
#endif

    if(avcodec_open2(p->audio_enc_ctx, p->codec, nullptr) < 0)
    {
        m_lastError = "FFMPEG: Could not open codec";
        close();
        return false;
    }

    int buffer_size = av_samples_get_buffer_size(NULL,
                                                 p->dst_channels,
                                                 p->audio_enc_ctx->frame_size,
                                                 p->audio_enc_ctx->sample_fmt, 0);
    if(buffer_size < 0)
    {
        m_lastError = "FFMPEG: Failed to calculate buffer size";
        close();
        return false;
    }

    m_io_buffer.resize(buffer_size);

    p->avio_out = avio_alloc_context(m_io_buffer.data(),
                                     m_io_buffer.size(),
                                     AVIO_FLAG_READ_WRITE,
                                     this,
                                     _rw_read_buffer,
                                     md_audio_ffmpeg_rw_write_cb,
                                     _rw_seek);

    if(!p->avio_out)
    {
        m_lastError = "FFMPEG: Failed to initialize AVIO interface";
        close();
        return false;
    }



    // Init resampler
    if(p->src_ch_layout != p->dst_ch_layout || p->schannels != p->dst_channels || p->sfmt != p->dst_sample_fmt)
    {
#if defined(AVCODEC_NEW_CHANNEL_LAYOUT)
        AVChannelLayout layout;
#else
        int layout;
#endif
        p->swr_ctx = swr_alloc();
        p->merge_buffer.clear();
    }



    return false;
}

bool MDAudioFFMPEG::close()
{
    if(p->encode_frame)
    {
        av_frame_free(&p->encode_frame);
        p->encode_frame = nullptr;
    }

    if(p->avio_out)
    {
        avio_context_free(&p->avio_out);
        if(p->fmt_ctx)
            p->fmt_ctx->pb = nullptr;
        p->avio_out = nullptr;
    }

    if(p->audio_enc_ctx)
    {
        avcodec_close(p->audio_enc_ctx);
        p->audio_enc_ctx = nullptr;
    }

    if(p->audio_dec_ctx)
    {
        avcodec_free_context(&p->audio_dec_ctx);
        p->audio_dec_ctx = nullptr;
    }

    if(p->fmt_ctx)
    {
        avformat_close_input(&p->fmt_ctx);
        p->fmt_ctx = nullptr;
    }

    if(p->pkt)
    {
        av_packet_free(&p->pkt);
        p->pkt = nullptr;
    }

    if(p->decoded_frame)
    {
        av_frame_free(&p->decoded_frame);
        p->decoded_frame = nullptr;
    }

    if(p->swr_ctx)
    {
        swr_free(&p->swr_ctx);
        p->swr_ctx = nullptr;
    }

    p->in_buffer = NULL; /* This buffer is already freed by FFMPEG side*/
    p->in_buffer_size = 0;

    p->merge_buffer.clear();

    m_io_buffer.clear();
    m_buffer_left = 0;
    m_buffer_pos = 0;

    m_write = false;
    m_file = nullptr;

    return true;
}

bool MDAudioFFMPEG::readRewind()
{
    av_seek_frame(p->fmt_ctx, p->stream_index, 0, AVSEEK_FLAG_ANY);
    m_buffer_left = 0;
    m_buffer_pos = 0;
    return true;
}

size_t MDAudioFFMPEG::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    size_t out_offset = 0;
    size_t written = 0;
    bool got_some = false;
    int ret = 0;

    if(spec_changed)
        *spec_changed = false;

retry:
    if(m_buffer_left == 0)
    {
        /* read frames from the file */
        while(av_read_frame(p->fmt_ctx, p->pkt) >= 0)
        {
            /* check if the packet belongs to a stream we are interested in, otherwise */
            /* skip it */
            if(p->pkt->stream_index == p->stream_index)
                ret = decode_packet(p->pkt, &got_some, spec_changed);

            av_packet_unref(p->pkt);

            if(ret < 0 || got_some)
                break;
        }
    }

    if(m_buffer_left > 0 && outSize > 0)
    {
        if(outSize <= m_buffer_left)
        {
            SDL_memcpy(out + out_offset, m_io_buffer.data() + m_buffer_pos, outSize);
            written += outSize;
            out_offset += outSize;
            m_buffer_left -= outSize;
            m_buffer_pos += outSize;
        }
        else
        {
            SDL_memcpy(out + out_offset, m_io_buffer.data() + m_buffer_pos, m_buffer_left);
            outSize -= m_buffer_left;
            written += m_buffer_left;
            out_offset += m_buffer_left;
            m_buffer_left -= m_buffer_left;
            m_buffer_pos += m_buffer_left;
            goto retry;
        }
    }

    if(written > 0)
        return written;
    else if(!got_some || ret == AVERROR_EOF)
        return 0;

    goto retry;
}

size_t MDAudioFFMPEG::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
