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

// information from ST3's TECH.DOC

#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "mas.h"
#include "s3m.h"
#include "files.h"
#include "simple.h"
#include "errors.h"
#include "samplefix.h"

#define S3M_NOTE(a) (((a&15)+(a>>4)*12)+12)

#ifdef SUPER_ASCII
#define vstr_s3m_samp " %5i │ %-4s│ %3i%% │%5ihz│ %-28s│\n"
#define vstr_s3m_sampe " ----- │ --- │ ---- │ ----- │ %-28s│\n"
#define vstr_s3m_div "────────────────────────────────────────────\n"
#define vstr_s3m_sampt_top   "┌─────┬───────┬─────┬──────┬───────┬─────────────────────────────┐\n"
#define vstr_s3m_sampt_mid   "│INDEX│LENGTH │LOOP │VOLUME│ MID-C │             NAME            │\n"
#define vstr_s3m_sampt_slice "├─────┼───────┼─────┼──────┼───────┼─────────────────────────────┤\n"
#define vstr_s3m_sampt_index "│ %2i  │"
#define vstr_s3m_sampt_bottom "└─────┴───────┴─────┴──────┴───────┴─────────────────────────────┘\n"
#define vstr_s3m_pattern " \x0e %2i%s"
#else
#define vstr_s3m_samp "%-5i   %-3s   %3i%%   %5ihz  %-28s \n"
#define vstr_s3m_sampe   "-----   ---   ----   -------  %-28s\n"
#define vstr_s3m_div "--------------------------------------------\n"
#define vstr_s3m_sampt_top  vstr_s3m_div
#define vstr_s3m_sampt_mid   " INDEX LENGTH  LOOP  VOLUME  MID-C   NAME\n"
//#define vstr_s3m_sampt_slice "" 
#define vstr_s3m_sampt_index " %-2i    "
#define vstr_s3m_sampt_bottom vstr_s3m_div
#define vstr_s3m_pattern " * %2i%s"
#endif

int Load_S3M_SampleData( Sample* samp, u8 ffi )
{
	u32 x;
	int a;
	if( samp->sample_length == 0 )
		return ERR_NONE;
	if( samp->format & SAMPF_16BIT )
		samp->data = (u16*)malloc( samp->sample_length*2 );
	else
		samp->data = (u8*)malloc( samp->sample_length );
	if( ffi == 1 )
	{
		// signed samples [VERY OLD]
		for( x = 0; x < samp->sample_length; x++ )
		{
			if( samp->format & SAMPF_16BIT )
			{
				a = mm_read16();
				a += 32768;
				((u16*)samp->data)[x] = (u16)a;
			}
			else
			{
				a = mm_read8();
				a += 128;
				((u8*)samp->data)[x] = (u8)a;
			}
		}
	}
	else if( ffi == 2 )
	{
		// unsigned samples
		for( x = 0; x < samp->sample_length; x++ )
		{
			if( samp->format & SAMPF_16BIT )
			{
				a = mm_read16();
				((u16*)samp->data)[x] = (u16)a;
			}
			else
			{
				a = mm_read8();
				((u8*)samp->data)[x] = (u8)a;
			}
		}
	}
	else
	{
		return ERR_UNKNOWNSAMPLE;
	}
	FixSample( samp );
	return ERR_NONE;
}

int Load_S3M_Sample( Sample* samp, bool verbose )
{
	u8 flags;
	u32 x;
	memset( samp, 0, sizeof( Sample ) );
	samp->msl_index = 0xFFFF;
	if( mm_read8() == 1 )			// type, 1 = sample
	{
		for( x = 0; x < 12; x++ )
			samp->filename[x] = mm_read8();
		samp->datapointer = (mm_read8()*65536+mm_read16())*16;//mm_read24();
		samp->sample_length = mm_read32();
		samp->loop_start = mm_read32();
		samp->loop_end = mm_read32();
		samp->default_volume = mm_read8();
		samp->global_volume = 64;
		mm_read8(); // reserved
		if( mm_read8() != 0 )			// packing, 0 = unpacked
			return ERR_UNKNOWNSAMPLE;
		flags = mm_read8();
		samp->loop_type = flags&1 ? 1 : 0;
		if( flags & 2 )
			return ERR_UNKNOWNSAMPLE;
		//samp->bit16 = flags&4 ? true : false;
		samp->format = flags&4 ? SAMP_FORMAT_U16 : SAMP_FORMAT_U8;
		samp->frequency = mm_read32();
		mm_read32(); // reserved
		mm_skip8( 8 ); // internal variables
		for( x =0 ; x < 28; x++ )
			samp->name[x] = mm_read8();
		if( mm_read32() != 'SRCS' )
			return ERR_UNKNOWNSAMPLE;

		if( verbose )
		{
	//		printf( "────────────────────────────────────────────\n" );
	//		printf( "Loading Samples...\n" );
	//		printf( "┌─────┬──────┬────┬──────┬─────┬─────────────────────────────┐\n" );
	//		printf( "│LENGTH│LOOP│VOLUME│ MID-C │             NAME            │\n");
	//		printf( "┼──────┼────┼──────┼─────┼─────────────────────────────┤\n" );
			printf( vstr_s3m_samp, samp->sample_length, samp->loop_type ? "Yes" : "No", (samp->default_volume*100) / 64, samp->frequency, samp->name );
			/*printf( "  Name......%s\n", samp->name );
			printf( "  Length....%i\n", samp->sample_length );
			if( samp->loop_type )
				printf( "	 Loop......%i->%i\n", samp->loop_start, samp->loop_end );
			else
				printf( "  Loop......Disabled\n" );
			printf( "  Volume....%i\n", samp->default_volume );
			printf( "  Middle C..%ihz\n", samp->frequency );
			if( samp->bit16 )
				printf( "  16 bit....yes\n" );*/
		}
	}
	else
	{
		if( verbose )
		{
			printf( vstr_s3m_sampe, samp->name );
		}
	}
	return ERR_NONE;
}

