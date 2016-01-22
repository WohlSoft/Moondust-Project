Attribute VB_Name = "SDL2_mixer_ext_vb6"
Option Explicit

Const SDL_AUDIO_DRIVER_DISK As Long = 1
Const SDL_AUDIO_DRIVER_DUMMY As Long = 1
Const SDL_AUDIO_DRIVER_DSOUND As Long = 1
Const SDL_AUDIO_DRIVER_WINMM As Long = 1

Const AUDIO_U8 As Long = &H8
Const AUDIO_S8 As Long = &H8008
Const AUDIO_U16LSB As Long = &H10
Const AUDIO_S16LSB As Long = &H8010
Const AUDIO_U16MSB As Long = &H1010
Const AUDIO_S16MSB As Long = &H9010
Const AUDIO_S32LSB As Long = &H8020
Const AUDIO_S32MSB As Long = &H9020
Const AUDIO_F32LSB As Long = &H8120
Const AUDIO_F32MSB As Long = &H9120

Const SDL_AUDIO_ALLOW_FREQUENCY_CHANGE As Long = &H1
Const SDL_AUDIO_ALLOW_FORMAT_CHANGE As Long = &H2
Const SDL_AUDIO_ALLOW_CHANNELS_CHANGE As Long = &H4

Const SDL_MIX_MAXVOLUME As Long = 128
Const SDL_MAJOR_VERSION As Long = 2
Const SDL_MINOR_VERSION As Long = 0
Const SDL_PATCHLEVEL As Long = 2
Const SDL_MIXER_MAJOR_VERSION As Long = 2
Const SDL_MIXER_MINOR_VERSION As Long = 0
Const SDL_MIXER_PATCHLEVEL As Long = 0
Const MIX_CHANNELS As Long = 8
Const MIX_DEFAULT_FREQUENCY As Long = 22050
Const MIX_DEFAULT_CHANNELS As Long = 2
Const MIX_MAX_VOLUME As Long = 128
Const MIX_EFFECTSMAXSPEED As String = "MIX_EFFECTSMAXSPEED"

Const RW_SEEK_SET As Long = 0 '/**< Seek from the beginning of data */
Const RW_SEEK_CUR As Long = 1 '/**< Seek relative to current read point */
Const RW_SEEK_END As Long = 2 '/**< Seek relative to the end of data */

Public Type SDL_AudioSpec
        freq As Long
        format As Long
        channels As Long
        silence As Long
        samples As Long
        padding As Long
        size As Long
        callback As Long
        userdata As Long
End Type

Public Type Mix_Chunk
        allocated As Long
        abuf As Long
        alen As Long
        Volume As Long
End Type

Public Enum Mix_Fading
    MIX_NO_FADING = 0
    MIX_FADING_OUT
    MIX_FADING_IN
End Enum

Public Enum Mix_MusicType
    MUS_NONE = 0
    MUS_CMD
    MUS_WAV
    MUS_MOD
    MUS_MID
    MUS_OGG
    MUS_MP3
    MUS_MP3_MAD
    MUS_FLAC
    MUS_MODPLUG
    MUS_SPC
End Enum

Public Enum Mix_MIDI_Device
    MIDI_ADLMIDI = 0
    MIDI_Native
    MIDI_Timidity
    MIDI_Fluidsynth
End Enum

'int Mix_Init(int flags);
'Loads dynamic libraries and prepares them for use.  Flags should be
'one or more flags from MIX_InitFlags OR'd together.
'It returns the flags successfully initialized, or 0 on failure.
Public Declare Function Mix_Init Lib "SDL2_mixer_ext_vb6.dll" Alias "Mix_InitVB6" () As Long
'Public Declare Function Mix_Init Lib "SDL2_mixer_ext_vb6.dll" (ByVal flags As Long) As Long

