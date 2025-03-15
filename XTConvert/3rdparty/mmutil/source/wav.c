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

// WAV file loader

#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "files.h"
#include "mas.h"
#include "wav.h"
#include "simple.h"
#include "samplefix.h"

int Load_WAV( Sample* samp, bool verbose, bool fix )
{
	unsigned int mm_file_size;
	unsigned int bit_depth = 8;
	unsigned int hasformat = 0;
	unsigned int hasdata = 0;
	unsigned int chunk_code;
	unsigned int chunk_size;
	unsigned int num_channels = 0;
	unsigned int smpl_chunk_pos = 0;
	
	if( verbose )
		printf( "Loading WAV file...\n" );
	
	// initialize data
	memset( samp, 0, sizeof( Sample ) );
	
	mm_file_size = mm_file_tell_size();
	
	mm_read32();						// "RIFF"
	mm_read32();						// filesize-8
	mm_read32();						// "WAVE"
	
	while( 1 )
	{
		// break on end of file
		if( mm_file_tell_read() >= mm_file_size ) break;
		
		// read chunk code and length
		chunk_code = mm_read32();
		chunk_size = mm_read32();
		
		// parse chunk code
		switch( chunk_code )
		{
		//---------------------------------------------------------------------
		case ' tmf':	// format chunk
		//---------------------------------------------------------------------
			
			// check compression code (1 = PCM)
			if( mm_read16() != 1 )
			{
				if( verbose )
					printf( "Unsupported WAV format.\n" );
				return LOADWAV_UNKNOWN_COMP;
			}
			
			// read # of channels
			num_channels = mm_read16();
			
			// read sampling frequency
			samp->frequency = mm_read32();
			
			// skip average something, wBlockAlign
			mm_read32();
			mm_read16();
			
			// get bit depth, catch unsupported values
			bit_depth = mm_read16();
			if( bit_depth != 8 && bit_depth != 16 )
			{
				if( verbose )
					printf( "Unsupported bit-depth.\n" );
				return LOADWAV_UNSUPPORTED_BD;
			}
			
			if( bit_depth == 16 )
				samp->format |= SAMPF_16BIT;
			
			// print verbose data
			if( verbose )
			{
				printf( "Sample Rate...%i\n", samp->frequency );
				printf( "Bit Depth.....%i-bit\n", bit_depth );
			}
			
			// skip the rest of the chunk (if any)
			if( (chunk_size - 0x10) > 0 )
				mm_skip8( (chunk_size - 0x10) );
			
			hasformat = 1;
			break;
			
		//---------------------------------------------------------------------
		case 'atad':	// data chunk
		//---------------------------------------------------------------------
		{
			if( !hasformat )
			{
				return LOADWAV_CORRUPT;
			}
			
			if( verbose )
				printf( "Loading Sample Data...\n" );
			
			// clip chunk size against end of file (for some borked wavs...)
			{
				unsigned int br = mm_file_size - mm_file_tell_read();
				chunk_size = chunk_size > br ? br : chunk_size;
			}
			
			samp->sample_length = chunk_size / (bit_depth/8) / num_channels;
			samp->data = malloc( chunk_size );
			
			// read sample data
			for( unsigned int t = 0; t < samp->sample_length; t++ )
			{
				int dat = 0;
				
				// for multi-channel samples, get average value
				for( unsigned int c = 0; c < num_channels; c++ )
				{
					dat += bit_depth == 8 ? ((int)mm_read8()) - 128 : ((short)mm_read16());
				}
				dat /= num_channels;
				
				if( bit_depth == 8 )
				{
					((u8*)samp->data)[t] = dat + 128;
				}
				else
				{
					((u16*)samp->data)[t] = dat + 32768;
				}
			}
			
			hasdata = 1;
			
			break;
		}
		//------------------------------------------------------------------------------
		case 'lpms':	// sampler chunk
		//------------------------------------------------------------------------------
		{
			smpl_chunk_pos = mm_file_tell_read();
			mm_skip8( chunk_size );
			break;
		}	
		default:
			mm_skip8( chunk_size );
		}
	}
	
	// sampler chunk is processed last because it depends on the sample length being known.
	if ( smpl_chunk_pos )
	{
		mm_file_seek_read( smpl_chunk_pos, SEEK_SET );
		
		mm_skip8( 	4		// manufacturer
				+4		// product
				+4		// sample period
				+4		// midi unity note
				+4		// midi pitch fraction
				+4		// smpte format
				+4		// smpte offset
				);
		
		int num_sample_loops = mm_read32();
		
		mm_read32();		// sample data
		
		// check for sample looping data
		if( num_sample_loops )
		{
			mm_read32();	// cue point ID
			int loop_type = mm_read32();
			
			if( loop_type < 2 )
			{
				// sample    | internal
				// 0=forward | 1
				// 1=bidi    | 2
				samp->loop_type = loop_type + 1;
				samp->loop_start = mm_read32();
				samp->loop_end = mm_read32();
				
				// clip loop start against sample length
				if( samp->loop_end > samp->sample_length ) {
					samp->loop_end = samp->sample_length;
				}
				
				// disable tiny loop
				// catch invalid loop
				if( (samp->loop_start > samp->sample_length) ||
					(samp->loop_end - samp->loop_start < 16) ) {
					samp->loop_type = 0;
					samp->loop_start = 0;
					samp->loop_end = 0;
				}
				
				// ignore fractional
				// ignore play count
			}
		}
	}
	
	if( hasformat && hasdata )
	{
		if( fix ) FixSample( samp );
		return LOADWAV_OK;
	}
	else
	{
		return LOADWAV_CORRUPT;
	}
}
