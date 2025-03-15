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

// information from FMODDOC.TXT by FireLight

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "mas.h"
#include "mod.h"
#include "xm.h"
#include "files.h"
#include "simple.h"
#include "errors.h"
#include "samplefix.h"

#ifdef SUPER_ASCII
#define vstr_mod_div "────────────────────────────────────────────\n"
#define vstr_mod_samp_top		"┌─────┬──────┬─────┬──────┬───────┬───────────────────────┐\n"
#define vstr_mod_samp_header	"│INDEX│LENGTH│LOOP │VOLUME│ MID-C │ NAME                  │\n"
#define vstr_mod_samp_slice		"├─────┼──────┼─────┼──────┼───────┼───────────────────────┤\n"
#define vstr_mod_samp			"│ %2i  │%5i │ %3s │ %3i%% │ %ihz│ %-22s│\n"
#define vstr_mod_samp_bottom	"└─────┴──────┴─────┴──────┴───────┴───────────────────────┘\n"
#define vstr_mod_pattern " \x0e %2i%s"
#else
#define vstr_mod_div "--------------------------------------------\n"
#define vstr_mod_samp_top		vstr_mod_div
#define vstr_mod_samp_header	" INDEX LENGTH LOOP  VOLUME  MID-C   NAME                   \n"
//#define vstr_mod_samp_slice		""
#define vstr_mod_samp			" %-2i    %-5i  %-3s   %3i%%    %ihz  %-22s \n"
#define vstr_mod_samp_bottom	vstr_mod_div
#define vstr_mod_pattern " * %2i%s"
#endif

#ifdef _MSC_VER
static double round( double value )
{
	return floor(value+0.5);
}
#endif

int Create_MOD_Instrument( Instrument* inst, u8 sample )
{
	int x;
	memset( inst, 0, sizeof( Instrument ) );
	inst->global_volume = 128;
	// setup notemap
	for( x = 0; x < 120; x++ )
	{
		inst->notemap[x] = x | ((sample+1)<<8);
	}
	return ERR_NONE;
}

int Load_MOD_SampleData( Sample* samp )
{
	u32 t;
	if( samp->sample_length > 0 )
	{
		samp->data = (u8*)malloc( samp->sample_length ); // allocate a SAMPLE_LENGTH sized pointer to buffer in memory and load the sample into it
		for( t = 0; t < samp->sample_length; t++ )
		{
			((u8*)samp->data)[t] = mm_read8() + 128; // unsign data
		}
	}
	FixSample( samp );
	return ERR_NONE;
}

int Load_MOD_Pattern( Pattern* patt, u8 nchannels, u8* inst_count )
{
	u8 data1;
	u8 data2;
	u8 data3;
	u8 data4;

	u16 period;
	u8 inst;
	u8 effect;
	u8 param;

	u32 row;
	u32 col;

	PatternEntry* p;

	memset( patt, 0, sizeof( Pattern ) );
	patt->nrows = 64;						// MODs have fixed 64 rows per pattern

	for( row = 0; row < 64*MAX_CHANNELS; row++ )
	{
		patt->data[row].note = 250;
	}
	
	for( row = 0; row < 64; row++ )
	{
		for( col = 0; col < nchannels; col++ )
		{
			data1 = mm_read8();	// +-------------------------------------+
			data2 = mm_read8();	// | Byte 0    Byte 1   Byte 2   Byte 3  |
			data3 = mm_read8();	// |-------------------------------------|
			data4 = mm_read8();	// |aaaaBBBB CCCCCCCCC DDDDeeee FFFFFFFFF|
								// +-------------------------------------+

			period = (data1&0xf)*256 + (data2);	// BBBBCCCCCCCC = sample period value
			inst = (data1&0xF0)+(data3>>4);		// aaaaDDDD     = sample number
			effect = data3&0xF;					// eeee         = effect number
			param = data4;						// FFFFFFFF     = effect parameters
			
			// fix parameter for certain MOD effects
			switch( effect )
			{
			case 5:	// 5xy glis+slide
			case 6:	// 6xy vib+slide
				if( param & 0xF0 )	// clear Y if X
					param &= 0xF0;
			}
			
			p = &patt->data[row*MAX_CHANNELS+col];	// copy data to pattern entry
			p->inst = inst;							// ...
			CONV_XM_EFFECT( &effect, &param );		//  ...
			p->fx = effect;							//   ...
			p->param = param;						//    ...
			
			if( period != 0 )						// 0 = no note, otherwise calculate note value from the amiga period
				p->note = (int)round(12.0*log( (856.0)/(double)period )/log(2)) + 37 + 11;
			if( *inst_count < (inst + 1) )
			{
				*inst_count = (inst + 1);
				if( *inst_count > 31 )
					*inst_count = 31;
			}
		}
	}
	return ERR_NONE;
}

