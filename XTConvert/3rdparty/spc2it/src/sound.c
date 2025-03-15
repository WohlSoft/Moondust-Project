/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "emu.h"
#include "sound.h"

sndvoice SNDvoices[8];
s32 SNDratecnt;

static const u32 C[0x20] = {
    0x0,    0x20000, 0x18000, 0x14000, 0x10000, 0xC000, 0xA000, 0x8000, 0x6000, 0x5000, 0x4000,
    0x3000, 0x2800,  0x2000,  0x1800,  0x1400,  0x1000, 0xC00,  0xA00,  0x800,  0x600,  0x500,
    0x400,  0x300,   0x280,   0x200,   0x180,   0x140,  0x100,  0xC0,   0x80,   0x40}; // How many cycles till adjust
                                                                                       // ADSR/GAIN

// PUBLIC (non-static) functions:

s32 SNDDoEnv(s32 voice)
{
	u32 envx, c;
	envx = SNDvoices[voice].envx;
	for (;;)
	{
		u32 cyc = TotalCycles - SNDvoices[voice].envcyc;
		switch (SNDvoices[voice].envstate)
		{
		case ATTACK:
			c = C[(SNDvoices[voice].ar << 1) + 1];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx += 0x2000000; // add 1/64th
				if (envx >= 0x7F000000)
				{
					envx = 0x7F000000;
					if (SNDvoices[voice].sl != 7)
						SNDvoices[voice].envstate = DECAY;
					else
						SNDvoices[voice].envstate = SUSTAIN;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case DECAY:
			c = C[(SNDvoices[voice].dr << 1) + 0x10];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx = (envx >> 8) * 255; // mult by 1-1/256
				if (envx <= 0x10000000 * (SNDvoices[voice].sl + 1))
				{
					envx = 0x10000000 * (SNDvoices[voice].sl + 1);
					SNDvoices[voice].envstate = SUSTAIN;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case SUSTAIN:
			c = C[SNDvoices[voice].sr];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx = (envx >> 8) * 255; // mult by 1-1/256
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case RELEASE:
			// says add 1/256??  That won't release, must be subtract.
			// But how often?  Oh well, who cares, I'll just
			// pick a number. :)
			c = C[0x1A];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx -= 0x800000; // sub 1/256th
				if ((envx == 0) || (envx > 0x7F000000))
				{
					SPC_DSP[0x4C] &= ~(1 << voice);
					return SNDvoices[voice].envx = 0;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case INCREASE:
			c = C[SNDvoices[voice].gn];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx += 0x2000000; // add 1/64th
				if (envx > 0x7F000000)
				{
					SNDvoices[voice].envcyc = TotalCycles;
					return SNDvoices[voice].envx = 0x7F000000;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case DECREASE:
			c = C[SNDvoices[voice].gn];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx -= 0x2000000;     // sub 1/64th
				if (envx > 0x7F000000) // underflow
				{
					SNDvoices[voice].envcyc = TotalCycles;
					return SNDvoices[voice].envx = 0;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case EXP:
			c = C[SNDvoices[voice].gn];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				envx = (envx >> 8) * 255; // mult by 1-1/256
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case BENT:
			c = C[SNDvoices[voice].gn];
			if (c == 0)
			{
				SNDvoices[voice].envcyc = TotalCycles;
				return SNDvoices[voice].envx = envx;
			}
			if (cyc > c)
			{
				SNDvoices[voice].envcyc += c;
				if (envx < 0x60000000)
					envx += 0x2000000; // add 1/64th
				else
					envx += 0x800000; // add 1/256th
				if (envx > 0x7F000000)
				{
					SNDvoices[voice].envcyc = TotalCycles;
					return SNDvoices[voice].envx = 0x7F000000;
				}
			}
			else
				return SNDvoices[voice].envx = envx;
			break;
		case DIRECT:
			SNDvoices[voice].envcyc = TotalCycles;
			return envx;
		}
	}
}

void SNDNoteOn(u8 v)
{
	s32 i, cursamp, adsr1, adsr2, gain;
	v &= 0xFF;
	for (i = 0; i < 8; i++)
		if (v & (1 << i))
		{
			cursamp = SPC_DSP[4 + (i << 4)];
			if (cursamp < 512)
			{
				SPC_DSP[0x4C] |= (1 << i);
				// figure ADSR/GAIN
				adsr1 = SPC_DSP[(i << 4) + 5];
				if (adsr1 & 0x80)
				{
					// ADSR mode
					adsr2 = SPC_DSP[(i << 4) + 6];
					SNDvoices[i].envx = 0;
					SNDvoices[i].envcyc = TotalCycles;
					SNDvoices[i].envstate = ATTACK;
					SNDvoices[i].ar = adsr1 & 0xF;
					SNDvoices[i].dr = adsr1 >> 4 & 7;
					SNDvoices[i].sr = adsr2 & 0x1f;
					SNDvoices[i].sl = adsr2 >> 5;
				}
				else
				{
					// GAIN mode
					gain = SPC_DSP[(i << 4) + 7];
					if (gain & 0x80)
					{
						SNDvoices[i].envcyc = TotalCycles;
						SNDvoices[i].envstate = gain >> 5;
						SNDvoices[i].gn = gain & 0x1F;
					}
					else
					{
						SNDvoices[i].envx = (gain & 0x7F) << 24;
						SNDvoices[i].envstate = DIRECT;
					}
				}
			}
		}
	if (SPC_Write_DSP_Hook)
		(*SPC_Write_DSP_Hook)(v);
}

void SNDNoteOff(u8 v)
{
	s32 i;
	for (i = 0; i < 8; i++)
		if (v & (1 << i))
		{
			SNDDoEnv(i);
			SNDvoices[i].envstate = RELEASE;
		}
}

s32 SNDInit()
{
	s32 i;
	for (i = 0; i < 8; i++)
		SNDvoices[i].envx = 0;
	return (0);
}
