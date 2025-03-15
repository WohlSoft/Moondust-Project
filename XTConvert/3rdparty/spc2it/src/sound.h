/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#ifndef SOUND_H
#define SOUND_H

#include "spc2ittypes.h"

#define ATTACK 0 // A of ADSR
#define DECAY 1 // D of ADSR
#define SUSTAIN 2 // S of ADSR
#define RELEASE 3 // R of ADSR
#define DECREASE 4 // GAIN linear decrease mode
#define EXP 5 // GAIN exponential decrease mode
#define INCREASE 6 // GAIN linear increase mode
#define BENT 7 // GAIN bent line increase mode
#define DIRECT 8 // Directly specify ENVX

extern sndvoice SNDvoices[8];
extern s32 SNDkeys, SNDratecnt;

s32 SNDDoEnv(s32);
void SNDNoteOn(u8);
void SNDNoteOff(u8);
s32 SNDInit();

#endif