int Load_S3M_Pattern( Pattern* patt  )
{
	int clength;
	int row, col;
	u8 what;
	int z;
	
	clength = mm_read16();
	// unpack s3m data
	
	memset( patt, 0, sizeof( Pattern ) );
	
	patt->clength = clength;
	patt->nrows = 64;
	
	for( row = 0; row < 64*MAX_CHANNELS; row++ )
	{
		patt->data[row].note = 250;
		patt->data[row].vol = 255;
	}
	
	for( row = 0; row < 64; row++ )
	{
		while( (what = mm_read8()) != 0 )	// BYTE:what / 0=end of row
		{
			col = what & 31;	// &31=channel

			z = row*MAX_CHANNELS+col;

			if( what & 32 )		// &32=follows;  BYTE:note, BYTE:instrument
			{
				patt->data[z].note = mm_read8();
				if( patt->data[z].note == 255 )
					patt->data[z].note = 250;
				else if( patt->data[z].note == 254 )
					patt->data[z].note = 254;
				else
					patt->data[z].note = S3M_NOTE( patt->data[z].note );
				patt->data[z].inst = mm_read8();
			}

			if( what & 64 )		// &64=follows;  BYTE:volume
			{
				patt->data[z].vol = mm_read8();
			}

			if( what & 128 )	// &128=follows; BYTE:command, BYTE:info
			{
				patt->data[z].fx = mm_read8();
				patt->data[z].param = mm_read8();
				if( patt->data[z].fx == 3 )		// convert pattern break to hexadecimal
				{
					patt->data[z].param = (patt->data[z].param&0xF) + (patt->data[z].param/16)*10;
				}
				if( patt->data[z].fx == 'X'-64 )
				{
					patt->data[z].param *= 2; // multiply volume scale by 2
				}
				if( patt->data[z].fx == 'V'-64 )
				{
					patt->data[z].param *= 2; // multiply volume scale by 2
				}
			}
			if( patt->data[z].fx   == 255 )
			{
				patt->data[z].fx = 0;
				patt->data[z].param = 0;
			}
		}
	}
	return ERR_NONE;
}

