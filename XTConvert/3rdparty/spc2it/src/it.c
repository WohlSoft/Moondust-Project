/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "it.h"
#include "sound.h"
#include "emu.h"

static itdata ITdata[8]; // Temp memory for patterns before going to file
static sndsamp *ITSamples[IT_SAMPLE_MAX];
static u8 *ITpattbuf[NUM_PATT_BUFS]; // Where patterns are going to be , before writing to file
static u8 *ITPatterns;
static u32 ITPatternsSize;
static s32 ITpattlen[NUM_PATT_BUFS]; // lengths of each pattern
static s32 ITcurbuf, ITbufpos, ITcurrow; // Pointers into temp pattern buffers
static s32 ITrows; // Number of rows per pattern

static pcm_t p1, p2;
static s32 offset[IT_PATTERN_MAX]; // table of offsets into temp file to each pattern
static s32 curpatt; // which pattern we are on in temp file
static s32 curoffs; // where we are in file

static sndsamp *ITAllocateSample(s32 size)
{
	sndsamp *s;
	if (((s = (sndsamp *)calloc(1, sizeof(sndsamp))) == NULL) || ((s->buf = (s16 *)calloc(1, size * sizeof(s16))) == NULL))
		return (NULL);
	s->length = size;
	s->loopto = -1;
	s->freq = 0;
	return (s);
}

static s32 ITGetBRRPrediction(u8 filter, pcm_t p1, pcm_t p2)
{
	s32 p;
	switch (filter)
	{
	case 0:
		return 0;

	case 1:
		p = p1;
		p -= p1 >> 4;
		return p;

	case 2:
		p = p1 << 1;
		p += (-(p1 + (p1 << 1))) >> 5;
		p -= p2;
		p += p2 >> 4;
		return p;

	case 3:
		p = p1 << 1;
		p += (-(p1 + (p1 << 2) + (p1 << 3))) >> 6;
		p -= p2;
		p += (p2 + (p2 << 1)) >> 4;
		return p;
	}
	return 0;
}

static void ITDecodeSampleInternal(s8 s, u8 shift_am, u8 filter)
{
	s32 a;
	if (shift_am <= 0x0c) // Valid shift count
		a = ((s < 8 ? s : s - 16) << shift_am) >> 1;
	else
		a = s < 8 ? 1 << 11 : (-1) << 11; // Values "invalid" shift counts

	a += ITGetBRRPrediction(filter, p1, p2);

	if (a > 0x7fff)
		a = 0x7fff;
	else if (a < -0x8000)
		a = -0x8000;
	if (a > 0x3fff)
		a -= 0x8000;
	else if (a < -0x4000)
		a += 0x8000;

	p2 = p1;
	p1 = a;
}

static s32 ITDecodeSample(u16 start, sndsamp **sp)
{
	sndsamp *s;
	u8 *src;
	u16 end;
	u32 brrptr, sampptr = 0;
	s32 i;
	src = &SPCRAM[start];
	for (end = 0; !(src[end] & 1); end += 9)
		;
	i = (end + 9) / 9 * 16;
	*sp = s = ITAllocateSample(i);
	if (s == NULL)
		return 1;
	if (src[end] & 2)
		s->loopto = 0;
	for (brrptr = 0; brrptr <= end;)
	{
		u8 range = src[brrptr++];
		u8 filter = (range & 0x0c) >> 2;
		u8 shift_amount = (range >> 4) & 0x0F;
		for (i = 0; i < 8; i++, brrptr++)
		{
			ITDecodeSampleInternal(src[brrptr] >> 4, shift_amount, filter); // Decode high nybble
			s->buf[sampptr++] = 2 * p1;
			ITDecodeSampleInternal(src[brrptr] & 0x0F, shift_amount, filter); // Decode low nybble
			s->buf[sampptr++] = 2 * p1;
		}
	}
	return 0;
}

