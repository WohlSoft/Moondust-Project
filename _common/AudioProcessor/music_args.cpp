#include <SDL2/SDL_stdinc.h>
#include "music_args.h"


MusicArgs::MusicArgs(const std::string &args)
{
    if(!args.empty())
        setArgs(args);
}

MusicArgs::MusicArgs(const MusicArgs &o)
{
    m_args = o.m_args;
    m_argTrack = o.m_argTrack;
}

int MusicArgs::getTrackNumber() const
{
    return m_argTrack;
}

bool MusicArgs::setArgs(const std::string &args)
{
    std::string chunk;
    size_t pos = 0;
    m_argTrack = 0;
    m_args.clear();

    if(args.empty())
        return false;

    // Begins with digit
    if(args[0] >= '0' && args[0] <= '9')
    {
        size_t tail = args.find(';');
        if(tail == std::string::npos)
        {
            // Entire args string is a number
            m_argTrack = SDL_atoi(args.c_str());
            return true;
        }

        chunk = args.substr(0, tail);
        m_argTrack = SDL_atoi(chunk.c_str());
    }

    while(pos < args.size())
    {
        size_t tail;
        std::string key;
        std::string value;

        key.push_back(args[pos++]);

        if(args[pos] == '=')
            key.push_back(args[pos++]);

        tail = args.find(';');
        value = args.substr(pos, tail);
        pos += value.size() + 1;

        m_args.insert({key, value});
    }

    return true;
}

int MusicArgs::getArgI(const std::string &key, int def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return SDL_atoi(v->second.c_str());
}

bool MusicArgs::getArgB(const std::string &key, bool def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return v->second != "0";
}

int MusicArgs::getArgBI(const std::string &key, bool def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return v->second != "0" ? 1 : 0;
}

float MusicArgs::getArgF(const std::string &key, float def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return (float)SDL_strtod(v->second.c_str(), NULL);
}

double MusicArgs::getArgD(const std::string &key, double def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return SDL_strtod(v->second.c_str(), NULL);
}

std::string MusicArgs::getArgS(const std::string &key, const std::string def) const
{
    ArgsMap::const_iterator v = m_args.find(key);
    if(v == m_args.end())
        return def;

    return v->second;
}
