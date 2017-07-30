/*
 * libOPNMIDI is a free MIDI to WAV conversion library with OPN2 (YM2612) emulation
 *
 * MIDI parser and player (Original code from ADLMIDI): Copyright (c) 2010-2014 Joel Yliluoma <bisqwit@iki.fi>
 * OPNMIDI Library and YM2612 support:   Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Library is based on the ADLMIDI, a MIDI player for Linux and Windows with OPL3 emulation:
 * http://iki.fi/bisqwit/source/adlmidi.html
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "opnmidi_private.hpp"

#ifdef OPN2MIDI_buildAsApp
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#endif

static const unsigned MaxCards = 100;

/*---------------------------EXPORTS---------------------------*/

OPNMIDI_EXPORT struct OPN2_MIDIPlayer *opn2_init(long sample_rate)
{
    OPN2_MIDIPlayer *_device;
    _device = (OPN2_MIDIPlayer *)malloc(sizeof(OPN2_MIDIPlayer));
    _device->PCM_RATE = static_cast<unsigned long>(sample_rate);
    _device->OpnBank    = 0;
    //_device->NumFourOps = 7;
    _device->NumCards   = 2;
    _device->LogarithmicVolumes = 0;
    _device->QuitFlag = 0;
    _device->SkipForward = 0;
    _device->QuitWithoutLooping = 0;
    _device->ScaleModulators = 0;
    _device->delay = 0.0;
    _device->carry = 0.0;
    _device->mindelay = 1.0 / (double)_device->PCM_RATE;
    _device->maxdelay = 512.0 / (double)_device->PCM_RATE;
    _device->stored_samples = 0;
    _device->backup_samples_size = 0;
    OPNMIDIplay *player = new OPNMIDIplay;
    _device->opn2_midiPlayer = player;
    player->config = _device;
    player->opn._parent = _device;
    player->opn.NumCards = _device->NumCards;
    player->opn.LogarithmicVolumes = (bool)_device->LogarithmicVolumes;
    player->opn.ScaleModulators = (bool)_device->ScaleModulators;
    player->ChooseDevice("none");
    opn2RefreshNumCards(_device);
    return _device;
}

OPNMIDI_EXPORT int opn2_setNumCards(OPN2_MIDIPlayer *device, int numCards)
{
    if(device == NULL)
        return -2;

    device->NumCards = static_cast<unsigned int>(numCards);
    reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->opn.NumCards = device->NumCards;

    if(device->NumCards < 1 || device->NumCards > MaxCards)
    {
        std::stringstream s;
        s << "number of cards may only be 1.." << MaxCards << ".\n";
        OPN2MIDI_ErrorString = s.str();
        return -1;
    }

    return opn2RefreshNumCards(device);
}

OPNMIDI_EXPORT void opn2_setScaleModulators(OPN2_MIDIPlayer *device, int smod)
{
    if(!device) return;

    device->ScaleModulators = static_cast<unsigned int>(smod);
    reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->opn.ScaleModulators = (bool)device->ScaleModulators;
}

OPNMIDI_EXPORT void opn2_setLoopEnabled(OPN2_MIDIPlayer *device, int loopEn)
{
    if(!device) return;

    device->QuitWithoutLooping = (loopEn == 0);
}

OPNMIDI_EXPORT void opn2_setLogarithmicVolumes(struct OPN2_MIDIPlayer *device, int logvol)
{
    if(!device) return;

    device->LogarithmicVolumes = static_cast<unsigned int>(logvol);
    reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->opn.LogarithmicVolumes = (bool)device->LogarithmicVolumes;
}

OPNMIDI_EXPORT void opn2_setVolumeRangeModel(OPN2_MIDIPlayer *device, int volumeModel)
{
    if(!device) return;

    device->VolumeModel = volumeModel;
    reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->opn.ChangeVolumeRangesModel(static_cast<OPNMIDI_VolumeModels>(volumeModel));
}

