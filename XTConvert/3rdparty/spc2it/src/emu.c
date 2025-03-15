/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sound.h"
#include "spc2ittypes.h"
#include "sneese_spc.h"

u8 SPC_DSP[256];
u8 SPCRAM[65536];
u32 Map_Address;
u32 SPC_DSP_DATA;
u32 Map_Byte;
u8 In_CPU;
u32 SPC_CPU_cycle_divisor;
u32 SPC_CPU_cycle_multiplicand;
u32 SPC_CPU_cycles;
u32 SPC_CPU_cycles_mul;
u32 sound_cycle_latch;

void (*SPC_Write_DSP_Hook)(u8); // function pointer

// ID tag stuff
s32 SPCtime;
SPCFileInformation SPCInfo;

static s32 LoadZState(const char *fn)
{
	SPCFile *sFile = (SPCFile *)calloc(1, sizeof(SPCFile));
	if (sFile == NULL)
	{
		// printf("Error: could not allocate memory for SPCFile struct\n");
		return(1);
	}

	FILE *f = fopen(fn, "rb");
	if (f == NULL)
	{
		// printf("Error: can't open file\n");
		return(1);
	}
	fseek(f, 0, SEEK_SET);
	fread(sFile, sizeof(SPCFile), 1, f);
	fclose(f);

	if (strncmp("SNES-SPC700 Sound File Data", sFile->FileTag, 27))
	{
		// printf("Error: invalid file format\n");
		return(1);
	}

	memcpy(&active_context->PC.w, sFile->Registers.PC, 2);
	active_context->YA.b.l = sFile->Registers.A;
	active_context->X = sFile->Registers.X;
	active_context->YA.b.h = sFile->Registers.Y;
	active_context->SP = 0x100 + sFile->Registers.SP;
	active_context->PSW = sFile->Registers.PSW;
	memcpy(SPCRAM, sFile->RAM, 65536);
	memcpy(SPC_DSP, sFile->DSPBuffer, 128);

	memcpy(&SPCInfo, &sFile->Information, sizeof(SPCFileInformation));
	char songLen[4];
	strncpy(songLen, SPCInfo.SongLength, 3);

	if (songLen[0] >= 0)
		SPCtime = atoi(songLen);
	else
		SPCtime = 0;

	if (0 == (SPC_CTRL & 0x80))
		active_context->FFC0_Address = SPCRAM;

	active_context->timers[0].target = (u8)(SPCRAM[0xFA] - 1) + 1;
	active_context->timers[1].target = (u8)(SPCRAM[0xFB] - 1) + 1;
	active_context->timers[2].target = (u8)(SPCRAM[0xFC] - 1) + 1;
	active_context->timers[0].counter = SPCRAM[0xFD] & 0xF;
	active_context->timers[1].counter = SPCRAM[0xFE] & 0xF;
	active_context->timers[2].counter = SPCRAM[0xFF] & 0xF;
	active_context->PORT_R[0] = SPCRAM[0xF4];
	active_context->PORT_R[1] = SPCRAM[0xF5];
	active_context->PORT_R[2] = SPCRAM[0xF6];
	active_context->PORT_R[3] = SPCRAM[0xF7];

	SPC_CPU_cycle_multiplicand = 1;
	SPC_CPU_cycle_divisor = 1;
	SPC_CPU_cycles_mul = 0;
	spc_restore_flags();
	free(sFile);
	return (0);
}

// PUBLIC (non-static) functions

s32 SPCInit(const char *fn)
{
	Reset_SPC();
	if (LoadZState(fn))
		return 1;
	return 0;
}

void SPCAddWriteDSPCallback(void (*ToAddCallback)(u8))
{
	SPC_Write_DSP_Hook = ToAddCallback;
}

// Called from SPC 700 engine

void DisplaySPC()
{
}

/*
void InvalidSPCOpcode()
{
	exit(-1);
}
*/

void SPC_READ_DSP()
{
	if ((SPC_DSP_ADDR & 0xf) == 8) // ENVX
		SPC_DSP[SPC_DSP_ADDR] = SNDDoEnv(SPC_DSP_ADDR >> 4) >> 24;
}

void SPC_WRITE_DSP()
{	
	s32 addr_lo = SPC_DSP_ADDR & 0xF, addr_hi = SPC_DSP_ADDR >> 4;
	switch (addr_lo)
	{
	case 3: // Pitch hi
		SPC_DSP_DATA &= 0x3F;
		break;
	case 5: // ADSR1
		if ((SPC_DSP[0x4C] & (1 << addr_hi)) && ((SPC_DSP_DATA & 0x80) != (SPC_DSP[SPC_DSP_ADDR] & 0x80)))
		{
			s32 i;
			// First of all, in case anything was already
			// going on, finish it up
			SNDDoEnv(addr_hi);
			if (SPC_DSP_DATA & 0x80)
			{
				// switch to ADSR--not sure what to do
				i = SPC_DSP[(addr_hi << 4) + 6];
				SNDvoices[addr_hi].envstate = ATTACK;
				SNDvoices[addr_hi].ar = SPC_DSP_DATA & 0xF;
				SNDvoices[addr_hi].dr = SPC_DSP_DATA >> 4 & 7;
				SNDvoices[addr_hi].sr = i & 0x1f;
				SNDvoices[addr_hi].sl = i >> 5;
			}
			else
			{
				// switch to a GAIN mode
				i = SPC_DSP[(addr_hi << 4) + 7];
				if (i & 0x80)
				{
					SNDvoices[addr_hi].envstate = i >> 5;
					SNDvoices[addr_hi].gn = i & 0x1F;
				}
				else
				{
					SNDvoices[addr_hi].envx = (i & 0x7F) << 24;
					SNDvoices[addr_hi].envstate = DIRECT;
				}
			}
		}
		break;
	case 6: // ADSR2
		// Finish up what was going on
		SNDDoEnv(addr_hi);
		SNDvoices[addr_hi].sr = SPC_DSP_DATA & 0x1f;
		SNDvoices[addr_hi].sl = SPC_DSP_DATA >> 5;
		break;
	case 7: // GAIN
		if ((SPC_DSP[0x4C] & (1 << addr_hi)) && (SPC_DSP_DATA != SPC_DSP[SPC_DSP_ADDR]) &&
		    !(SPC_DSP[(addr_hi << 4) + 5] & 0x80))
		{
			if (SPC_DSP_DATA & 0x80)
			{
				// Finish up what was going on
				SNDDoEnv(addr_hi);
				SNDvoices[addr_hi].envstate = SPC_DSP_DATA >> 5;
				SNDvoices[addr_hi].gn = SPC_DSP_DATA & 0x1F;
			}
			else
			{
				SNDvoices[addr_hi].envx = (SPC_DSP_DATA & 0x7F) << 24;
				SNDvoices[addr_hi].envstate = DIRECT;
			}
		}
		break;
	// These are general registers
	case 12: // 0xc
		switch (addr_hi)
		{
		case 4: // Key on
			SNDNoteOn(SPC_DSP_DATA);
			SPC_DSP_DATA = SPC_DSP[0x4C];
			break;
		case 5: // Key off
			SNDNoteOff(SPC_DSP_DATA);
			SPC_DSP_DATA = 0;
			break;
		}
		break;
	}

	SPC_DSP[SPC_DSP_ADDR] = SPC_DSP_DATA;
}
