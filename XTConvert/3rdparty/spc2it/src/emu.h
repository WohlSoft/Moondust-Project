/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#ifndef EMU_H
#define EMU_H
#include "spc2ittypes.h"
#include "sneese_spc.h"
extern u8 SPC_DSP[256];
extern u8 SPCRAM[65536];
#define TotalCycles (active_context->TotalCycles)
extern u32 SPC_DSP_DATA;

extern s32 SPCtime;
extern SPCFileInformation SPCInfo;
extern void (*SPC_Write_DSP_Hook)(u8);

#define SPCUpdateRate 100

s32 SPCInit(const char *);
void SPCAddWriteDSPCallback(void (*ToAddCallback)(u8));

#endif
