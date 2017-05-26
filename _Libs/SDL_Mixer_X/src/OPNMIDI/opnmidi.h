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

#ifndef OPNMIDI_H
#define OPNMIDI_H

#ifdef __cplusplus
extern "C" {
#endif

enum OPNMIDI_VolumeModels
{
    OPNMIDI_VolumeModel_AUTO = 0,
    OPNMIDI_VolumeModel_Generic,
    OPNMIDI_VolumeModel_CMF,
    OPNMIDI_VolumeModel_DMX,
    OPNMIDI_VolumeModel_APOGEE,
    OPNMIDI_VolumeModel_9X
};

struct OPN2_MIDIPlayer
{
    unsigned int OpnBank;
    unsigned int NumCards;
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

    void *opn2_midiPlayer;
    unsigned long PCM_RATE;
};

/* Sets number of emulated sound cards (from 1 to 100). Emulation of multiple sound cards exchanges polyphony limits*/
extern int  opn2_setNumCards(struct OPN2_MIDIPlayer *device, int numCards);

/*Enable or disable Enables scaling of modulator volumes*/
extern void opn2_setScaleModulators(struct OPN2_MIDIPlayer *device, int smod);

/*Enable or disable built-in loop (built-in loop supports 'loopStart' and 'loopEnd' tags to loop specific part)*/
extern void opn2_setLoopEnabled(struct OPN2_MIDIPlayer *device, int loopEn);

/*Enable or disable Logariphmic volume changer */
extern void opn2_setLogarithmicVolumes(struct OPN2_MIDIPlayer *device, int logvol);

/*Set different volume range model */
extern void opn2_setVolumeRangeModel(struct OPN2_MIDIPlayer *device, int volumeModel);


/*Returns string which contains last error message*/
extern const char *opn2_errorString();

/*Initialize ADLMIDI Player device*/
extern struct OPN2_MIDIPlayer *opn2_init(long sample_rate);

/*Load WOPN bank file from File System*/
extern int opn2_openBankFile(struct OPN2_MIDIPlayer *device, char *filePath);

/*Load WOPN bank file from memory data*/
extern int opn2_openBankData(struct OPN2_MIDIPlayer *device, void *mem, long size);

/*Load MIDI file from File System*/
extern int opn2_openFile(struct OPN2_MIDIPlayer *device, char *filePath);

/*Load MIDI file from memory data*/
extern int opn2_openData(struct OPN2_MIDIPlayer *device, void *mem, long size);

/*Resets MIDI player*/
extern void opn2_reset(struct OPN2_MIDIPlayer *device);

/*Close and delete ADLMIDI device*/
extern void opn2_close(struct OPN2_MIDIPlayer *device);

/*Take a sample buffer*/
extern int  opn2_play(struct OPN2_MIDIPlayer *device, int sampleCount, short out[]);

#ifdef __cplusplus
}
#endif

#endif /* OPNMIDI_H */