OPNMIDI_EXPORT int opn2_openBankFile(OPN2_MIDIPlayer *device, char *filePath)
{
    OPN2MIDI_ErrorString.clear();

    if(device && device->opn2_midiPlayer)
    {
        device->stored_samples = 0;
        device->backup_samples_size = 0;

        if(!reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->LoadBank(filePath))
        {
            if(OPN2MIDI_ErrorString.empty())
                OPN2MIDI_ErrorString = "OPN2 MIDI: Can't load file";

            return -1;
        }
        else return 0;
    }

    OPN2MIDI_ErrorString = "Can't load file: OPN2 MIDI is not initialized";
    return -1;
}

OPNMIDI_EXPORT int opn2_openBankData(OPN2_MIDIPlayer *device, void *mem, long size)
{
    OPN2MIDI_ErrorString.clear();

    if(device && device->opn2_midiPlayer)
    {
        device->stored_samples = 0;
        device->backup_samples_size = 0;

        if(!reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->LoadBank(mem, static_cast<size_t>(size)))
        {
            if(OPN2MIDI_ErrorString.empty())
                OPN2MIDI_ErrorString = "OPN2 MIDI: Can't load data from memory";

            return -1;
        }
        else return 0;
    }

    OPN2MIDI_ErrorString = "Can't load file: OPN2 MIDI is not initialized";
    return -1;
}

OPNMIDI_EXPORT int opn2_openFile(OPN2_MIDIPlayer *device, char *filePath)
{
    OPN2MIDI_ErrorString.clear();

    if(device && device->opn2_midiPlayer)
    {
        device->stored_samples = 0;
        device->backup_samples_size = 0;

        if(!reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->LoadMIDI(filePath))
        {
            if(OPN2MIDI_ErrorString.empty())
                OPN2MIDI_ErrorString = "OPN2 MIDI: Can't load file";

            return -1;
        }
        else return 0;
    }

    OPN2MIDI_ErrorString = "Can't load file: OPN2 MIDI is not initialized";
    return -1;
}

OPNMIDI_EXPORT int opn2_openData(OPN2_MIDIPlayer *device, void *mem, long size)
{
    OPN2MIDI_ErrorString.clear();

    if(device && device->opn2_midiPlayer)
    {
        device->stored_samples = 0;
        device->backup_samples_size = 0;

        if(!reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->LoadMIDI(mem, static_cast<size_t>(size)))
        {
            if(OPN2MIDI_ErrorString.empty())
                OPN2MIDI_ErrorString = "OPN2 MIDI: Can't load data from memory";

            return -1;
        }
        else return 0;
    }

    OPN2MIDI_ErrorString = "Can't load file: OPN2 MIDI is not initialized";
    return -1;
}


OPNMIDI_EXPORT const char *opn2_errorString()
{
    return OPN2MIDI_ErrorString.c_str();
}

OPNMIDI_EXPORT const char *adl_getMusicTitle(OPN2_MIDIPlayer *device)
{
    if(!device) return "";

    return reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->musTitle.c_str();
}

OPNMIDI_EXPORT void opn2_close(OPN2_MIDIPlayer *device)
{
    if(device->opn2_midiPlayer)
        delete reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer);

    device->opn2_midiPlayer = NULL;
    free(device);
    device = NULL;
}

OPNMIDI_EXPORT void opn2_reset(OPN2_MIDIPlayer *device)
{
    if(!device)
        return;

    device->stored_samples = 0;
    device->backup_samples_size = 0;
    reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->opn.Reset();
}


