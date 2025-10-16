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
};

bool MDAudioFFMPEG::updateStream()
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

                if(channels > 2)
                    layout.u.mask = AV_CH_LAYOUT_SURROUND;
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
                if(channels > 2)
                    layout = AV_CH_LAYOUT_SURROUND;
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

int MDAudioFFMPEG::decode_packet(const AVPacket *pkt, bool *got_some)
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

        updateStream();

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
    return false;
}

bool MDAudioFFMPEG::close()
{
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

retry:
    if(m_buffer_left == 0)
    {
        /* read frames from the file */
        while(av_read_frame(p->fmt_ctx, p->pkt) >= 0)
        {
            /* check if the packet belongs to a stream we are interested in, otherwise */
            /* skip it */
            if(p->pkt->stream_index == p->stream_index)
                ret = decode_packet(p->pkt, &got_some);

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
