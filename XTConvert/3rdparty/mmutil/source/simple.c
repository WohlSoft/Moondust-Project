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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "defs.h"
#include "mas.h"
#include "simple.h"
#include "files.h"
#include "samplefix.h"

// cREDITS TO CODA, LOOK AT THIS AWESOME CODES
u32 readbits(u8* buffer, unsigned int pos, unsigned int size) {
	u32 result=0;
	u32 i;
	
	for(i=0;i<size;i++) {
		u32 byte_pos;
		u32 bit_pos;
		byte_pos = (pos+i)>>3;
		bit_pos = (pos+i)&7;
		result |= ( (buffer[byte_pos] >> bit_pos) & 1 ) << i;
	}
	return result;
}

int get_ext( char* filename )
{
	int strl = strlen(filename);
	int x;
	u32 a=0;
	if( strl < 4 )
		return INPUT_TYPE_UNK;
	for( x = 0; x < 4; x++ )
	{
		if( filename[strl-x-1] != '.' )
		{
			a |= tolower(filename[strl-x-1]) << (x*8);
		}
		else
			break;
	}
	//a = tolower( filename[strl-1] ) | (tolower( filename[strl-1] )<<8) | tolower( filename[strl-2]<<16 ) | tolower( filename[strl-3]<<24 );
	
	switch( a )
	{
	case 'mod':
		return INPUT_TYPE_MOD;
	case 's3m':
		return INPUT_TYPE_S3M;
	case 'txt':
		return INPUT_TYPE_TXT;
	case 'wav':
		return INPUT_TYPE_WAV;
	case 'msl':
		return INPUT_TYPE_MSL;
	case 'xm':
		return INPUT_TYPE_XM;
	case 'it':
		return INPUT_TYPE_IT;
	case 'h':
		return INPUT_TYPE_H;
	}
	return INPUT_TYPE_UNK;
}

u32 calc_samplen_ex2( Sample* s )
{
	if( s->loop_type == 0 )
	{
		return s->sample_length;
	}
	else
	{
		return s->loop_end;
	}
}

u32 calc_samplooplen( Sample* s )
{
	u32 a;
	if( s->loop_type == 1 )
	{
		a = s->loop_end - s->loop_start;
		return a;
	}
	else if( s->loop_type == 2 )
	{
		a = (s->loop_end-s->loop_start) *2;
		return a;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

u32 calc_samplen( Sample* s )
{
	if( s->loop_type == 1 )
	{
		return s->loop_end;
	}
	else if( s->loop_type == 2 )
	{
		return (s->loop_end-s->loop_start)+s->loop_end;
	}
	else
	{
		return s->sample_length;
	}
}

u8 sample_dsformat( Sample* samp )
{
	if( samp->format & SAMPF_COMP )
	{
		return 2;
	}
	else
	{
		if( samp->format & SAMPF_SIGNED )
		{
			if( samp->format & SAMPF_16BIT )
				return 1;
			else
				return 0;
		}
		else
		{
			if( !(samp->format & SAMPF_16BIT) )
				return 3;
			else
				return 3; // error
		}
	}
}

u8 sample_dsreptype( Sample* samp )
{
	if( samp->loop_type )
		return 1;
	else
		return 2;
}

int clamp_s8( int value )
{
	if( value < -128 ) value = -128;
	if( value > 127 ) value = 127;
	return value;
}

int clamp_u8( int value )
{
	if( value < 0 ) value = 0;
	if( value > 255 ) value = 255;
	return value;
}