int Load_S3M( MAS_Module* mod, bool verbose )
{
	u16 s3m_flags;
	u16 cwt;
	u16 ffi;
	u8 dp;
	
	bool stereo;

	u8 a;
	bool chan_enabled[32];

	int x,y;

	u16* parap_inst;
	u16* parap_patt;

	

	memset( mod, 0, sizeof( MAS_Module ) );
	for( x = 0; x < 28; x++ )
		mod->title[x] = mm_read8();	// read song name

	if( mm_read8() != 0x1A )
	{
//		return ERR_INVALID_MODULE;
	}
	if( mm_read8() != 16 )
		return ERR_INVALID_MODULE;
	if( verbose )
	{
		printf( vstr_s3m_div );
	}
	if( verbose )
		printf( "Loading S3M, \"%s\"\n", mod->title );

	mm_skip8( 2 ); // reserved space
	mod->order_count = (u8)mm_read16();
	mod->inst_count = (u8)mm_read16();
	mod->samp_count = mod->inst_count;
	mod->patt_count = (u8)mm_read16();

	for( x = 0; x < 32; x++ )
		mod->channel_volume[x] = 64;

	mod->freq_mode = 0;		// amiga frequencies
	mod->old_effects=true;	// old effects (maybe not?)
	mod->link_gxx=false;	// dont link gxx memory
	mod->restart_pos = 0;	// restart from beginning
	mod->old_mode=true;
	
	s3m_flags = mm_read16();
	(void)s3m_flags; // unused for now
	cwt = mm_read16();
	(void)cwt; // unused for now
	ffi = mm_read16();
	if( mm_read32() != 'MRCS' ) // "SCRM" mark
		return ERR_INVALID_MODULE;
	mod->global_volume = mm_read8()*2;
	mod->initial_speed = mm_read8();
	mod->initial_tempo = mm_read8();
	stereo = mm_read8() >> 7; // master volume
	mm_read8(); // ultra click removal
	dp = mm_read8(); // default pan positions (when 252)
	mm_skip8( 8+2 ); // reserved space + special pointer
	for( x = 0; x < 32; x++ )
	{
		u8 chn = mm_read8();
		chan_enabled[x] = chn >> 7;
		if( stereo )
		{
			if( (chn&127) < 8 )	// left channel
				mod->channel_panning[x] = clamp_u8( 128 - (MM_PANNING_SEP/2) );
			else // right channel
				mod->channel_panning[x] = clamp_u8( 128 + (MM_PANNING_SEP/2) );
		}
		else
		{
			mod->channel_panning[x] = 128;
		}
	}
	(void)chan_enabled; // unused for now
	for( x = 0; x < mod->order_count; x++ )
	{
		mod->orders[x] = mm_read8();
	}
	parap_inst = (u16*)malloc( mod->inst_count * sizeof( u16 ) );
	parap_patt = (u16*)malloc( mod->patt_count * sizeof( u16 ) );
	
	for( x = 0; x < mod->inst_count; x++ )
		parap_inst[x] = mm_read16();
	for( x = 0; x < mod->patt_count; x++ )
		parap_patt[x] = mm_read16();
	
	if( dp == 252 )
	{
		for( x = 0; x < 32; x++ )
		{
			a = mm_read8();
			if( a & 32 )
			{
				mod->channel_panning[x] = (a&15)*16 > 255 ? 255 : (a&15)*16;
			}
			else
			{/*
				if( stereo )
				{
					switch( x & 3 ) {
					case 0:
					case 3:
						mod->channel_panning[x] = clamp_u8( 128 - (MM_PANNING_SEP/2) );
						break;
					case 1:
					case 2:
						mod->channel_panning[x] = clamp_u8( 128 + (MM_PANNING_SEP/2) );
					}
				}
				else
				{
					mod->channel_panning[x] = 128;
				}*/
			}
		}
	}
	else
	{
		for( x = 0; x < 32; x++ )
		{
			if( stereo )
				mod->channel_panning[x] = x & 1 ? clamp_u8( 128 - (MM_PANNING_SEP/2) ) : clamp_u8( 128 + (MM_PANNING_SEP/2) );
			else
				mod->channel_panning[x] = 128;
		}
	}
	
	mod->instruments = (Instrument*)malloc( mod->inst_count * sizeof( Instrument ) );
	mod->samples = (Sample*)malloc( mod->samp_count * sizeof( Sample ) );
	mod->patterns = (Pattern*)malloc( mod->patt_count * sizeof( Pattern ) );
	
	if( verbose )
	{
		printf( vstr_s3m_div );
		printf( "Loading Samples...\n" );
		printf( vstr_s3m_sampt_top );
		printf( vstr_s3m_sampt_mid );
#ifdef vstr_s3m_sampt_slice
		printf( vstr_s3m_sampt_slice );
#endif
	}
	// load instruments
	for( x = 0; x < mod->inst_count; x++ )
	{
		if( verbose )
		{
			printf( vstr_s3m_sampt_index, x+1 );
			//printf( "Sample %i\n", x+1 );
		}
		// create instrument for sample
		memset( &mod->instruments[x], 0, sizeof( Instrument ) );
		mod->instruments[x].global_volume = 128;
		// make notemap
		for( y = 0; y < 120; y++ )
			mod->instruments[x].notemap[y] = y | ((x+1) << 8);
		
		// load sample
		mm_file_seek_read( parap_inst[x]*16, SEEK_SET );
		if( Load_S3M_Sample( &mod->samples[x], verbose ) )
		{
			printf( "Error loading sample!\n" );
			return ERR_UNKNOWNSAMPLE;
		}
	}
	
	// load patterns
	if( verbose )
	{
		printf( vstr_s3m_sampt_bottom );
		printf( "Loading Patterns...\n" );
		printf( vstr_s3m_div );
	}
	for( x = 0; x < mod->patt_count; x++ )
	{
		if( verbose )
		{
			printf( vstr_s3m_pattern, x+1, ((x+1)%15)?"":"\n" );
		}
			//printf( "%i...", x+1 );
		mm_file_seek_read( parap_patt[x]*16, SEEK_SET );
		Load_S3M_Pattern( &mod->patterns[x] );
	}
	
	if( verbose )
	{
		printf( "\n" );
		printf( vstr_s3m_div );
		printf( "Loading Sample Data...\n" );
	}
	for( x = 0; x < mod->samp_count; x++ )
	{
		mm_file_seek_read( mod->samples[x].datapointer, SEEK_SET );
		Load_S3M_SampleData( &mod->samples[x], (u8)ffi );
	}
	if( verbose )
	{
		printf( vstr_s3m_div );
	}
	return ERR_NONE;
}
