#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_opus.h"

#include <opus/opusfile.h>

int MDAudioOpus::set_op_error(const char *function, int error)
{
#define HANDLE_ERROR_CASE(X) case X: m_lastError = std::string(function) + ": " + std::string(#X); break;
    switch(error)
    {
    HANDLE_ERROR_CASE(OP_FALSE)
    HANDLE_ERROR_CASE(OP_EOF)
    HANDLE_ERROR_CASE(OP_HOLE)
    HANDLE_ERROR_CASE(OP_EREAD)
    HANDLE_ERROR_CASE(OP_EFAULT)
    HANDLE_ERROR_CASE(OP_EIMPL)
    HANDLE_ERROR_CASE(OP_EINVAL)
    HANDLE_ERROR_CASE(OP_ENOTFORMAT)
    HANDLE_ERROR_CASE(OP_EBADHEADER)
    HANDLE_ERROR_CASE(OP_EVERSION)
    HANDLE_ERROR_CASE(OP_ENOTAUDIO)
    HANDLE_ERROR_CASE(OP_EBADPACKET)
    HANDLE_ERROR_CASE(OP_EBADLINK)
    HANDLE_ERROR_CASE(OP_ENOSEEK)
    HANDLE_ERROR_CASE(OP_EBADTIMESTAMP)
    default:
        m_lastError = std::string(function) + ": unknown error " + std::to_string(error);
        break;
    }
    return -1;
}

bool MDAudioOpus::updateSection()
{
    return false;
}

void MDAudioOpus::writeFlush()
{

}

MDAudioOpus::MDAudioOpus() :
    MDAudioFile()
{}

MDAudioOpus::~MDAudioOpus()
{
    MDAudioOpus::close();
}

uint32_t MDAudioOpus::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_SAMPLE_RATE;
}

static int sdl_read_func(void *datasource, unsigned char *ptr, int size)
{
    return (int)SDL_RWread((SDL_RWops*)datasource, ptr, 1, (size_t)size);
}

static int sdl_seek_func(void *datasource, opus_int64 offset, int whence)
{
    return (SDL_RWseek((SDL_RWops*)datasource, offset, whence) < 0)? -1 : 0;
}

static opus_int64 sdl_tell_func(void *datasource)
{
    return SDL_RWtell((SDL_RWops*)datasource);
}

bool MDAudioOpus::openRead(SDL_RWops *file)
{
    return false;
}

bool MDAudioOpus::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    return false;
}

bool MDAudioOpus::close()
{
    return false;
}

bool MDAudioOpus::readRewind()
{
    return false;
}

size_t MDAudioOpus::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    return 0;
}

size_t MDAudioOpus::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
