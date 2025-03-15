/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#ifndef IT_H
#define IT_H

#define NUM_PATT_BUFS 128

#include "spc2ittypes.h"

s32 ITStart(s32); // Opens temp file, inits writing
s32 ITUpdate(); // Dumps pattern buffers to file
s32 ITWrite(const char *fn); // Stops recording and writes IT file from temp data
void ITCleanup(); // clears IT temp data
int ITMix();

// Macros

#define FINE_SLIDE 0xF0
#define EXTRA_FINE_SLIDE 0xE0
#define EFFECT_X 24
#define EFFECT_F 6
#define EFFECT_E 5

#define IT_PATTERN_MAX 0xFD // The original Impulse Tracker has 200 patterns max
#define IT_SAMPLE_MAX 0xFF // The original Impulse Tracker has 99 samples max

#define IT_MASK_NOTE 1 // 0001 (Note)
#define IT_MASK_SAMPLE 2 // 0010 (Sample/instrument marker)
#define IT_MASK_ADJUSTVOLUME 4 // 0100 (volume/panning)
#define IT_MASK_NOTE_SAMPLE_ADJUSTVOLUME (IT_MASK_NOTE | IT_MASK_SAMPLE | IT_MASK_ADJUSTVOLUME)
#define IT_MASK_PITCHSLIDE 8 // 1000 (some special command, we use effect F and effect E)
#define IT_MASK_ADJUSTPAN 16 // 10000 change the note panning

#endif
