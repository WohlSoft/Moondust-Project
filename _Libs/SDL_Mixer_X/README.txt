SDL_mixer_ext (aka SDL Mixer 2.0 Modded or SDL Mixer X),
by Vitaly Novichkov <Wohlstand>,
forked from SDL Mixer 2.0 by Sam Lantinga <slouken@libsdl.org>

vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
WARNING: The licenses for libmad, ADLMIDI and GME is GPL,
         which means that in order to use it your application must
         also be GPL!
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The latest original version of this library is available from:
http://www.libsdl.org/projects/SDL_mixer/

Sources of modified library version is available in the PGE Project's sources:
https://github.com/Wohlhabend-Networks/PGE-Project/
    in the folder: _Libs/SDL2_mixer_modified/
or:
https://bitbucket.org/Wohlstand/pge-project
    in the folder: _Libs/SDL2_mixer_modified/

=============================================================================
Difference between original and this library:
-----------------------------------------------------------------------------
+ Added new codecs: 
  - Game Music Emulators (LGPL v2.1) which adds support of chip tunes 
        like NSF, VGM, SPC, HES, etc.
  - libADLMIDI (GPL v3, LGPL v3) (remake from ADLMIDI) to play MIDI with 
        emulated OPL synthesiser, also supports loop points
        "loopStart" and "loopEnd"
+ Added some new functions
+ Added support of loop points for OGG files (via "LOOPSTART" and "LOOPEND"
  (or "LOOPLENGTH" to be compatible with RPG Maker) vorbis comments)

vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
IMPORTANT: To choice a track number of NSF, GBM, HES, etc file,
           you must append "|xxx" to end of file path for 
           Mix_LoadMUS function.
           Where xxx - actual number of chip track, (from 0 to N-1)
           Examples: "file.nsf|12", "file.hes|2"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
Arguments are passing like argument for a GME-based files (NSF, HES, etc.):
Syntax for MIDI is:
        myfile.mid|xyy;xyy;xyy;...;
    where x - parameter type
    where y - value (every value must be ended with semicolon!)

available parameters:
        s - use a specific synthesiser
            0 - ADLMIDI
            1 - Native MIDI [Win32/OSX/Haiku only]
            2 - Timidity
            3 - Fluidsynth
        b - value from 0 to 66 - number of ADLMIDI bank
        t - (0 or 1) enable deep tremolo on ADLMIDI
        v - (0 or 1) enable deep vibrato on ADLMIDI
        m - (0 or 1) enable scalable modulation on ADLMIDI
        a - (0 or 1) enable AdLib mode of percussion on ADLMIDI
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

=============================================================================
Added functions:
-----------------------------------------------------------------------------
void MIX_Timidity_addToPathList(const char *path);

  //Allows you to set up custom path for Timidify patches


const char* Mix_GetMusicTitle(const Mix_Music *music);

  //Get music title from meta-tag if possible. If title tag is empty, filename will be returned


const char* Mix_GetMusicTitleTag(const Mix_Music *music);

  //Get music title from meta-tag if possible

const char* Mix_GetMusicArtistTag(const Mix_Music *music);

  //Get music artist from meta-tag if possible


const char* Mix_GetMusicAlbumTag(const Mix_Music *music);

  //Get music album from meta-tag if possible


const char* Mix_GetMusicCopyrightTag(const Mix_Music *music);

  //Get music copyright from meta-tag if possible


Mix_Music * SDLCALLCC Mix_LoadMUS_RW_ARG(SDL_RWops *src, int freesrc, char *args)

    //Load music from memory with passing arguments


Mix_Music * SDLCALLCC Mix_LoadMUS_RW_GME(SDL_RWops *src, int freesrc, int trackID)

    //Load music from memory with passing NSF/HES/etc. track number (accepts integer unlike SDLCALLCC Mix_LoadMUS_RW_ARG)


typedef enum {
    MIDI_ADLMIDI,
    MIDI_Native,
    MIDI_Timidity,
    MIDI_Fluidsynth
} Mix_MIDI_Device;

int MIX_SetMidiDevice(int device);

  //Allows you to toggle MIDI Devices!
  // (change will be applied on re-opening of MIDI file)
  // Attempt to toggle unsupported MIDI device takes no effect
  // (for case when library built without linking of required library)

int  MIX_ADLMIDI_getBankID();

  //Returns current ADLMIDI bank number


void MIX_ADLMIDI_setBankID(int bnk);

  //Changes ADLMIDI bank number (changes applying on MIDI file reopen)


int  MIX_ADLMIDI_getTremolo();

  //Returns current state of ADLMIDI deep tremolo flag


void MIX_ADLMIDI_setTremolo(int tr);

  //Changes ADLMIDI deep tremolo flag (changes applying on MIDI file reopen)


int  MIX_ADLMIDI_getVibrato();

  //Returns current state of ADLMIDI deep vibrato flag


void MIX_ADLMIDI_setVibrato(int vib);

  //Changes ADLMIDI deep vibrato flag (changes applying on MIDI file reopen)

int  MIX_ADLMIDI_getScaleMod();

  //Returns current state of ADLMIDI deep scaling modulation flag


void MIX_ADLMIDI_setScaleMod(int sc);

  //Changes ADLMIDI scaling modulation flag (changes applying on MIDI file reopen)


void MIX_ADLMIDI_setSetDefaults();

  //Resets ADLMIDI flags and settings to default state


=============================================================================

Due to popular demand, here is a simple multi-channel audio mixer.
It supports 8 channels of 16 bit stereo audio, plus a single channel
of music, mixed by the Modplug MOD, Timidity MIDI, ADLMIDI, GME 
and LibMAD MP3 libraries.

See the header file SDL_mixer_ext.h for documentation on this mixer library.

The mixer can currently load Microsoft WAVE files and Creative Labs VOC
files as audio samples, and can load MIDI files via Timidity and the
following music formats via MikMod:  .MOD .S3M .IT .XM. It can load
Ogg Vorbis streams as music if built with Ogg Vorbis or Tremor libraries,
and finally it can load MP3 music using the SMPEG or libmad libraries.

Tremor decoding is disabled by default; you can enable it by passing
	--enable-music-ogg-tremor
to configure, or by defining OGG_MUSIC and OGG_USE_TREMOR.

libmad decoding is disabled by default; you can enable it by passing
	--enable-music-mp3-mad
to configure, or by defining MP3_MAD_MUSIC
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
WARNING: The license for libmad is GPL, which means that in order to
         use it your application must also be GPL!
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The process of mixing MIDI files to wave output is very CPU intensive,
so if playing regular WAVE files sound great, but playing MIDI files
sound choppy on slow computers, try using 8-bit audio, mono audio,
or lower frequencies.

To play MIDI files via Timidity, you'll need to get a complete set of GUS patches
from:
http://www.libsdl.org/projects/mixer/timidity/timidity.tar.gz
and unpack them in /usr/local/lib under UNIX, and C:\ under Win32.

This library is under the zlib license, see the file "COPYING.txt" for details.



