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

#define MAX_UNROLL_THRESHOLD	1024	// will unroll upto 1kb more of data (when fixing NDS samples)
#define GBA_MIN_LOOP_SIZE 512

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "mas.h"
#include "errors.h"
#include "systems.h"
#include "adpcm.h"

extern int mm_ignore_sflags;

void Sample_PadStart( Sample* samp, u32 count )
{
	// Pad beginning of sample with zero
	u8* newdata8;
	u16* newdata16;
	u32 x;
	if( count == 0 )
		return;		// nothing to do
	if( samp->format & SAMPF_16BIT )
	{
		newdata16 = (u16*)malloc( (samp->sample_length+count)*2 );
		for( x = 0; x < count; x++ )
			newdata16[x]=32768;
		for( x = 0; x < samp->sample_length; x++ )
			newdata16[count+x]=((u16*)samp->data)[x];
		free( samp->data );
		samp->data = (void*)newdata16;
	}
	else
	{
		newdata8 = (u8*)malloc( (samp->sample_length+count) );
		for( x = 0; x < count; x++ )
			newdata8[x]=128;
		for( x = 0; x < samp->sample_length; x++ )
			newdata8[count+x] = ((u8*)samp->data)[x];
		free( samp->data );
		samp->data = (void*)newdata8;
	}
	samp->loop_start    += count;
	samp->loop_end      += count;
	samp->sample_length += count;
}

void Sample_PadEnd( Sample* samp, u32 count )
{
	// Pad end of sample with zero
	u8* newdata8;
	u16* newdata16;
	u32 x;
	if( count == 0 )
		return;		// nothing to do
	if( samp->format & SAMPF_16BIT )
	{
		newdata16 = malloc( (samp->sample_length+count)*2 );
		for( x = 0; x < samp->sample_length; x++ )
			newdata16[x]= ((u16*)samp->data)[x];
		for( x = 0; x < count; x++ )
			newdata16[samp->sample_length+x]=32768;
		free( samp->data );
		samp->data = (void*)newdata16;
	}
	else
	{
		newdata8 = malloc( (samp->sample_length+count) );
		for( x = 0; x < samp->sample_length; x++ )
			newdata8[x]= ((u8*)samp->data)[x];
		for( x = 0; x < count; x++ )
			newdata8[samp->sample_length+x]=128;
		free( samp->data );
		samp->data = (void*)newdata8;
	}
	samp->loop_end      += count;
	samp->sample_length += count;
}

void Unroll_Sample_Loop( Sample* samp, u32 count )
{
	// unrolls sample loop (count) times
	// loop end MUST equal sample length
	u8* newdata8;
	u16* newdata16;
	u32 newlen;
	u32 looplen;
	u32 x;
	looplen = samp->loop_end-samp->loop_start;
	newlen = samp->sample_length + looplen*count;
	if( samp->format & SAMPF_16BIT )
	{
		newdata16 = (u16*)malloc( newlen *2 );
		for( x = 0; x < samp->sample_length; x++ )
			newdata16[x] = ((u16*)samp->data)[x];
		for( x = 0; x < looplen*count; x++ )
			newdata16[samp->sample_length+x] = ((u16*)samp->data)[samp->loop_start+ (x%looplen)];
		free( samp->data );
		samp->data = (void*)newdata16;
	}
	else
	{
		newdata8 = (u8*)malloc( newlen );
		for( x = 0; x < samp->sample_length; x++ )
			newdata8[x] = ((u8*)samp->data)[x];
		for( x = 0; x < looplen*count; x++ )
			newdata8[samp->sample_length+x] = ((u8*)samp->data)[samp->loop_start+ (x%looplen)];
		free( samp->data );
		samp->data = (void*)newdata8;
	}
	samp->loop_end += looplen*count;
	samp->sample_length += looplen*count;
}

void Unroll_BIDI_Sample( Sample* samp )
{
	// sample length MUST equal sample loop end
	// sample MUST have loop type 2 (BIDI)
	u8* newdata8;
	u16* newdata16;
	u32 newlen;
	u32 looplen;
	u32 x;

	looplen = samp->loop_end-samp->loop_start;
	newlen = (samp->sample_length + looplen);
	
	if( samp->format & SAMPF_16BIT )
	{
		newdata16 = malloc( newlen *2 );
		for( x = 0; x < samp->sample_length; x++ )
			newdata16[x] = ((u16*)samp->data)[x];
		for( x = 0; x < looplen; x++ )
			newdata16[x+samp->sample_length] = ((u16*)samp->data)[samp->loop_end-1-x];
		free( samp->data );
		samp->data = (void*)newdata16;
	}
	else
	{
		newdata8 = malloc( newlen );
		for( x = 0; x < samp->sample_length; x++ )
			newdata8[x] = ((u8*)samp->data)[x];
		for( x = 0; x < looplen; x++ )
			newdata8[x+samp->sample_length] = ((u8*)samp->data)[samp->loop_end-1-x];
		free( samp->data );
		samp->data = (void*)newdata8;
	}
	samp->loop_type = 1;
	samp->sample_length += looplen;
	samp->loop_end += looplen;
}

/* NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
  The following resample routine was stolen from CHIBITRACKER (http://chibitracker.berlios.de), thanks reduz!
 NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE */

void Resample( Sample* samp, u32 newsize )
{
	// output pointers
	u8	*dst8 =0;
	u16	*dst16=0;
	u8	*src8 = (u8*)samp->data;
	u16	*src16 = (u16*)samp->data;
	
	int oldlength = samp->sample_length;
	int lpoint = samp->loop_start;
	
	bool bit16 = samp->format & SAMPF_16BIT;
	double sign_diff;
	
	// allocate memory
	if( bit16 )
	{
		dst16 = (u16*)malloc(newsize*2);
		sign_diff = 32768.0;
	}
	else
	{
		dst8 = (u8*)malloc(newsize);
		sign_diff = 128.0;
	}
		
	double tscale = (double)oldlength / (double)newsize;
	double posf;
	
	for( unsigned int i = 0; i < newsize; i++ )
	{
		posf = (double)i * tscale;
		int posi = (int)floor(posf);
		
		double mu = posf - (double)posi;
		double s0, s1, s2, s3;
		double mu2, a0, a1, a2, a3, res;
		
		// get previous, current, next, and after next samples
		if( bit16 )
		{
			s0 = (posi-1) < 0 ? 0 :		((double)(src16[posi-1]));
			s1 =							((double)(src16[posi  ]));
			s2 = (posi+1) >= oldlength ? 
									(samp->loop_type ? 
										((double)(src16[lpoint + (posi + 1 - oldlength)])) : 0) : 
										((double)(src16[posi+1]));
			s3 = (posi+2) >= oldlength ?
									(samp->loop_type ? 
										((double)(src16[lpoint + (posi + 2 - oldlength)])) : 0) : 
										((double)(src16[posi+2]));
		}
		else
		{
			s0 = (posi-1) < 0 ? 0 :		((double)(src8[posi-1]));
			s1 =							((double)(src8[posi  ]));
			s2 = (posi+1) >= oldlength ? 
									(samp->loop_type ? 
										((double)(src8[lpoint + (posi + 1 - oldlength)])) : 0) : 
										((double)(src8[posi+1]));
			s3 = (posi+2) >= oldlength ?
									(samp->loop_type ? 
										((double)(src8[lpoint + (posi + 2 - oldlength)])) : 0) : 
										((double)(src8[posi+2]));
		}
		
		// sign data
		s0 -= sign_diff;
		s1 -= sign_diff;
		s2 -= sign_diff;
		s3 -= sign_diff;
		
		mu2 = mu * mu;
		a0 = s3 - s2 - s0 + s1;
		a1 = s0 - s1 - a0;
		a2 = s2 - s0;
		a3 = s1;
		
		res = a0*mu*mu2 + a1*mu2 + a2*mu + a3;
		int resi = ((int)floor(res+0.5));
		
		if( bit16 )
		{
			if( resi < -32768 ) resi = -32768;
			if( resi > 32767 ) resi = 32767;
			dst16[i] = resi + 32768;
		}
		else
		{
			if( resi < -128 ) resi = -128;
			if( resi > 127 ) resi = 127;
			dst8[i] = resi + 128;
		}
		
	}
	
	free( samp->data );
	if( bit16 )
		samp->data = (void*)dst16;
	else
		samp->data = (void*)dst8;
	
	samp->sample_length = newsize;
	samp->loop_end = newsize;
	samp->loop_start = (int)(((double)samp->loop_start * (double)newsize+((double)oldlength/2))/(double)oldlength);
	samp->frequency = (int)(((double)samp->frequency * (double)newsize+((double)oldlength/2))/(double)oldlength);
}

void Sample_8bit( Sample* samp )
{
	if( samp->format & SAMPF_16BIT )
	{
		u8* newdata;
		u32 t;
		newdata = (u8*)malloc( samp->sample_length );
		for( t = 0; t < samp->sample_length; t++ )
			newdata[t] = ((u16*)samp->data)[t] / 256;
		free( samp->data );
		samp->data = newdata;
//		samp->bit16=false;
		samp->format &= ~SAMPF_16BIT;
	}
}

void Sample_Sign( Sample* samp )
{
	// sample must be unsigned
	u32 x;
	if( samp->format & SAMPF_16BIT )
	{
		for( x =0 ; x < samp->sample_length; x++ )
		{
			int a =  (( (int) ((u16*)samp->data)[x] ) - 32768);
			if(a < -32767) a = -32767;						// clamp LOW to -32767 (leave space for interpolation error)
//			if(a > 32765)  a = 32765;						// clamp HIGH to 32766
			((u16*)samp->data)[x] = (u16)a ;
		}
	}
	else
	{
		for( x =0 ; x < samp->sample_length; x++ )
		{
			int a =  (( (int) ((u8*)samp->data)[x] ) - 128);
			if( a == -128 ) a = -127;
			((u8*)samp->data)[x] = (u8) a;
		}

	}
	samp->format |= SAMPF_SIGNED;
}