''''''''''''''''''''''''''''SDL Functions'''''''''''''''''''''''''''''''''''''''''
'This function calls SDL_Init(SDL_INIT_AUDIO)
Public Declare Function SDL_InitAudio Lib "SDL2_mixer_ext_vb6.dll" () As Long
Public Declare Sub SDL_Quit Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_QuitVB6" ()
Public Declare Function SDL_GetError Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_GetErrorVB6" () As String
''''''''''''''''''''''''''''SDL Functions'''END'''''''''''''''''''''''''''''''''''

'void Mix_Quit(void);
'Unloads libraries loaded with Mix_Init
Public Declare Sub Mix_Quit Lib "SDL2_mixer_ext_vb6.dll" ()

'Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
'Open the mixer with a certain audio format
Public Declare Function Mix_OpenAudio Lib "SDL2_mixer_ext_vb6.dll" _
                Alias "Mix_OpenAudioVB6" _
                (ByVal frequency As Long, _
                 ByVal format As Long, _
                 ByVal channels As Long, _
                 ByVal chunksize As Long) As Integer
'Public Declare Function Mix_OpenAudio Lib "SDL2_mixer_ext_vb6.dll" (ByVal frequency As Long, ByVal format As Integer, ByVal channels As Long, ByVal chunksize As Long) As Long

'extern DECLSPEC int SDLCALL Mix_AllocateChannels(int numchans);
'Dynamically change the number of channels managed by the mixer.
'If decreasing the number of channels, the upper channels are stopped.
'This function returns the new number of allocated channels.
Public Declare Function Mix_AllocateChannels Lib "SDL2_mixer_ext_vb6.dll" (ByVal numchans As Long) As Long

'Mix_Music * Mix_LoadMUS(const char *file);
'Load a wave file or a music (.mod .s3m .it .xm) file
'List of all supported musics are here: http://engine.wohlnet.ru/pgewiki/SDL2_mixer
Public Declare Function Mix_LoadMUS Lib "SDL2_mixer_ext_vb6.dll" (ByVal file As String) As Long

'Mix_Chunk * Mix_LoadWAV(const char* file);
Public Declare Function Mix_LoadWAV Lib "SDL2_mixer_ext_vb6.dll" Alias "Mix_LoadWAV_VB6" (ByVal file As String) As Long

'Mix_Chunk * Mix_LoadWAV_RW(SDL_RWops *src, int freesrc);
'       rwops - an SDL_rwops structure pointer (in VB is a Long value!)
'       freesrc - close and destroy SDL_rwpos structure (you no need free it after!)
Public Declare Function Mix_LoadWAV_RW Lib "SDL2_mixer_ext_vb6.dll" (ByVal rwops As Long, ByVal freesrc As Long) As Long

'/* Load a music file from an SDL_RWop object (Ogg and MikMod specific currently)
' Matt Campbell (matt@campbellhome.dhs.org) April 2000 */
'extern DECLSPEC Mix_Music * SDLCALL Mix_LoadMUS_RW(SDL_RWops *src, int freesrc);
'       rwops - an SDL_rwops structure pointer (in VB is a Long value!)
'       freesrc - close and destroy SDL_rwpos structure (you no need free it after!)
Public Declare Function Mix_LoadMUS_RW Lib "SDL2_mixer_ext_vb6.dll" _
                        (ByVal rwops As Long, ByVal freesrc As Long) As Long

'/* Load a music file from an SDL_RWop object assuming a specific format */
'extern DECLSPEC Mix_Music * SDLCALL Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc);
Public Declare Function Mix_LoadMUSType_RW Lib "SDL2_mixer_ext_vb6.dll" _
                        (ByVal rwops As Long, ByVal mustype As Mix_MusicType, ByVal freesrc As Long) As Long

'/* Load a wave file of the mixer format from a memory buffer */
'extern DECLSPEC Mix_Chunk * SDLCALL Mix_QuickLoad_WAV(Uint8 *mem);

'/* Load raw audio data of the mixer format from a memory buffer */
'extern DECLSPEC Mix_Chunk * SDLCALL Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len);


'Free an audio chunk previously loaded
'void Mix_FreeChunk(Mix_Chunk *chunk);
Public Declare Sub Mix_FreeChunk Lib "SDL2_mixer_ext_vb6.dll" (ByVal chunk As Long)
'void Mix_FreeMusic(Mix_Music *music);
Public Declare Sub Mix_FreeMusic Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long)


'Get a list of chunk/music decoders that this build of SDL_mixer provides.
'This list can change between builds AND runs of the program, if external
'libraries that add functionality become available.
'You must successfully call Mix_OpenAudio() before calling these functions.
'This API is only available in SDL_mixer 1.2.9 and later.
'
'// usage...
'int i;
'const int total = Mix_GetNumChunkDecoders();
'for (i = 0; i < total; i++)
'    printf("Supported chunk decoder: [%s]\n", Mix_GetChunkDecoder(i));
'
'Appearing in this list doesn't promise your specific audio file will
'decode...but it's handy to know if you have, say, a functioning Timidity
'install.
'
'These return values are static, read-only data; do not modify or free it.
'The pointers remain valid until you call Mix_CloseAudio().

'int Mix_GetNumChunkDecoders(void);
Public Declare Function Mix_GetNumChunkDecoders Lib "SDL2_mixer_ext_vb6.dll" () As Long
'const char * Mix_GetChunkDecoder(int index);
Public Declare Function Mix_GetChunkDecoder Lib "SDL2_mixer_ext_vb6.dll" (ByVal index As Long) As String
'extern DECLSPEC int SDLCALL Mix_GetNumMusicDecoders(void);
Public Declare Function Mix_GetNumMusicDecoders Lib "SDL2_mixer_ext_vb6.dll" () As Long
'extern DECLSPEC const char * SDLCALL Mix_GetMusicDecoder(int index);
Public Declare Function Mix_GetMusicDecoder Lib "SDL2_mixer_ext_vb6.dll" (ByVal index As Long) As String

'Mix_MusicType Mix_GetMusicType(const Mix_Music *music);
'Find out the music format of a mixer music, or the currently playing
'music, if 'music' is NULL.
Public Declare Function Mix_GetMusicType Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As Long

'/* Get music title from meta-tag if possible. If title tag is empty, filename will be returned */
'const char* Mix_GetMusicTitle(const Mix_Music *music);
Public Declare Function Mix_GetMusicTitle Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As String

'/* Get music title from meta-tag if possible */
'const char* Mix_GetMusicTitleTag(const Mix_Music *music);
Public Declare Function Mix_GetMusicTitleTag Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As String

'/* Get music artist from meta-tag if possible */
'const char* Mix_GetMusicArtistTag(const Mix_Music *music);
Public Declare Function Mix_GetMusicArtistTag Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As String

'/* Get music album from meta-tag if possible */
'const char* Mix_GetMusicAlbumTag(const Mix_Music *music);
Public Declare Function Mix_GetMusicAlbumTag Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As String

'/* Get music copyright from meta-tag if possible */
'const char* Mix_GetMusicCopyrightTag(const Mix_Music *music);
Public Declare Function Mix_GetMusicCopyrightTag Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long) As String


'/* Set the panning of a channel. The left and right channels are specified
'int Mix_SetPanning(int channel, Uint8 left, Uint8 right);
Public Declare Function Mix_SetPanning Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal left As Integer, ByVal right As Integer) As Long

'/* Set the position of a channel. (angle) is an integer from 0 to 360
'int Mix_SetPosition(int channel, Sint16 angle, Uint8 distance);
Public Declare Function Mix_SetPosition Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal angle As Integer, ByVal distance As Integer) As Long

'/* Set the "distance" of a channel. (distance) is an integer from 0 to 255
'int Mix_SetDistance(int channel, Uint8 distance);
Public Declare Function Mix_SetDistance Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal distance As Integer) As Long

'/* Causes a channel to reverse its stereo.
'int Mix_SetReverseStereo(int channel, int flip);
Public Declare Function Mix_SetReverseStereo Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal flip As Long) As Long



'/* Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
'   them dynamically to the next sample if requested with a -1 value below.
'   Returns the number of reserved channels.
' */
'int Mix_ReserveChannels(int num);
Public Declare Function Mix_ReserveChannels Lib "SDL2_mixer_ext_vb6.dll" (ByVal num As Long) As Long

'/* Attach a tag to a channel. A tag can be assigned to several mixer
'   channels, to form groups of channels.
'   If 'tag' is -1, the tag is removed (actually -1 is the tag used to
'   represent the group of all the channels).
'   Returns true if everything was OK.
' */
'int Mix_GroupChannel(int which, int tag);
Public Declare Function Mix_GroupChannel Lib "SDL2_mixer_ext_vb6.dll" (ByVal which As Long, ByVal tag As Long) As Long

'/* Assign several consecutive channels to a group */
'int Mix_GroupChannels(int from, int to, int tag);
Public Declare Function Mix_GroupChannels Lib "SDL2_mixer_ext_vb6.dll" (ByVal groupfrom As Long, ByVal groupto As Long, ByVal tag As Long) As Long

'/* Finds the first available channel in a group of channels,
'   returning -1 if none are available.
' */
'extern DECLSPEC int SDLCALL Mix_GroupAvailable(int tag);
Public Declare Function Mix_GroupAvailable Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long) As Long

'/* Returns the number of channels in a group. This is also a subtle
'   way to get the total number of channels when 'tag' is -1
' */
'int Mix_GroupCount(int tag);
Public Declare Function Mix_GroupCount Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long) As Long

'/* Finds the "oldest" sample playing in a group of channels */
'int Mix_GroupOldest(int tag);
Public Declare Function Mix_GroupOldest Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long) As Long

'/* Finds the "most recent" (i.e. last) sample playing in a group of channels */
'int Mix_GroupNewer(int tag);
Public Declare Function Mix_GroupNewer Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long) As Long

'/* Play an audio chunk on a specific channel.
'   If the specified channel is -1, play on the first free channel.
'   If 'loops' is greater than zero, loop the sound that many times.
'   If 'loops' is -1, loop inifinitely (~65000 times).
'   Returns which channel was used to play the sound.
'*/
'#define Mix_PlayChannel(channel,chunk,loops) Mix_PlayChannelTimed(channel,chunk,loops,-1)
'int Mix_PlayChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
Public Declare Function Mix_PlayChannel Lib "SDL2_mixer_ext_vb6.dll" Alias "Mix_PlayChannelTimed" (ByVal channel As Long, ByVal chunk As Long, ByVal loops As Long, Optional ByVal ticks As Long = -1) As Long
Public Declare Function Mix_PlayChannelTimed Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal chunk As Long, ByVal loops As Long, ByVal ticks As Long) As Long

'int Mix_PlayMusic(Mix_Music *music, int loops);
Public Declare Function Mix_PlayMusic Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long, ByVal loops As Long) As Long

'/* Fade in music or a channel over "ms" milliseconds, same semantics as the "Play" functions */
'int Mix_FadeInMusic(Mix_Music *music, int loops, int ms);
Public Declare Function Mix_FadeInMusic Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long, ByVal loops As Long, ByVal milliseconds As Long) As Long
'int Mix_FadeInMusicPos(Mix_Music *music, int loops, int ms, double position);
Public Declare Function Mix_FadeInMusicPos Lib "SDL2_mixer_ext_vb6.dll" (ByVal music As Long, ByVal loops As Long, ByVal milliseconds As Long, ByVal position As Double) As Long

'#define Mix_FadeInChannel(channel,chunk,loops,ms) Mix_FadeInChannelTimed(channel,chunk,loops,ms,-1)
Public Declare Function Mix_FadeInChannel Lib "SDL2_mixer_ext_vb6.dll" Alias "Mix_FadeInChannelTimed" (ByVal channel As Long, ByVal chunk As Long, ByVal loops As Long, ByVal milliseconds As Long, Optional ByVal ticks As Long = -1) As Long
'extern DECLSPEC int SDLCALL Mix_FadeInChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks);
Public Declare Function Mix_FadeInChannelTimed Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal chunk As Long, ByVal loops As Long, ByVal milliseconds As Long, ByVal ticks As Long) As Long

'/* Set the volume in the range of 0-128 of a specific channel or chunk.
'   If the specified channel is -1, set volume for all channels.
'   Returns the original volume.
'   If the specified volume is -1, just return the current volume.
'*/
'int Mix_Volume(int channel, int volume);
Public Declare Function Mix_Volume Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal Volume As Long) As Long

'int Mix_VolumeChunk(Mix_Chunk *chunk, int volume);
Public Declare Function Mix_VolumeChunk Lib "SDL2_mixer_ext_vb6.dll" (ByVal chunk As Long, ByVal Volume As Long) As Long

'int Mix_VolumeMusic(int volume);
Public Declare Function Mix_VolumeMusic Lib "SDL2_mixer_ext_vb6.dll" (ByVal Volume As Long) As Long


'/* Halt playing of a particular channel */
'extern DECLSPEC int SDLCALL Mix_HaltChannel(int channel);
Public Declare Function Mix_HaltChannel Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long) As Long

'extern DECLSPEC int SDLCALL Mix_HaltGroup(int tag);
Public Declare Function Mix_HaltGroup Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long) As Long

'extern DECLSPEC int SDLCALL Mix_HaltMusic(void);
Public Declare Function Mix_HaltMusic Lib "SDL2_mixer_ext_vb6.dll" () As Long

'/* Change the expiration delay for a particular channel.
'   The sample will stop playing after the 'ticks' milliseconds have elapsed,
'   or remove the expiration if 'ticks' is -1
'*/
'int Mix_ExpireChannel(int channel, int ticks);
Public Declare Function Mix_ExpireChannel Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long, ByVal ticks As Long) As Long

'/* Halt a channel, fading it out progressively till it's silent
'   The ms parameter indicates the number of milliseconds the fading
'   will take.
' */
'int Mix_FadeOutChannel(int which, int ms);
Public Declare Function Mix_FadeOutChannel Lib "SDL2_mixer_ext_vb6.dll" (ByVal which As Long, ByVal milliseconds As Long) As Long

'int Mix_FadeOutGroup(int tag, int ms);
Public Declare Function Mix_FadeOutGroup Lib "SDL2_mixer_ext_vb6.dll" (ByVal tag As Long, ByVal milliseconds As Long) As Long

'int Mix_FadeOutMusic(int ms);
Public Declare Function Mix_FadeOutMusic Lib "SDL2_mixer_ext_vb6.dll" (ByVal milliseconds As Long) As Long

'/* Query the fading status of a channel */
'Mix_Fading Mix_FadingMusic(void); //Mix_Fading - enum!
Public Declare Function Mix_FadingMusic Lib "SDL2_mixer_ext_vb6.dll" () As Long

'Mix_Fading Mix_FadingChannel(int which); //Mix_Fading - enum!
Public Declare Function Mix_FadingChannel Lib "SDL2_mixer_ext_vb6.dll" (ByVal which As Long) As Long


'/* Pause/Resume a particular channel */
'void Mix_Pause(int channel);
Public Declare Sub Mix_Pause Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long)

'void Mix_Resume(int channel);
Public Declare Sub Mix_Resume Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long)

'int Mix_Paused(int channel);
Public Declare Function Mix_Paused Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long) As Long


'/* Pause/Resume the music stream */
'void Mix_PauseMusic(void);
Public Declare Sub Mix_PauseMusic Lib "SDL2_mixer_ext_vb6.dll" ()

'void Mix_ResumeMusic(void);
Public Declare Sub Mix_ResumeMusic Lib "SDL2_mixer_ext_vb6.dll" ()

'void Mix_RewindMusic(void);
Public Declare Sub Mix_RewindMusic Lib "SDL2_mixer_ext_vb6.dll" ()

'int  Mix_PausedMusic(void);
Public Declare Function Mix_PausedMusic Lib "SDL2_mixer_ext_vb6.dll" () As Long


'/* Set the current position in the music stream.
'   This returns 0 if successful, or -1 if it failed or isn't implemented.
'   This function is only implemented for MOD music formats (set pattern
'   order number) and for OGG, FLAC, MP3_MAD, and MODPLUG music (set
'   position in seconds), at the moment.
'*/
'int Mix_SetMusicPosition(double position);
Public Declare Function Mix_SetMusicPosition Lib "SDL2_mixer_ext_vb6.dll" (ByVal position As Double) As Long


'/* Check the status of a specific channel.
'   If the specified channel is -1, check all channels.
'*/
'int Mix_Playing(int channel);
Public Declare Function Mix_Playing Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long) As Long

'int Mix_PlayingMusic(void);
Public Declare Function Mix_PlayingMusic Lib "SDL2_mixer_ext_vb6.dll" () As Long

'/* Get the Mix_Chunk currently associated with a mixer channel
'    Returns NULL if it's an invalid channel, or there's no chunk associated.'
'*/
'Mix_Chunk * Mix_GetChunk(int channel);
Public Declare Function Mix_GetChunk Lib "SDL2_mixer_ext_vb6.dll" (ByVal channel As Long) As Long


'extern DECLSPEC void SDLCALL Mix_CloseAudio(void);
Public Declare Sub Mix_CloseAudio Lib "SDL2_mixer_ext_vb6.dll" ()

'/* Add additional Timidity bank path */
'void MIX_Timidity_addToPathList(const char *path);
Public Declare Sub MIX_Timidity_addToPathList Lib "SDL2_mixer_ext_vb6.dll" (ByVal path As String)


'/* ADLMIDI Setup functions */
'int  MIX_ADLMIDI_getBankID();
Public Declare Function MIX_ADLMIDI_getBankID Lib "SDL2_mixer_ext_vb6.dll" () As Long
'void MIX_ADLMIDI_setBankID(int bnk);
Public Declare Function MIX_ADLMIDI_setBankID Lib "SDL2_mixer_ext_vb6.dll" (ByVal bankID As Long) As Long

'int  MIX_ADLMIDI_getTremolo();
Public Declare Function MIX_ADLMIDI_getTremolo Lib "SDL2_mixer_ext_vb6.dll" () As Long
'void MIX_ADLMIDI_setTremolo(int tr);
Public Declare Function MIX_ADLMIDI_setTremolo Lib "SDL2_mixer_ext_vb6.dll" (ByVal tr As Long) As Long

'int  MIX_ADLMIDI_getVibrato();
Public Declare Function MIX_ADLMIDI_getVibrato Lib "SDL2_mixer_ext_vb6.dll" () As Long
'void MIX_ADLMIDI_setVibrato(int vib);
Public Declare Function MIX_ADLMIDI_setVibrato Lib "SDL2_mixer_ext_vb6.dll" (ByVal vib As Long) As Long

'int  MIX_ADLMIDI_getScaleMod();
Public Declare Function MIX_ADLMIDI_getScaleMod Lib "SDL2_mixer_ext_vb6.dll" () As Long
'void MIX_ADLMIDI_setScaleMod(int sc);
Public Declare Function MIX_ADLMIDI_setScaleMod Lib "SDL2_mixer_ext_vb6.dll" (ByVal sc As Long) As Long

'int MIX_SetMidiDevice(int device);
' Allows you to toggle MIDI Device (change applying only on reopening of MIDI file)
Public Declare Function MIX_SetMidiDevice Lib "SDL2_mixer_ext_vb6.dll" (ByVal MIDIDevice As Mix_MIDI_Device) As Long

'/*****************SDL RWops AIP*****************/

'SDL_RWops * SDL_RWFromFileVB6(const char *file, const char *mode)
Public Declare Function SDL_RWFromFile Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWFromFileVB6" _
                    (ByVal filename As String, ByVal mode As String) As Long

'SDL_RWops * SDL_RWFromMemVB6(void *mem, int size)
Public Declare Function SDL_RWFromMem Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWFromMemVB6" _
                    (ByRef mem As Any, ByVal size As Long) As Long
                    
'SDL_RWops * SDL_AllocRWVB6(void)
Public Declare Function SDL_AllocRW Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_AllocRWVB6" () As Long

'void SDL_FreeRWVB6(SDL_RWops * area)
Public Declare Sub SDL_FreeRW Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_FreeRWVB6" _
                    (ByVal rwops As Long)

'int SDL_RWsizeVB6(SDL_RWops * ctx)
Public Declare Function SDL_RWsize Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWseekVB6" _
                    (ByVal rwops As Long) As Long
                    
'int SDL_RWseekVB6(SDL_RWops * ctx, int offset, int whence)
Public Declare Function SDL_RWseek Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWseekVB6" _
                    (ByVal rwops As Long, ByVal offset As Long, ByVal whence As Long) As Long
                    
'int SDL_RWtellVB6(SDL_RWops * ctx)
Public Declare Function SDL_RWtell Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWtellVB6" _
                    (ByVal rwops As Long) As Long

'int SDL_RWreadVB6(SDL_RWops * ctx, void*ptr, int size, int maxnum)
Public Declare Function SDL_RWread Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWreadVB6" _
                    (ByVal rwops As Long, ByRef ptr As Any, ByVal size As Long, ByVal max As Long) As Long

'int SDL_RWwriteVB6(SDL_RWops * ctx, void* ptr, int size, int maxnum)
Public Declare Function SDL_RWwrite Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWwriteVB6" _
                    (ByVal rwops As Long, ByRef ptr As Any, ByVal size As Long, ByVal max As Long) As Long

'int SDL_RWcloseVB6(SDL_RWops * ctx)
Public Declare Function SDL_RWclose Lib "SDL2_mixer_ext_vb6.dll" Alias "SDL_RWcloseVB6" _
                    (ByVal rwops As Long) As Long





