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
#ifndef AUDIO_MIDI_OPN_H
#define AUDIO_MIDI_OPN_H

#include <vector>

#include "../audio_file.h"

class MDAudioOPNMIDI : public MDAudioFile
{
    struct OPN2_MIDIPlayer *m_synth = nullptr;

    int volume_model = 0;
    int alloc_mode = -1;
    int chips_count = -1;
    int full_brightness_range = 0;
    int auto_arpeggio = 0;
    int soft_pan = 1;
    int emulator = -1;
    std::string custom_bank_path;
    double tempo = 1.0;
    float gain = 1.0f;

    bool reCreateSynth();
    bool reInit();
    bool reOpenFile();

    struct AudioFormat_t
    {
        /*! type of sample */
        int type;
        /*! size in bytes of the storage type */
        unsigned containerSize;
        /*! distance in bytes between consecutive samples */
        unsigned sampleOffset;
    } m_sample_format;

    std::vector<uint8_t> m_in_file;

public:
    MDAudioOPNMIDI();
    virtual ~MDAudioOPNMIDI();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_MIDI_OPN_H