inline static void SendStereoAudio(OPN2_MIDIPlayer *device,
                                   int      &samples_requested,
                                   ssize_t  &in_size,
                                   short    *_in,
                                   ssize_t   out_pos,
                                   short    *_out)
{
    if(!in_size)
        return;

    device->stored_samples = 0;
    size_t offset       = static_cast<size_t>(out_pos);
    size_t inSamples    = static_cast<size_t>(in_size * 2);
    size_t maxSamples   = static_cast<size_t>(samples_requested) - offset;
    size_t toCopy       = std::min(maxSamples, inSamples);
    memcpy(_out + out_pos, _in, toCopy * sizeof(short));

    if(maxSamples < inSamples)
    {
        size_t appendSize = inSamples - maxSamples;
        memcpy(device->backup_samples + device->backup_samples_size,
               maxSamples + _in, appendSize * sizeof(short));
        device->backup_samples_size += appendSize;
        device->stored_samples += appendSize;
    }
}


OPNMIDI_EXPORT int opn2_play(OPN2_MIDIPlayer *device, int sampleCount, short *out)
{
    if(!device)
        return 0;

    sampleCount -= sampleCount % 2; //Avoid even sample requests

    if(sampleCount < 0)
        return 0;

    if(device->QuitFlag)
        return 0;

    ssize_t gotten_len = 0;
    ssize_t n_periodCountStereo = 512;
    //ssize_t n_periodCountPhys = n_periodCountStereo * 2;
    int left = sampleCount;

    while(left > 0)
    {
        if(device->backup_samples_size > 0)
        {
            //Send reserved samples if exist
            ssize_t ate = 0;

            while((ate < device->backup_samples_size) && (ate < left))
            {
                out[ate] = device->backup_samples[ate];
                ate++;
            }

            left -= ate;
            gotten_len += ate;

            if(ate < device->backup_samples_size)
            {
                for(int j = 0; j < ate; j++)
                    device->backup_samples[(ate - 1) - j] = device->backup_samples[(device->backup_samples_size - 1) - j];
            }

            device->backup_samples_size -= ate;
        }
        else
        {
            const double eat_delay = device->delay < device->maxdelay ? device->delay : device->maxdelay;
            device->delay -= eat_delay;
            device->carry += device->PCM_RATE * eat_delay;
            n_periodCountStereo = static_cast<ssize_t>(device->carry);
            //n_periodCountPhys = n_periodCountStereo * 2;
            device->carry -= n_periodCountStereo;

            if(device->SkipForward > 0)
                device->SkipForward -= 1;
            else
            {
                std::vector<int16_t> out_buf;
                out_buf.resize(1024 /*n_samples * 2*/);

                ssize_t in_generatedStereo = (n_periodCountStereo > 512) ? 512 : n_periodCountStereo;
                ssize_t in_generatedPhys = in_generatedStereo * 2;

                //fill buffer with zeros
                size_t in_countStereoU = static_cast<size_t>(in_generatedStereo * 2);
                memset(out_buf.data(), 0, in_countStereoU * sizeof(short));
                if(device->NumCards == 1)
                {
                    (reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer))->opn.cardsOP2[0]->run(int(in_generatedStereo), out_buf.data());
                    /* Process it */
                    SendStereoAudio(device, sampleCount, in_generatedStereo, out_buf.data(), gotten_len, out);
                }
                else if(n_periodCountStereo > 0)
                {
                    memset(out_buf.data(), 0, in_countStereoU * sizeof(short));
                    /* Generate data from every chip and mix result */
                    for(unsigned card = 0; card < device->NumCards; ++card)
                    {
                        (reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer))->opn.cardsOP2[card]->run(int(in_generatedStereo), out_buf.data());
                    }
                    /* Process it */
                    SendStereoAudio(device, sampleCount, in_generatedStereo, out_buf.data(), gotten_len, out);
                }

                left -= in_generatedPhys;
                gotten_len += (in_generatedPhys) - device->stored_samples;
            }

            device->delay = reinterpret_cast<OPNMIDIplay *>(device->opn2_midiPlayer)->Tick(eat_delay, device->mindelay);
        }
    }

    return static_cast<int>(gotten_len);
}
