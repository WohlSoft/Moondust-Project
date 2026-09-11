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
        // Continue at this point
        pos = tail + 1;
    }

    while(pos < args.size())
    {
        size_t tail;
        std::string key;
        std::string value;

        key.push_back(args[pos++]);

        // A secodn letter of key (for two-letter keys)
        if(args[pos] >= 'a' && args[pos] < 'z')
            key.push_back(args[pos++]);

        if(args[pos] == '=')
            key.push_back(args[pos++]);

        tail = args.find(';', pos);
        value = args.substr(pos, tail - pos);
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