static void ITUpdateSample(s32 s)
{
	s32 i;
	struct srcdir_s
	{
		u16 vptr, lptr;
	} *SRCDIR;
	i = SPC_DSP[0x5D] << 8; //sample directory table...
	SRCDIR = (struct srcdir_s *)(void *)&SPCRAM[i];
	if (ITDecodeSample(SRCDIR[s].vptr, &ITSamples[s]))
		return;
	if (ITSamples[s]->loopto != -1)
	{
		ITSamples[s]->loopto = (SRCDIR[s].lptr - SRCDIR[s].vptr) / 9 * 16;
		if ((ITSamples[s]->loopto > ITSamples[s]->length) || (ITSamples[s]->loopto < 0))
			ITSamples[s]->loopto = -1;
	}
}

static s32 ITPitchToNote(s32 pitch, s32 base)
{
	f64 tmp;
	s32 note;
	tmp = log2((f64)pitch / (f64)base) * 12 + 60;
	if (tmp > 127)
		tmp = 127;
	else if (tmp < 0)
		tmp = 0;
	note = (s32)tmp;
	if ((s32)(tmp * 2) != (note * 2))
		note++; // correct rounding
	return note;
}

static void ITWriteDSPCallback(u8 v)
{
	s32 addr_lo = SPC_DSP_ADDR & 0xF;
	s32 addr_hi = SPC_DSP_ADDR >> 4;

	// fix "missing first note" by accepting a slightly different DSP state on first row
	if (!(addr_lo == 12))
	{
		if (TotalCycles > 100)
			return;
		else
			printf("Warning: accepting low address %u at cycle %u\n", (unsigned)addr_lo, (unsigned)TotalCycles);
	}

	if (!(addr_hi == 4))
	{
		if (TotalCycles > 100)
			return;
		else
			printf("Warning: accepting high address %u at cycle %u\n", (unsigned)addr_hi, (unsigned)TotalCycles);
	}

	s32 i, cursamp, pitch;
	v &= 0xFF; // ext
	for (i = 0; i < 8; i++)
	{
		if (v & (1 << i))
		{
			cursamp = SPC_DSP[4 + (i << 4)]; // Number of current sample
			if (cursamp < IT_SAMPLE_MAX) // Only 99 samples supported, sorry
			{
				if (ITSamples[cursamp] == NULL)
					ITUpdateSample(cursamp);
				pitch = (s32)(*(u16 *)&SPC_DSP[(i << 4) + 0x02]) * 7.8125; // Ext, Get pitch
				if (ITSamples[cursamp]->freq == 0)
					ITSamples[cursamp]->freq = pitch;
				if ((pitch != 0) && (ITSamples[cursamp] != NULL) &&
				    (ITSamples[cursamp]->freq != 0)) // Ext, Sample is actually useful?
				{
					ITdata[i].mask |= IT_MASK_NOTE_SAMPLE_ADJUSTVOLUME; // Update note, sample, and adjust the volume.
					ITdata[i].note = ITPitchToNote(pitch, ITSamples[cursamp]->freq); // change pitch to note
					ITdata[i].pitch = (s32)(pow(2, ((f64)ITdata[i].note - 60) / 12) *
					                        (f64)ITSamples[cursamp]->freq); // needed for pitch slide detection
					ITdata[i].lvol = 0;
					ITdata[i].rvol = 0;
					// IT code will get sample from DSP buffer
				}
			}
		}
	}
}