int Load_MOD_Sample( Sample* samp, bool verbose, int index )
{
	int finetune;
	int x;

	memset( samp, 0, sizeof( Sample ) );
	samp->msl_index = 0xFFFF;
	for( x = 0; x < 22; x++ )
		samp->name[x] = mm_read8();									// read in 22 bytes, store as SAMPLE_NAME
	for( x = 0; x < 12; x++ )				// copy to filename
		samp->filename[x] = samp->name[x];
	samp->sample_length = (mm_read8()*256 + mm_read8()) * 2;				// read in 2 bytes (word), store as SAMPLE_LENGTH
	finetune = mm_read8();												// read in 1 byte,         store as FINE_TUNE
	if( finetune >= 8 ) finetune -= 16;
	
	samp->default_volume = mm_read8();									// read in 1 byte,         store as VOLUME
	samp->loop_start = (mm_read8()*256 + mm_read8()) * 2;					// read in 2 bytes (word), store as LOOP_START
	samp->loop_end = samp->loop_start + (mm_read8()*256+mm_read8())*2;	// read in 2 bytes (word), store as LOOP_LENGTH

	// IS THIS WRONG?? :
	samp->frequency = (int)(8363.0 * pow( 2.0, ((double)finetune) * (1.0/192.0) ) );	// calculate frequency...

	samp->global_volume=64;											// max global volume
	if( (samp->loop_end - samp->loop_start) <= 2 )					// if loop length <= 2 then disabled loop
	{
		samp->loop_type = samp->loop_start = samp->loop_end = 0;
	}
	else
	{
		samp->loop_type = 1;											// otherwise enable
	}
	if( verbose )
	{
		if( samp->sample_length != 0 )
		{
			//printf( "%i	%s	%i%%	%ihz\n", samp->sample_length, samp->loop_type != 0 ? "Yes" : "No", (samp->default_volume*100)/64, samp->frequency );
			printf( vstr_mod_samp, index, samp->sample_length, samp->loop_type != 0 ? "Yes" : "No", (samp->default_volume*100)/64, samp->frequency, samp->name );
			/*
			printf( "  Length......%i\n", samp->sample_length );
			if( samp->loop_type != 0 )
			{
				printf( "  Loop........%i->%i\n", samp->loop_start, samp->loop_end );
			}
			else
			{
				printf( "  Loop........None\n" );
			}
			printf( "  Volume......%i\n", samp->default_volume );
			printf( "  Middle C....%ihz\n", samp->frequency );*/
		}
		else
		{
			//printf( "---\n" );
		}
	}
	return ERR_NONE;
}

