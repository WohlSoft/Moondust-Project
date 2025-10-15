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

#pragma once
#ifndef MUSIC_ARGS_H
#define MUSIC_ARGS_H

#include <string>
#include <map>

class MusicArgs
{
    typedef std::map<std::string, std::string> ArgsMap;
    ArgsMap m_args;
    int     m_argTrack = 0; //!< GME only track number

public:
    explicit MusicArgs(const std::string &args = std::string());
    MusicArgs(const MusicArgs &o);
    MusicArgs &operator=(const MusicArgs &o) = default;

    int getTrackNumber() const;

    bool setArgs(const std::string &args);

    int getArgI(const std::string &key, int def) const;
    bool getArgB(const std::string &key, bool def) const;
    int getArgBI(const std::string &key, bool def) const;
    float getArgF(const std::string &key, float def) const;
    double getArgD(const std::string &key, double def) const;
    std::string getArgS(const std::string &key, const std::string def) const;
};

#endif // MUSIC_ARGS_H
