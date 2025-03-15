/****************************************************************************
 *                                                          __              *
 *                ____ ___  ____ __  ______ ___  ____  ____/ /              *
 *               / __ `__ \/ __ `/ |/ / __ `__ \/ __ \/ __  /               *
 *              / / / / / / /_/ />  </ / / / / / /_/ / /_/ /                *
 *             /_/ /_/ /_/\__,_/_/|_/_/ /_/ /_/\____/\__,_/                 *
 *                                                                          *
 *         Copyright (c) 2008, Mukunda Johnson (mukunda@maxmod.org)         *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted, provided that the above   *
 * copyright notice and this permission notice appear in all copies.        *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

// thanks GBATEK for the ima-adpcm specification

#include <stdlib.h>
#include "defs.h"
#include "deftypes.h"
#include "mas.h"

static const s8 IndexTable[8] = {
    -1, -1, -1, -1, 2, 4, 6, 8
};

static const u16 AdpcmTable[89] = {
	    7,     8,     9,    10,    11,    12,    13,    14,    16,    17,
       19,    21,    23,    25,    28,    31,    34,    37,    41,    45,
       50,    55,    60,    66,    73,    80,    88,    97,   107,   118,
      130,   143,   157,   173,   190,   209,   230,   253,   279,   307,
      337,   371,   408,   449,   494,   544,   598,   658,   724,   796,
      876,   963,  1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
     2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static int read_sample( Sample* sample, int position )
{
	int s;
	if( sample->format & SAMPF_16BIT )
	{
		s = ((s16*)sample->data)[position];
	}
	else
	{
		// expand 8-bit value
		int a = ((s8*)sample->data)[position];
		s = (a<<8);
	}
	if( s < -32767 ) s = -32767;	// is this necessary?...
	return s;
}

static int minmax( int value, int low, int high )
{
	if( value < low ) value = low;
	if( value > high ) value = high;
	return value;
}

static int calc_delta( int diff, int step )
{
	int delta = (step >> 3);	// t/8
	
	if( diff >= step ) {		// t/1
		diff -= step;
		delta += step;
	}
	if( diff >= (step>>1) ) {	// t/2
		diff -= step>>1;
		delta += step>>1;
	}
	if( diff >= (step>>2) ) {	// t/4
		diff -= step>>2;
		delta += step>>2;
	}
	return delta;
}

//----------------------------------------------------------------------
// Compresses a sample with IMA-ADPCM
// Make sure the data has proper alignment/length!
//----------------------------------------------------------------------
void adpcm_compress_sample( Sample* sample )
//----------------------------------------------------------------------
{
	u32 x;
	u8* output;

	int prev_value;
	int curr_value;
	
	int diff;
	int data;
	int delta;
	
	int index;
	int step;
	
	// allocate space for sample (compressed size)
	output = (u8*)malloc( sample->sample_length/2+4 );
	
	prev_value = read_sample( sample, 0 );
	index = 0;

	{ // determine best (or close to best) initial table value

		int i;
		int smallest_error;
		int tmp_error;
		smallest_error = 9999999;


		diff = read_sample( sample, 1 ) - read_sample( sample, 0 );
		
		for( i = 0; i < 88; i++ )
		{
			tmp_error = calc_delta( diff, i ) - diff;
			if( tmp_error < smallest_error )
			{
				smallest_error = tmp_error;
				index = i;
			}
		}
	}
		
	// set data header
	(*((u32*)output)) =		prev_value			// initial PCM16 value
							| (index << 16);	// initial table index value
	
	step = AdpcmTable[index];
	
	for( x = 0; x < sample->sample_length; x++ )
	{
		curr_value = read_sample( sample, x );
		
		diff = curr_value - prev_value;
		if( diff < 0 )
		{
			// negate difference & set negative bit
			diff = -diff;
			data = 8;
		}
		else
		{
			// clear negative flag
			data = 0;
		}
		
		/*
		  difference calculation:
		  Diff = AdpcmTable[Index]/8
		  IF (data4bit AND 1) THEN Diff = Diff + AdpcmTable[Index]/4
		  IF (data4bit AND 2) THEN Diff = Diff + AdpcmTable[Index]/2
		  IF (data4bit AND 4) THEN Diff = Diff + AdpcmTable[Index]/1
		*/	
		
		delta = (step >> 3);		// t/8 (always)
		
		if( diff >= step ) {		// t/1
			data |= 4;
			diff -= step;
			delta += step;
		}
		if( diff >= (step>>1) ) {	// t/2
			data |= 2;
			diff -= step>>1;
			delta += step>>1;
		}
		if( diff >= (step>>2) ) {	// t/4
			data |= 1;
			diff -= step>>2;
			delta += step>>2;
		}
		
		// add/subtract delta
		prev_value += (data&8) ? -delta : delta;
		
		// claamp output
		prev_value = minmax( prev_value, -0x7FFF, 0x7FFF );
				
		// add index table value (and clamp)
		index = minmax( index + IndexTable[data & 7], 0, 88 );
		
		// read new step value
		step = AdpcmTable[index];
		
		// write output
		if( (x & 1) )
			output[(x>>1)+4] |= (data) << 4;
		else
			output[(x>>1)+4] = (data);
	}

	// delete old sample
	free( sample->data );

	// assign new sample
	sample->data = output;

	// new format
	sample->format = SAMP_FORMAT_ADPCM;

	// new length/loop
	sample->sample_length = (sample->sample_length/2) +4;
	sample->loop_start /= 2;
	sample->loop_end /= 2;
	
	// step loop past adpcm header
	sample->loop_start += 4;
	sample->loop_end += 4;
}