static int ITSSave(sndsamp *s, FILE *f) // Save sample
{
	s32 loopto = -1;
	s32 length = 0;
	s32 freq = 0;
	s32 ofs = ftell(f);
	ITFileSample *sHeader = (ITFileSample *)calloc(1, sizeof(ITFileSample));
	if (sHeader == NULL)
	{
		// printf("Error: could not allocate memory for ITFileSample struct\n");
		return(1);
	}
	if (s != NULL)
	{
		loopto = s->loopto;
		length = s->length;
		freq = s->freq;
	}
	else
	{
		freq = 8363;
		loopto = 0;
	}
	memcpy(sHeader->magic, "IMPS", 4);
	if (length)
		strcpy(sHeader->fileName, "SPC2ITSAMPLE");
	sHeader->GlobalVolume = 64;
	sHeader->Flags |= 2; // Bit 1 (16 bit)
	if (length)
		sHeader->Flags |= 1; // Bit 0 (sample included with header)
	sHeader->Volume = 64;
	if (length)
		strcpy(sHeader->SampleName, "SPC2ITSAMPLE");
	sHeader->Convert = 1;
	sHeader->DefaultPan = 0;
	sHeader->NumberOfSamples = length;
	if (loopto != -1)
	{
		sHeader->Flags |= 16; // Bit 4 (Use loop)
		sHeader->LoopBeginning = loopto;
		sHeader->LoopEnd = length;
	}
	sHeader->C5Speed = freq;
	if (length)
		sHeader->SampleOffset = ofs + sizeof(ITFileSample);
	fwrite(sHeader, sizeof(ITFileSample), 1, f);
	free(sHeader);
	if (length)
		fwrite(s->buf, s->length * 2, 1, f); // Write the sample itself... 2x length.

	return 0;
}

static void ITWritePattern(ITPatternInfo *pInfo) {
	ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Channel;
	ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Mask;

	if (pInfo->Mask & IT_MASK_NOTE)
		ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Note;
	if (pInfo->Mask & IT_MASK_SAMPLE)
		ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Sample;
	if (pInfo->Mask & IT_MASK_ADJUSTVOLUME) 
		ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Volume;
	if (pInfo->Mask & IT_MASK_PITCHSLIDE)
	{
		ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->Command;
		ITpattbuf[ITcurbuf][ITbufpos++] = pInfo->CommandValue;
	}
}

s32 ITStart(s32 rows) // Opens up temporary file and inits writing
{
	SPCAddWriteDSPCallback(&ITWriteDSPCallback);
	s32 i;
	ITrows = rows;

	for (i = 0; i < NUM_PATT_BUFS; i++)
	{
		ITpattbuf[i] = (u8 *)calloc(1, Mem64k - 8); //Don't include the 8 byte header
		if (ITpattbuf[i] == NULL)
		{
			// printf("Error: could not allocate memory for IT pattern buffer\n");
			return(1);
		}
		ITpattlen[i] = 0;
	}

	ITPatterns = (u8 *)calloc(1, Mem64k * IT_PATTERN_MAX);
	if (ITPatterns == NULL)
	{
		// printf("Error: could not allocate memory for IT pattern storage\n");
		return(1);
	}
	ITPatternsSize = 0;
	ITcurbuf = 0;
	ITbufpos = 0;
	ITcurrow = 0;

	curoffs = 0;
	for (i = 0; i < IT_PATTERN_MAX; i++)
		offset[i] = -1; // -1 means unused pattern
	curpatt = 0;

	for (i = 0; i < 8; i++)
		ITdata[i].mask = 0;
	return 0;
}

s32 ITUpdate() // Dumps pattern buffers to file
{
	u8 *tmpptr;
	s32 i;
	ITFilePattern *pHeader = (ITFilePattern *)calloc(1, sizeof(ITFilePattern));
	if (pHeader == NULL)
	{
		// printf("Error: could not allocate memory for ITFilePattern struct\n");
		return(1);
	}
	for (i = 0; i < ITcurbuf; i++)
	{
		offset[curpatt] = curoffs;
		pHeader->Length = ITpattlen[i];
		pHeader->Rows = ITrows;
		memcpy(&ITPatterns[ITPatternsSize], pHeader, sizeof(ITFilePattern));
		ITPatternsSize += sizeof(ITFilePattern);
		memcpy(&ITPatterns[ITPatternsSize], ITpattbuf[i], ITpattlen[i]);
		ITPatternsSize += ITpattlen[i];
		curoffs += ITpattlen[i] + 8;
		if (curpatt < IT_PATTERN_MAX) 
			curpatt++; // Continue counting if we haven't reached the limit yet
	}
	free(pHeader);
	tmpptr = ITpattbuf[0];
	ITpattbuf[0] = ITpattbuf[ITcurbuf];
	ITpattbuf[ITcurbuf] = tmpptr;
	ITcurbuf = 0;
	return 0;
}