int Load_MOD( MAS_Module* mod, bool verbose )
{
	u32 file_start;
	u32 mod_channels;
	int x;

	int npatterns;

	u32 sig;
	char sigs[5];

	if( verbose )
		printf( "Loading MOD, " );

	memset( mod, 0, sizeof( MAS_Module ) );
	file_start = mm_file_tell_read();
	mm_file_seek_read( 0x438, SEEK_SET );	// Seek to offset 1080 (438h) in the file
	sig = mm_read32();						// read in 4 bytes
	sigs[0]=sig&0xFF;
	sigs[1]=(sig>>8)&0xFF;
	sigs[2]=(sig>>16)&0xFF;
	sigs[3]=(sig>>24);
	sigs[4]=0;
	switch( sig )
	{
	case 'NHC1':
		mod_channels=1;
		break;
	case 'NHC2':
		mod_channels=2;
		break;
	case 'NHC3':
		mod_channels=3;
		break;
	case '.K.M':						// compare them to "M.K."  - if true we have a 4 channel mod
	case 'NHC4':
		mod_channels=4;
		break;
	case 'NHC5':
		mod_channels=5;
		break;
	case 'NHC6':
		mod_channels=6;					// compare them to "6CHN"  - if true we have a 6 channel mod
		break;
	case 'NHC7':
		mod_channels=7;
		break;
	case 'NHC8':						// compare them to "8CHN"  - if true we have an 8 channel mod
		mod_channels=8;
		break;
	case 'NHC9':
		mod_channels=9;
		break;
	default:
		if( sig>>16 == 'HC' )			// There are also rare tunes that use **CH where ** = 10-32 channels
		{
			char chn_number[3];
			chn_number[0] = (char)(sig&0xFF);
			chn_number[1] = (char)((sig>>8)&0xFF);
			chn_number[2] = 0;
			mod_channels = atoi( chn_number );
			if( mod_channels > MAX_CHANNELS  )
				return ERR_MANYCHANNELS;
		}
		else
		{
			return ERR_INVALID_MODULE;	// otherwise exit and display error message.
		}
	}

	mm_file_seek_read( file_start, SEEK_SET );	// - Seek back to position 0, the start of the file
	for( x = 0; x < 20; x++ )
		mod->title[x] = mm_read8();			// - read in 20 bytes, store as MODULE_NAME.

	if( verbose )
	{
		printf( "\"%s\"\n", mod->title );
		printf( "%i channels (%s)\n", mod_channels, sigs );
	}
	
	for( x = 0; x < MAX_CHANNELS; x++ )
	{
		if( (x&3)!=1 && (x&3) != 2 )
			mod->channel_panning[x] = clamp_u8( 128 - (MM_PANNING_SEP/2) );
		else
			mod->channel_panning[x] = clamp_u8( 128 + (MM_PANNING_SEP/2) );
		mod->channel_volume[x] = 64;
	}
	
	// set MOD settings
	mod->freq_mode = 0;
	mod->global_volume = 64;
	mod->initial_speed = 6;
	mod->initial_tempo = 125;
	mod->inst_count = 0; // filled in by Load_MOD_Pattern
	mod->inst_mode = false;
	mod->instruments = (Instrument*)malloc( 31 * sizeof( Instrument ) );
	mod->link_gxx = false;
	mod->old_effects = true;
	mod->restart_pos = 0;
	mod->samp_count = 0; // filled in before Load_MOD_SampleData
	mod->samples = (Sample*)malloc( 31 * sizeof( Sample ) );
	mod->stereo = true;
	mod->xm_mode = true;
	mod->old_mode=true;
	
	if( verbose )
	{
		printf( vstr_mod_div );
		printf( "Loading Samples...\n" );
		printf( vstr_mod_samp_top );
		printf( vstr_mod_samp_header );
#ifdef vstr_mod_samp_slice
		printf( vstr_mod_samp_slice );
#endif
	}
	// Load Sample Information
	for( x = 0; x < 31; x++ )
	{
	//	if( verbose )
			//printf( "Loading Sample %i...\n", x+1 );
		Create_MOD_Instrument( &mod->instruments[x], (u8)x );
		Load_MOD_Sample( &mod->samples[x], verbose, x );
	}
	
	// read sequence
	mod->order_count = mm_read8();	// read a byte, store as SONG_LENGTH (this is the number of orders in a song)
	mod->restart_pos = mm_read8();	// read a byte, discard it (this is the UNUSED byte - used to be used in PT as the restart position, but not now since jump to pattern was introduced)
	if( mod->restart_pos >= 127 )
		mod->restart_pos=0;
	npatterns=0;				// set NUMBER_OF_PATTERNS to equal 0......... or -1 :)
	for( x = 0; x < 128; x++ )	// from this point, loop 128 times
	{
		mod->orders[x] = mm_read8();			// read 1 byte, store it as ORDER <loopcounter>
		if( mod->orders[x] >= npatterns )	// if this value was bigger than NUMBER_OF_PATTERNS then set it to that value.
			npatterns=mod->orders[x]+1;
	}
	
	mm_read32();								// read 4 bytes, discard them (we are at position 1080 again, this is M.K. etc!)
	mod->patt_count = npatterns;
	mod->patterns = (Pattern*)malloc( mod->patt_count * sizeof( Pattern ) );	// allocate patterns

	if( verbose )
	{
		printf( vstr_mod_samp_bottom );
		printf( "Sequence has %i entries.\n", mod->order_count );
		printf( "Module has %i pattern%s.\n", mod->patt_count, mod->patt_count==1?"":"s" );
		printf( vstr_mod_div );
		printf( "Loading Patterns...\n" );
		printf( vstr_mod_div );
	}
	
	// Load Patterns
	for( x = 0; x < mod->patt_count; x++ )
	{
		if( verbose )
		{
			printf( vstr_mod_pattern, x+1, ((x+1)%15)?"":"\n" );
		}
		Load_MOD_Pattern( &mod->patterns[x], (u8)mod_channels, &(mod->inst_count) );
	}
	if( verbose )
	{
		printf( "\n" );
		printf( vstr_mod_div );
	}
	// Load Sample Data
	if( verbose )
		printf( "Loading Sample Data...\n" );
	mod->samp_count = mod->inst_count;
	for( x = 0; x < 31; x++ )
	{
		Load_MOD_SampleData( &mod->samples[x] );
	}
	if( verbose )
		printf( vstr_mod_div );
	return ERR_NONE;
}
