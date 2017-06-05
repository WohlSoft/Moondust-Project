/*
    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.
 */

#include "../audio_codec.h"

extern int Timidity_init2(AudioCodec *codec, SDL_AudioSpec *mixer);

typedef struct _MidiSong MidiSong;

extern int Timidity_Init(int rate, int format, int channels, int samples);
extern void Timidity_addToPathList(const char* path);
extern const char *Timidity_Error(void);
extern void Timidity_SetVolume(void *song_p, int volume);
extern int Timidity_PlaySome(void *stream, int samples);
extern void *Timidity_LoadSong_RW(SDL_RWops *rw, int freerw);
extern void Timidity_Start(void *song_p);
extern int Timidity_Active(void *song_p);
extern void Timidity_Stop(void *song_p);
extern void Timidity_FreeSong(MidiSong *song);
extern void Timidity_Close(void);