s32 ITWrite(const char *fn) // Write the final IT file
{
	if (fn == NULL)
	{
		// printf("Error: no IT filename\n");
		return 1;
	}

	FILE *f;
	s32 i, t, numsamps, ofs;

	ITPatternInfo *pInfo;
	pInfo = (ITPatternInfo *)calloc(1, sizeof(ITPatternInfo));
	if (pInfo == NULL)
	{
		// printf("Error: could not allocate memory for ITPatternInfo struct\n");
		return 1;
	}

	// START IT CLEANUP
	pInfo->Mask = 1;
	pInfo->Note = 254; //note cut
	// Stop all notes and loop back to the beginning
	for (i = 0; i < 7; i++) // Save the last channel to put loop in
	{
		pInfo->Channel = (i + 1) | 128; // Channels are 1 based (Channels start at 1, not 0, ITTECH.TXT is WRONG) !!!
		ITWritePattern(pInfo);
	}
	pInfo->Channel = (7 + 1) | 128;
	pInfo->Mask = 9; // 1001 (note, special command)
	pInfo->Command = 2; // Effect B: jump to...
	pInfo->CommandValue = 0; //...order 0 (Loop to beginning)
	ITWritePattern(pInfo);
	free(pInfo);

	while (ITcurrow++ < ITrows)
		ITpattbuf[ITcurbuf][ITbufpos++] = 0; // end-of-row

	ITpattlen[ITcurbuf++] = ITbufpos;
	if (ITUpdate()) // Save the changes we just made
		return 1;
	// END IT CLEANUP

	ITFileHeader *fHeader;
	fHeader = (ITFileHeader *)calloc(1, sizeof(ITFileHeader));
	if (fHeader == NULL)
	{
		// printf("Error: could not allocate memory for ITFileHeader struct\n");
		return 1;
	}

	memcpy(fHeader->magic, "IMPM", 4);
	if (SPCInfo.SongTitle[0])
		strncpy(fHeader->songName, SPCInfo.SongTitle, 25);
	else
		strcpy(fHeader->songName, "spc2it conversion"); // default string
	fHeader->OrderNumber = curpatt + 1; // number of orders + terminating order
	for (numsamps = IT_SAMPLE_MAX; ITSamples[numsamps - 1] == NULL; numsamps--)
		; // Count the number of samples (the reason of the minus one is because c arrays start at 0)
	numsamps++;
	fHeader->SampleNumber = numsamps; // Number of samples
	fHeader->PatternNumber = curpatt; // Number of patterns
	fHeader->TrackerCreatorVersion = 0xDAEB; // Created with this tracker version
	fHeader->TrackerFormatVersion = 0x200; // Compatible with this tracker version
	fHeader->Flags = 9; // Flags: Stereo, Linear Slides
	fHeader->GlobalVolume = 128; // Global volume
	fHeader->MixVolume = 100; // Mix volume
	fHeader->InitialSpeed = 1; // Initial speed (fastest)
	fHeader->InitialTempo = (u8)(SPCUpdateRate * 2.5); // Initial tempo (determined by update rate)
	fHeader->PanningSeperation = 128; // Stereo separation (max)
	for (i = 0; i < 8; i++) 
		fHeader->ChannelPan[i] = 32; // Channel pan: Set 8 channels to center
	for (i = 8; i < 64; i++)
		fHeader->ChannelPan[i] = 128; // Disable the rest of the channels (Value: +128)
	for (i = 0; i < 8; i++)
		fHeader->ChannelVolume[i] = 64; // Channel Vol: set 8 channels loud

	f = fopen(fn, "wb");
	if (f == NULL)
	{
		// printf("Error: could not open IT file\n");
		free(fHeader);
		return 1;
	}

	// header
	fwrite(fHeader, sizeof(ITFileHeader), 1, f);
	free(fHeader);

	// orders
	for (i = 0; i < curpatt; i++)
		fputc(i, f); // Write from 0 to the number of patterns (max: 0xFD)
	fputc(255, f); // terminating order

	// Sample offsets
	ofs = sizeof(ITFileHeader) + (curpatt + 1) + ((numsamps * sizeof(s32)) + (curpatt * sizeof(s32)));
	for (i = 0; i < numsamps; i++)
	{
		fwrite(&ofs, sizeof(s32), 1, f);
		ofs += sizeof(ITFileSample);
		if (ITSamples[i] != NULL) // Sample is going to be put in file? Add the length of the sample.
			ofs += (ITSamples[i]->length * 2);
	}

	// Pattern offsets
	for (i = 0; i < curpatt; i++)
	{
		t = offset[i] + ofs;
		fwrite(&t, sizeof(s32), 1, f);
	}

	// samples
	for (i = 0; i < numsamps; i++)
	{
		if (ITSSave(ITSamples[i], f))
		{
			fclose(f);
			return 1;
		}
	}

	// patterns
	fwrite(ITPatterns, ITPatternsSize, 1, f);

	// close file
	fclose(f);

	return 0;
}

