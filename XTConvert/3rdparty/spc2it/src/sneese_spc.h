/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

/**************************************************************************

        Copyright (c) 2005 Brad Martin.
        Some portions copyright (c) 1998-2005 Charles Bilyue'.

This file is part of OpenSPC.

sneese_spc.h: This file defines the interface between the SNEeSe SPC700
core and the associated wrapper files.  As the licensing rights for SNEeSe
are different from the rest of OpenSPC, none of the files in this directory
are LGPL.  Although this file was created by me (Brad Martin), it contains
some code derived from SNEeSe and therefore falls under its license.  See
the file './doc/LICENSE_SNEESE' in this directory for more information.

 **************************************************************************/

#if !defined(_SNEESE_SPC_H)
#define _SNEESE_SPC_H

#include "spc2ittypes.h"

/*========== DEFINES ==========*/

#define SPC_CTRL (SPCRAM[0xF1])
#define SPC_DSP_ADDR (SPCRAM[0xF2])

/*========== TYPES ==========*/

typedef union
{
	u16 w;
	struct
	{
		u8 l;
		u8 h;
	} b;
} word_2b;

typedef struct
{
	u8 B_flag;
	u8 C_flag;
	u8 H_flag;
	u8 I_flag;
	u8 N_flag;
	u8 P_flag;
	u8 PSW;
	u8 SP;
	u8 V_flag;
	u8 X;
	u8 Z_flag;
	u8 cycle;
	u8 data;
	u8 data2;
	u8 opcode;
	u8 offset;

	word_2b PC;
	word_2b YA;
	word_2b address;
	word_2b address2;
	word_2b data16;
	word_2b direct_page;

	u32 Cycles;
	void *FFC0_Address;
	u32 TotalCycles;
	s32 WorkCycles;
	u32 last_cycles;

	u8 PORT_R[4];
	u8 PORT_W[4];
	struct
	{
		u8 counter;
		s16 position;
		s16 target;
		u32 cycle_latch;
	} timers[4];
} SPC700_CONTEXT;

/*========== VARIABLES ==========*/

/* SPCimpl.c variables */
extern u8 In_CPU;
extern u32 Map_Address;
extern u32 Map_Byte;
extern u32 SPC_CPU_cycle_divisor;
extern u32 SPC_CPU_cycle_multiplicand;
extern u32 SPC_CPU_cycles;
extern u32 SPC_CPU_cycles_mul;
extern u8 SPC_DSP[256];
extern u32 SPC_DSP_DATA;
extern u8 SPCRAM[65536];
extern u32 sound_cycle_latch;

/* spc700.c variables */
extern SPC700_CONTEXT *active_context;

/*========== MACROS ==========*/

#define Wrap_SDSP_Cyclecounter()
#define update_sound()
#define BIT(bit) (1 << (bit))

/*========== PROCEDURES ==========*/

/* SPCimpl.c procedures */
void DisplaySPC(void);

// void InvalidSPCOpcode(void);

void SPC_READ_DSP(void);

void SPC_WRITE_DSP(void);

/* spc700.c procedures */
void Reset_SPC(void);

u8 SPC_READ_PORT_W(u16 address);

int SPC_START(u32 cycles);

void SPC_WRITE_PORT_R(u16 address, u8 data);

u8 get_SPC_PSW(void);

void spc_restore_flags(void);

#endif