void FixSample_GBA( Sample* samp )
{
	// convert to 8-bit if neccesary
	Sample_8bit( samp );
	
	// delete data after loop_end if loop exists
	if( samp->loop_type != 0 )
		samp->sample_length = samp->loop_end;
	
	// unroll BIDI loop
	if( samp->loop_type == 2 )
		Unroll_BIDI_Sample( samp );

	if( samp->loop_type )
	{
		if( samp->loop_end-samp->loop_start < GBA_MIN_LOOP_SIZE )
		{
			Unroll_Sample_Loop( samp, (GBA_MIN_LOOP_SIZE / (samp->loop_end-samp->loop_start))+1 );
		}
	}
}

int strcmpshit( char* str1, char* str2 )
{
	int x=0;
	int f=0;
	while( str1[x] != 0 )
	{
		if( str1[x] == str2[f] )f++;
		else					f=0;
		if( str2[f] == 0 )		return 1;
		x++;
	}
	return 0;
}

void FixSample_NDS( Sample* samp )
{
	if( samp->sample_length == 0 )
	{
		// sample has no data
		samp->loop_end=samp->loop_start=0;
		return;
	}
	// delete data after loop_end if loop exists
	if( samp->loop_type != 0 )
		samp->sample_length = samp->loop_end;
	
	// unroll BIDI loop
	if( samp->loop_type == 2 )
		Unroll_BIDI_Sample( samp );

	// %o option
	if( samp->loop_type )
	{
		if( !mm_ignore_sflags )
		{
			if( ((strcmpshit( samp->name, "%o"  )) > 0) )
			{
				Unroll_Sample_Loop( samp, 1 );
				samp->loop_start += (samp->loop_end-samp->loop_start) / 2;
			}
		}
	}

	if( !mm_ignore_sflags )
	{
		if( ((strcmpshit( samp->name, "%c" )) > 0) )
		{
			samp->format |= SAMPF_COMP;
		}
	}
	
	// Resize loop
	if( samp->loop_type )
	{
		int looplen = samp->loop_end-samp->loop_start;
		if( !(samp->format & SAMPF_COMP) )
		{
			if( samp->format & SAMPF_16BIT )
			{
				if( looplen & 1 )
				{
					int addition = (samp->loop_end - samp->loop_start);
					if( addition > MAX_UNROLL_THRESHOLD )
						Resample( samp, samp->sample_length +1 );
					else
						Unroll_Sample_Loop( samp, 1 );
				}
			}
			else
			{
				if( looplen & 3 )
				{
					int count;
					int addition;
					count = looplen & 3;
					switch( count ) {
					case 0:
						count=0;	break;
					case 1:
						count=3;	break;
					case 2:
						count=1;	break;
					case 3:
						count=3;	break;
					}
					addition = looplen*count;
					if( addition > MAX_UNROLL_THRESHOLD )
						Resample( samp, samp->sample_length + (4-(looplen & 3)) );
					else
						Unroll_Sample_Loop( samp, count );
				}
			}
		}
		else
		{
			int a = looplen;
			int count=0, addition;
			while( looplen & 7 )
			{
				count++;
				looplen += a;
			}
			addition = looplen*count;
			if( addition > MAX_UNROLL_THRESHOLD )
				Resample( samp, samp->sample_length + (4-(looplen & 7)) );
			else
				Unroll_Sample_Loop( samp, count );
		}
	}
	
	// Align loop_start
	if( samp->loop_type )
	{
		int padsize;
		if( !(samp->format & SAMPF_COMP) )
		{
			if( samp->format & SAMPF_16BIT ) {
				padsize = ( (2 - (samp->loop_start & 1)) & 1 );
			} else {
				padsize = ( (4 - (samp->loop_start & 3)) & 3 );
			}
		}
		else
		{
			padsize = ( (8 - (samp->loop_start & 7)) & 7 );
		}
		Sample_PadStart( samp, padsize );
	}
	
	// Pad end, only happens when loop is disabled
	if( !(samp->format & SAMPF_COMP) )
	{
		if( samp->format & SAMPF_16BIT )
		{
			if( samp->sample_length & 1 )
			{
				Sample_PadEnd( samp, 2-(samp->sample_length&1) );
			}
		}
		else
		{
			if( samp->sample_length & 3 )
			{
				Sample_PadEnd( samp, 4-(samp->sample_length&3) );
			}
		}
	}
	else
	{
		if( samp->sample_length & 7 )
		{
			Sample_PadEnd( samp, 8-(samp->sample_length&7) );
		}
	}
	
	Sample_Sign( samp );	// DS hardware takes signed samples

	if( samp->format & SAMPF_COMP )
	{
		// compress with IMA-ADPCM hunger owned
		adpcm_compress_sample( samp );
	}
	else
	{
		
	}
}

void FixSample( Sample* samp )
{
	// Clamp loop_start and loop_end (f.e. FR_TOWER.MOD)
	samp->loop_start = MIN(samp->loop_start, samp->sample_length);
	samp->loop_end = MIN(samp->loop_end, samp->sample_length);

	if( mm_target_system == SYSTEM_GBA )
		FixSample_GBA( samp );
	else if( mm_target_system == SYSTEM_NDS )
		FixSample_NDS( samp );
}