void ITCleanup()
{
	// generic cleanup that must occur in any case
	for (int i = 0; i < NUM_PATT_BUFS; i++)
	{
		free(ITpattbuf[i]);
		ITpattbuf[i] = NULL;
	}

	for (int i = 0; i < IT_SAMPLE_MAX; i++)
	{
		if (!ITSamples[i])
			continue;

		free(ITSamples[i]->buf);
		free(ITSamples[i]);

		ITSamples[i] = NULL;
	}

	free(ITPatterns);
	ITPatterns = NULL;
}

int ITMix()
{
	s32 envx, pitchslide, lvol = 0, rvol = 0, pitch, temp = 0, voice;
	ITPatternInfo *pInfo = (ITPatternInfo *)calloc(1, sizeof(ITPatternInfo));
	if (pInfo == NULL)
	{
		// printf("Error: could not allocate memory for ITPatternInfo struct\n");
		return(1);
	}
	u8 mastervolume = SPC_DSP[0x0C];
	for (voice = 0; voice < 8; voice++)
	{
		if ((SPC_DSP[0x4C] & (1 << voice))) // 0x4C == key on
		{
			envx = SNDDoEnv(voice);
			lvol = (envx >> 24) * (s32)((s8)SPC_DSP[(voice << 4)       ]) * mastervolume >> 14; // Ext
			rvol = (envx >> 24) * (s32)((s8)SPC_DSP[(voice << 4) + 0x01]) * mastervolume >> 14; // Ext
			// Volume no echo: (s32)((s8)SPC_DSP[(voice << 4)       ]) * mastervolume >> 7;


			pitch = (s32)(*(u16 *)&SPC_DSP[(voice << 4) + 0x02]) * 7.8125; // This code merges 2 numbers together, high and low 8 bits, to make 16 bits.
			// adjust for negative volumes
			if (lvol < 0)
				lvol = -lvol;
			if (rvol < 0)
				rvol = -rvol;
			// lets see if we need to pitch slide
			if (pitch && ITdata[voice].pitch)
			{
				pitchslide = (s32)(log2((f64)pitch / (f64)ITdata[voice].pitch) * 768.0);
				if (pitchslide)
					ITdata[voice].mask |= IT_MASK_PITCHSLIDE; // enable pitch slide
			}
			// adjust volume?
			if ((lvol != ITdata[voice].lvol) || (rvol != ITdata[voice].rvol))
			{
				if (lvol + rvol != ITdata[voice].lvol + ITdata[voice].rvol)
					ITdata[voice].mask |= IT_MASK_ADJUSTVOLUME; // Enable adjust volume
				if (ITdata[voice].lvol * (lvol + rvol + 1) / (ITdata[voice].lvol + ITdata[voice].rvol + 1) != lvol
					|| ITdata[voice].rvol * (lvol + rvol + 1) / (ITdata[voice].lvol + ITdata[voice].rvol + 1) != rvol)
				{
					ITdata[voice].mask |= IT_MASK_ADJUSTPAN; // Enable adjust panning
				}

				ITdata[voice].lvol = lvol;
				ITdata[voice].rvol = rvol;
			}
		}

		pInfo->Channel = (voice + 1) | 128; //Channels here are 1 based!
		pInfo->Mask = ITdata[voice].mask & ~IT_MASK_ADJUSTPAN;

		if (ITdata[voice].mask & IT_MASK_NOTE)
			pInfo->Note = ITdata[voice].note;
		if (ITdata[voice].mask & IT_MASK_SAMPLE)
			pInfo->Sample = SPC_DSP[(voice << 4) + 4] + 1;

		if (ITdata[voice].mask & IT_MASK_PITCHSLIDE)
		{
			if (pitchslide > 0xF)
			{
				temp = pitchslide >> 2;
				if (temp > 0xF)
					temp = 0xF;
				temp |= FINE_SLIDE;
				ITdata[voice].pitch = (s32)((f64)ITdata[voice].pitch * pow(2, (f64)((temp & 0xF) << 2) / 768.0)); 
				pInfo->Command = EFFECT_F;
				pInfo->CommandValue = temp;
			}
			else if (pitchslide > 0)
			{
				temp = pitchslide | EXTRA_FINE_SLIDE;
				ITdata[voice].pitch = (s32)((f64)ITdata[voice].pitch * pow(2, (f64)(temp & 0xF) / 768.0));
				pInfo->Command = EFFECT_F;
				pInfo->CommandValue = temp;
			}
			else if (pitchslide > -0x10)
			{
				temp = (-pitchslide) | EXTRA_FINE_SLIDE;
				ITdata[voice].pitch = (s32)((f64)ITdata[voice].pitch * pow(2, (f64)(temp & 0xF) / -768.0));
				pInfo->Command = EFFECT_E;
				pInfo->CommandValue = temp;
			}
			else
			{
				temp = (-pitchslide) >> 2;
				if (temp > 0xF)
					temp = 0xF;
				temp |= FINE_SLIDE;
				ITdata[voice].pitch = (s32)((f64)ITdata[voice].pitch * pow(2, (f64)((temp & 0xF) << 2) / -768.0));
				pInfo->Command = EFFECT_E;
				pInfo->CommandValue = temp;
			}
		}
		else if (ITdata[voice].mask & IT_MASK_ADJUSTPAN)
		{
			pInfo->Command = EFFECT_X;
			pInfo->CommandValue = 256 * ITdata[voice].rvol / (ITdata[voice].lvol + ITdata[voice].rvol + 1);
			pInfo->Mask |= IT_MASK_PITCHSLIDE;
			ITdata[voice].mask &= ~IT_MASK_ADJUSTPAN;
		}

		if (ITdata[voice].mask & IT_MASK_ADJUSTVOLUME)
		{
			pInfo->Volume = ((lvol + rvol) > 64) ? 64 : (lvol + rvol);
		}

		ITWritePattern(pInfo); // Write for left (only?) channel

		ITdata[voice].mask &= IT_MASK_ADJUSTPAN; // Clear the mask
	}
	ITpattbuf[ITcurbuf][ITbufpos++] = 0; // End-of-row
	if (++ITcurrow >= ITrows)
	{
		ITpattlen[ITcurbuf++] = ITbufpos;
		ITbufpos = 0; // Reset buffer pos
		ITcurrow = 0; // Reset current row
	}
	free(pInfo);

	return 0;
}
