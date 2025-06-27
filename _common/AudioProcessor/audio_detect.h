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
#ifndef AUDIO_DETECT_H
#define AUDIO_DETECT_H

#include <string>

#include "audio_format.h"

typedef enum Mix_MIDI_Player
{
    MIDI_ADLMIDI,
    MIDI_Native,
    MIDI_Timidity,
    MIDI_OPNMIDI,
    MIDI_Fluidsynth,
    MIDI_EDMIDI,
    MIDI_ANY,
    MIDI_KnownPlayers, /* Count of MIDI player types */
    MIDI_KnownDevices = MIDI_KnownPlayers /* Backward compatibility */
} Mix_MIDI_Player;

extern int midiplayer_current;

typedef struct SDL_RWops SDL_RWops;

AudioFormats audio_detect_format(SDL_RWops *src, std::string &error);

#endif // AUDIO_DETECT_H
