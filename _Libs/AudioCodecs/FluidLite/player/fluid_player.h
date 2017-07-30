/*
 * libADLMIDI is a free MIDI to WAV conversion library with OPL3 emulation
 *
 * Original ADLMIDI code: Copyright (c) 2010-2014 Joel Yliluoma <bisqwit@iki.fi>
 * ADLMIDI Library API:   Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef ADLMIDI_H
#define ADLMIDI_H

#ifdef __cplusplus
extern "C" {
#endif

struct Fluid_MIDIPlayer
{
    unsigned int AdlBank;
    unsigned int NumFourOps;
    unsigned int NumCards;
    unsigned int HighTremoloMode;
    unsigned int HighVibratoMode;
    unsigned int AdlPercussionMode;
    unsigned int LogarithmicVolumes;
    int VolumeModel;
    unsigned int QuitFlag;
    unsigned int SkipForward;
    unsigned int QuitWithoutLooping;
    unsigned int ScaleModulators;
    double delay;
    double carry;

    /* The lag between visual content and audio content equals */
    /* the sum of these two buffers. */
    double mindelay;
    double maxdelay;

    /* For internal usage */
    int     stored_samples; /* num of collected samples */
    short   backup_samples[1024]; /* Backup sample storage. */
    int     backup_samples_size; /* Backup sample storage. */
    /* For internal usage */

    void *adl_midiPlayer;
    unsigned long PCM_RATE;
};

/*Enable or disable built-in loop (built-in loop supports 'loopStart' and 'loopEnd' tags to loop specific part)*/
extern void fluid_setLoopEnabled(struct Fluid_MIDIPlayer *device, int loopEn);

/*Returns string which contains last error message*/
extern const char *fluid_errorString();

/*Initialize ADLMIDI Player device*/
extern struct Fluid_MIDIPlayer *fluid_play_init(long sample_rate);

/*Load MIDI file from File System*/
extern int fluid_openFile(struct Fluid_MIDIPlayer *device, char *filePath);

/*Load MIDI file from memory data*/
extern int fluid_openData(struct Fluid_MIDIPlayer *device, void *mem, long size);

/*Resets MIDI player*/
extern void fluid_reset(struct Fluid_MIDIPlayer *device);

/*Close and delete ADLMIDI device*/
extern void fluid_close(struct Fluid_MIDIPlayer *device);

/*Take a sample buffer*/
extern int  fluid_play(struct Fluid_MIDIPlayer *device, int sampleCount, short out[]);

#ifdef __cplusplus
}
#endif

#endif /* ADLMIDI_H */
