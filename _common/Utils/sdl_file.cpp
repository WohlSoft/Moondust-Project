
#include "sdl_file.h"
#include "files.h"

#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_assert.h>

struct SdlFile_private
{
    SDL_RWops*  m_rw = NULL;
    std::string m_mode;
    std::string m_fileName;
    std::string m_errorString;
};

SdlFile::SdlFile()
{
    p = new SdlFile_private;
    SDL_assert(p);
}

SdlFile::SdlFile(const std::string &fileName)
{
    SDL_assert(p);
    p = new SdlFile_private;
    p->m_fileName = fileName;
}

SdlFile::SdlFile(const SdlFile &f)
{
    SDL_assert(f.p);
    p = new SdlFile_private;
    p->m_mode           = f.p->m_mode;
    p->m_fileName       = f.p->m_fileName;
    p->m_errorString    = f.p->m_errorString;
    if(f.p->m_rw)
    {
        p->m_rw = SDL_RWFromFile(p->m_fileName.c_str(), p->m_mode.c_str());
        if(!p->m_rw)
            p->m_errorString = SDL_GetError();
    }
}

SdlFile::~SdlFile()
{
    SDL_assert(p);
    if(p->m_rw)
    {
        SDL_RWclose(p->m_rw);
        p->m_rw = NULL;
    }
    delete p;
}

std::string SdlFile::errorString()
{
    return p->m_errorString;
}

void SdlFile::setFileName(const std::string &fileName)
{
    SDL_assert(p);
    if(p->m_rw)
        close();
    p->m_fileName = fileName;
}

bool SdlFile::open(int mode)
{
    if(!p)
        return false;
    if(p->m_rw)
    {
        p->m_errorString = "Already opened!";
        return false;
    }

    p->m_mode.clear();
    if(mode & Read)
        p->m_mode.push_back('r');
    if(mode & Write)
        p->m_mode.push_back('w');
    if(mode & Append)
        p->m_mode.push_back('a');
    if((mode & Text) == 0)
        p->m_mode.push_back('b');

    p->m_rw = SDL_RWFromFile(p->m_fileName.c_str(), p->m_mode.c_str());
    if(!p->m_rw)
    {
        p->m_errorString = SDL_GetError();
        return false;
    }
    return true;
}

bool SdlFile::exists()
{
    SDL_assert(p);

    if(p->m_fileName.empty())
        return false;
    return Files::fileExists(p->m_fileName);
}

bool SdlFile::close()
{
    SDL_assert(p);
    if(p->m_rw)
    {
        SDL_RWclose(p->m_rw);
        p->m_rw = NULL;
        return true;
    }
    return false;
}

int64_t SdlFile::size()
{
    int64_t size = 0;
    if(!p->m_rw)
        return size;
    Sint64 pos = SDL_RWtell(p->m_rw);
    SDL_RWseek(p->m_rw, 0, RW_SEEK_END);
    size = SDL_RWtell(p->m_rw);
    SDL_RWseek(p->m_rw, pos, RW_SEEK_SET);
    return size;
}

std::string SdlFile::readAll()
{
    std::string out;
    if(!p->m_rw)
        return out;

    int64_t sz = size();
    SDL_assert(sz < INT32_MAX);
    out.resize(static_cast<size_t>(sz));
    size_t sz_got = SDL_RWread(p->m_rw, &out[0], 1, static_cast<size_t>(sz));
    if(sz_got != static_cast<size_t>(sz))
        p->m_errorString = "Read error!";
    return out;
}
