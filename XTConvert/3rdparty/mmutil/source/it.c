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

// information from ITTECH.TXT

#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "mas.h"
#include "it.h"
#include "files.h"
#include "simple.h"
#include "errors.h"
#include "samplefix.h"

#ifdef SUPER_ASCII
#define vstr_it_div "────────────────────────────────────────────\n"
#define vstr_it_instr_top   "┌─────┬──────┬─────┬─────┬───────────────────────────┐\n"
#define vstr_it_instr_head  "│INDEX│VOLUME│ NNA │ ENV │            NAME           │\n"
#define vstr_it_instr_slice "├─────┼──────┼─────┼─────┼───────────────────────────┤\n"
#define vstr_it_instr		"│%3i  │ %3i%% │ %3s │ %s%s%s │ %-26s│\n"
#define vstr_it_instr_bottom "└─────┴──────┴─────┴─────┴───────────────────────────┘\n"

#define vstr_it_samp_top    "┌─────┬──────┬───────┬──────┬─────────┬───────────────────────────┐\n"
#define vstr_it_samp_head   "│INDEX│VOLUME│DVOLUME│ LOOP │  MID-C  │            NAME           │\n"
#define vstr_it_samp_slice  "├─────┼──────┼───────┼──────┼─────────┼───────────────────────────┤\n"
#define vstr_it_samp		"│%3i  │ %3i%% │ %3i%%  │ %4s │%6ihz │ %-26s│\n"
#define vstr_it_samp_bottom "└─────┴──────┴───────┴──────┴─────────┴───────────────────────────┘\n"

#define vstr_it_pattern " \x0e %2i"
#else
#define vstr_it_div "--------------------------------------------\n"
#define vstr_it_instr_top   vstr_it_div
#define vstr_it_instr_head  " INDEX VOLUME  NNA   ENV   NAME\n"
//#define vstr_it_instr_slice ""
#define vstr_it_instr		" %-3i   %3i%%    %3s   %s%s%s   %-26s \n"
#define vstr_it_instr_bottom vstr_it_div

#define vstr_it_samp_top    vstr_it_div
#define vstr_it_samp_head   " INDEX VOLUME DVOLUME LOOP   MID-C     NAME            \n"
//#define vstr_it_samp_slice  ""
#define vstr_it_samp		" %-3i   %3i%%   %3i%%    %4s  %6ihz   %-26s \n"
#define vstr_it_samp_bottom vstr_it_div

#define vstr_it_pattern " * %2i"
#endif

bool Load_IT_Envelope( Instrument_Envelope* env, bool unsign )
{
	// read envelopes
	u8 a;
	u8 node_count;
	int x;
	
	bool env_loop=false;
	bool env_sus=false;
	bool env_enabled=false;
	bool env_filter=false;

	memset( env, 0, sizeof( Instrument_Envelope ) );

	a=mm_read8();
	
	if( a & 1 )
		env_enabled = true;
	if( !(a & 2) )
	{
		env->loop_start=255;
		env->loop_end  =255;
	}
	else
		env_loop=true;
	if( !(a & 4) )
	{
		env->sus_start=255;
		env->sus_end=255;
	}
	else
		env_sus=true;

	if( a & 128 )
	{
		unsign=false;
		env_filter=true;
		env->env_filter=env_filter;
	}
	
	node_count=mm_read8();
	if( node_count != 0 )
		env->env_valid=true;
	
	env->node_count = node_count;
	if( env_loop )
	{
		env->loop_start=mm_read8();
		env->loop_end=mm_read8();
	}
	else
	{
		mm_skip8( 2 );
	}
	if( env_sus )
	{
		env->sus_start=mm_read8();
		env->sus_end=mm_read8();
	}
	else
	{
		mm_skip8( 2 );
	}
	for( x = 0; x < 25; x++ )
	{
		env->node_y[x] = mm_read8();
		if( unsign )
			env->node_y[x] += 32;
		env->node_x[x] = mm_read16();
		
	}
	mm_read8(); // unused byte
	env->env_enabled = env_enabled;
	return env_enabled;
}

int Load_IT_Instrument( Instrument* inst, bool verbose, int index )
{
	u16 a;
	int x;

	memset( inst, 0, sizeof( Instrument ) );

	mm_skip8( 17 );
	inst->nna		=mm_read8();
	inst->dct		=mm_read8();
	inst->dca		=mm_read8();
	a=mm_read16(); if( a > 255 ) a = 255;
	inst->fadeout	=(u8)a;
	mm_skip8( 2 );
	inst->global_volume		=mm_read8();
	a= mm_read8();
	a = (a&128) | ((a&127)*2 > 127 ? 127 : (a&127)*2);
	inst->setpan			=a^128;
	inst->random_volume		=mm_read8();
	mm_skip8( 5 );
	for( x = 0; x < 26; x++ )
		inst->name[x] = mm_read8();
	mm_skip8( 6 );
	
	for( x = 0; x < 120; x++ )
		inst->notemap[x] = mm_read16();
	
	inst->env_flags=0;
	
	Load_IT_Envelope( &inst->envelope_volume, false );
	inst->env_flags |= inst->envelope_volume.env_valid ? 1 : 0;
	inst->env_flags |= inst->envelope_volume.env_enabled ? 8 : 0;

	Load_IT_Envelope( &inst->envelope_pan, true );
	inst->env_flags |= inst->envelope_pan.env_enabled ? 2 : 0;

	Load_IT_Envelope( &inst->envelope_pitch, true );
	inst->env_flags |= inst->envelope_pitch.env_enabled ? 4 : 0;

	if( verbose )
	{
		printf( vstr_it_instr, \
				index+1, \
				(inst->global_volume*100)/128, \
				((inst->nna==0)?"CUT":((inst->nna==1)?"CON":((inst->nna==2)?"OFF":((inst->nna==3)?"FAD":"???")))), \
				(inst->env_flags&8)?"V":"-", \
				(inst->env_flags&2)?"P":"-", \
				(inst->env_flags&4)?"T":"-", \
				inst->name );

	/*	printf( "%i%%	", (inst->global_volume*100) / 128 );
		switch( inst->nna )
		{
		case 0:
			printf( "%s	", "CUT" ); break;
		case 1:
			printf( "%s	", "OFF" ); break;
		case 2:
			printf( "%s	", "CONT" ); break;
		case 3:
			printf( "%s	", "FADE" ); break;
		}
		if( (!(inst->env_flags & 2)) && (!(inst->env_flags & 4)) && (!(inst->env_flags & 8)) )
		{
			printf( "-	" );
		}
		else
		{
			if( inst->env_flags & 8)
				printf( "V" );
			if( inst->env_flags & 2)
				printf( "P" );
			if( inst->env_flags & 4)
				printf( "S" );
			printf( "	" );
		}
		printf( "%s\n", inst->name );*/
	}

	mm_skip8( 7 );
	return 0;
}

void Create_IT_Instrument( Instrument* inst, int sample )
{
	int x;

	memset( inst, 0, sizeof( Instrument ) );

	inst->global_volume		=128;
	
	for( x = 0; x < 120; x++ )
		inst->notemap[x] = x+sample*256;
}

int Load_IT_Sample( Sample* samp )
{
	bool bit16;
	bool hasloop;
	bool pingpong;
	bool samp_unsigned=false;
	u8 a;
	u32 samp_length;
	u32 loop_start;
	u32 loop_end;
	u32 c5spd;
	u32 data_address;
	int x;
	
	memset( samp, 0, sizeof( Sample ) );
	samp->msl_index = 0xFFFF;

	if( mm_read32() != 'SPMI' )
		return ERR_UNKNOWNSAMPLE;
	for( x = 0; x < 12; x++ )	// dos filename
		samp->filename[x] = mm_read8();
	if( mm_read8() != 0 )
		return ERR_UNKNOWNSAMPLE;
	samp->global_volume = mm_read8();
	a = mm_read8();
	samp->it_compression = a & 8 ? 1 : 0;
	bit16 = a & 2;
	hasloop = a & 16;
	pingpong = a & 64;
	samp->default_volume = mm_read8();
	for( x = 0; x < 26; x++ )
		samp->name[x] = mm_read8();
	a=mm_read8();
	samp->default_panning = mm_read8();
	samp->default_panning = (((samp->default_panning&127) == 64) ? 127 : (samp->default_panning<<1)) | (samp->default_panning&128);
	if( !(a & 1) )
		samp_unsigned=true;
	samp_length = mm_read32();
	loop_start=mm_read32();
	loop_end=mm_read32();
	c5spd=mm_read32();
	
	samp->frequency			= c5spd;
	samp->sample_length		= samp_length;
	samp->loop_start		= loop_start;
	samp->loop_end			= loop_end;
	
	mm_skip8( 8 ); // susloop start/end
	data_address = mm_read32();
	samp->vibspeed = mm_read8();
	samp->vibdepth = mm_read8();
	samp->vibrate = mm_read8();
	samp->vibtype = mm_read8();
	samp->datapointer = data_address;
	if( hasloop )
	{
		if( pingpong )
			samp->loop_type = 2;
		else
			samp->loop_type = 1;
		samp->loop_start = loop_start;
		samp->loop_end = loop_end;
	}
	else
	{
		samp->loop_type = 0;
	}
	samp->format = (bit16 ? SAMPF_16BIT : 0) | (samp_unsigned ? 0 : SAMPF_SIGNED );
	if( samp->sample_length == 0 ) samp->loop_type = 0;
	return 0;
}

int Load_IT_Sample_CMP( u8 *p_dest_buffer, int samp_len, u16 cmwt, bool bit16 );
int Load_IT_SampleData( Sample* samp, u16 cwmt )
{
	u32 x;
	int a;
	if( samp->sample_length == 0 ) return 0;
	if( samp->format & SAMPF_16BIT )
		samp->data = (u16*)malloc( (u32)(samp->sample_length)*2 );
	else
		samp->data = (u8*)malloc( (u32)(samp->sample_length) );

	if( !samp->it_compression )
	{
	
		for( x = 0; x < samp->sample_length; x++ )
		{
			if( samp->format & SAMPF_16BIT )
			{
				if( !(samp->format & SAMPF_SIGNED) )
				{
					a = (unsigned short)mm_read16();
				}
				else
				{
					a = (signed short)mm_read16();
					a += 32768;
				}
				((u16*)samp->data)[x] = (u16)a;
			}
			else
			{
				if( !(samp->format & SAMPF_SIGNED) )
				{
					a = (unsigned char)mm_read8();
				}
				else
				{
					a = (signed char)mm_read8();
					a += 128;
				}
				((u8*)samp->data)[x] = (u8)a;
			}
			
		}
	}
	else
	{
		Load_IT_Sample_CMP( samp->data, samp->sample_length, cwmt, (bool)(samp->format & SAMPF_16BIT) );
	}
	FixSample( samp );
	return 0;
}

int Empty_IT_Pattern( Pattern *patt ) {

	int x;
	memset( patt, 0, sizeof( Pattern ) );
	patt->nrows = 64;
	for( x = 0; x < patt->nrows*MAX_CHANNELS; x++ ) {
		patt->data[x].note = 250; // special clears for vol&note
		patt->data[x].vol = 255;
	}
	return ERR_NONE;
}

int Load_IT_Pattern( Pattern* patt )
{
	int x;
	int clength;
	u8 chanvar;
	u8 chan;
	u8 maskvar;

	u8 old_maskvar[MAX_CHANNELS];
	u8 old_note[MAX_CHANNELS];
	u8 old_inst[MAX_CHANNELS];
	u8 old_vol[MAX_CHANNELS];
	u8 old_fx[MAX_CHANNELS];
	u8 old_param[MAX_CHANNELS];
	
	memset( patt, 0, sizeof( Pattern ) );
	
	clength = mm_read16();
	patt->nrows = mm_read16();
	mm_skip8(4);

	patt->clength = clength;

	for( x = 0; x < patt->nrows*MAX_CHANNELS; x++ )
	{
		patt->data[x].note = 250; // special clears for vol&note
		patt->data[x].vol = 255;
	}
	
	
	// DECOMPRESS IT PATTERN
	
	for( x = 0; x < patt->nrows; x++ )
	{
GetNextChannelMarker:
		chanvar = mm_read8();					// Read byte into channelvariable.
		if( chanvar == 0 )					// if(channelvariable = 0) then end of row
			continue;					
		
		chan = (chanvar-1) & 63;			// Channel = (channelvariable-1) & 63
		if( chan >= MAX_CHANNELS )
			return ERR_MANYCHANNELS;
		
		if( chanvar & 128 )					// if(channelvariable & 128) then read byte into maskvariable
			old_maskvar[chan] = mm_read8();
		
		maskvar = old_maskvar[chan];
		
		if( maskvar & 1 )					// if(maskvariable & 1), then read note. (byte value)
		{
			old_note[chan] = mm_read8();
			patt->data[x*MAX_CHANNELS+chan].note = old_note[chan];
		}
		
		if( maskvar & 2 )					// if(maskvariable & 2), then read instrument (byte value)
		{
			old_inst[chan] = mm_read8();
			patt->data[x*MAX_CHANNELS+chan].inst = old_inst[chan];
		}
		
		if( maskvar & 4 )					// if(maskvariable & 4), then read volume/panning (byte value)
		{
			old_vol[chan] = mm_read8();
			patt->data[x*MAX_CHANNELS+chan].vol = old_vol[chan];
		}

		if( maskvar & 8 )					// if(maskvariable & 8), then read command (byte value) and commandvalue
		{
			old_fx[chan] = mm_read8();
			patt->data[x*MAX_CHANNELS+chan].fx = old_fx[chan];
			old_param[chan] = mm_read8();
			patt->data[x*MAX_CHANNELS+chan].param = old_param[chan];
		}

		if( maskvar & 16 )											// if(maskvariable & 16), then note = lastnote for channel
			patt->data[x*MAX_CHANNELS+chan].note = old_note[chan];
		if( maskvar & 32 )											// if(maskvariable & 32), then instrument = lastinstrument for channel
			patt->data[x*MAX_CHANNELS+chan].inst = old_inst[chan];
		if( maskvar & 64 )											// if(maskvariable & 64), then volume/pan = lastvolume/pan for channel
			patt->data[x*MAX_CHANNELS+chan].vol = old_vol[chan];
		if( maskvar & 128 )											// if(maskvariable & 128), then {
		{
			patt->data[x*MAX_CHANNELS+chan].fx = old_fx[chan];		// command = lastcommand for channel and
			patt->data[x*MAX_CHANNELS+chan].param = old_param[chan];// commandvalue = lastcommandvalue for channel
		}
		goto GetNextChannelMarker;
	}
	
	return ERR_NONE;
}

int Load_IT( MAS_Module* itm, bool verbose )
{
	u8 b;
	u16 w;
	int x;
	int cc;

	u16 cwt;
	u16 cmwt;
	
	u32* parap_inst;
	u32* parap_samp;
	u32* parap_patt;

	bool instr_mode;
	
	memset( itm, 0, sizeof( MAS_Module ) );

	if( mm_read32() != 'MPMI' )
		return ERR_INVALID_MODULE;
	for( x = 0; x < 28; x++ )
		itm->title[x] = mm_read8();
	itm->order_count	= (u16)mm_read16();
	itm->inst_count		= (u8)mm_read16();
	itm->samp_count		= (u8)mm_read16();
	itm->patt_count		= (u8)mm_read16();
	cwt = mm_read16();
	(void)cwt; // uncertain why unused
	cmwt = mm_read16(); // upward compatible
	//mm_skip8( 4 );	// created with tracker / upward compatible
	w = mm_read16(); // flags
	itm->stereo = w & 1;
	itm->inst_mode = instr_mode = w & 4;
	itm->freq_mode = w & 8;
	itm->old_effects = w & 16;
	itm->link_gxx = w & 32;
	mm_skip8( 2 ); // special
	itm->global_volume = mm_read8();
	mm_skip8( 1 ); // mix volume
	itm->initial_speed = mm_read8();
	itm->initial_tempo = mm_read8();

	if( verbose )
	{
		printf( vstr_it_div );
		printf( "Loading IT, \"%s\"\n", itm->title );
		printf( vstr_it_div );
		printf( "#Orders......%i\n", itm->order_count );
		printf( "#Instr.......%i\n", itm->inst_count );
		printf( "#Samples.....%i\n", itm->samp_count );
		printf( "#Patterns....%i\n", itm->patt_count );
		printf( "Stereo.......%s\n", itm->stereo ? "Yes" : "No" );
		printf( "Slides.......%s\n", itm->freq_mode ? "Linear" : "Amiga" );
		printf( "Old Effects..%s\n", itm->old_effects ? "Yes" : "No" );
		printf( "Global Vol...%i%%\n", (itm->global_volume*100)/128 );
		printf( "Speed........%i\n", itm->initial_speed );
		printf( "Tempo........%i\n", itm->initial_tempo );
		printf( "Instruments..%s\n", instr_mode ? "Yes" : "Will be supplied" );
		printf( vstr_it_div );
	}
	mm_skip8( 12 ); //	SEP, PWD, MSGLENGTH, MESSAGE OFFSET, [RESERVED]
	for( x = 0; x < 64; x++ )
	{
		b = mm_read8();
		if( x < MAX_CHANNELS )
			itm->channel_panning[x] = b*4 > 255 ? 255 : b*4;	// map 0->64 to 0->255
	}
	for( x = 0; x < 64; x++ )
	{
		b = mm_read8();
		if( x < MAX_CHANNELS )
			itm->channel_volume[x] = b;
	}
	for( x = 0; x < itm->order_count; x++ )
		itm->orders[x] = mm_read8();
	
	parap_inst = (u32*)malloc( itm->inst_count * sizeof( u32 ) );
	parap_samp = (u32*)malloc( itm->samp_count * sizeof( u32 ) );
	parap_patt = (u32*)malloc( itm->patt_count * sizeof( u32 ) );
	
	for( x = 0; x < itm->inst_count; x++ )
		parap_inst[x] = mm_read32();
	for( x = 0; x < itm->samp_count; x++ )
		parap_samp[x] = mm_read32();
	for( x = 0; x < itm->patt_count; x++ )
		parap_patt[x] = mm_read32();
	
	itm->samples = (Sample*)malloc( itm->samp_count * sizeof( Sample ) );
	itm->patterns = (Pattern*)malloc( itm->patt_count * sizeof( Pattern ) );

	if( instr_mode )
	{
		itm->instruments = (Instrument*)malloc( itm->inst_count * sizeof( Instrument ) );
		if( verbose )
		{
			printf( "Loading Instruments...\n" );
			printf( vstr_it_instr_top );
			printf( vstr_it_instr_head );
#ifdef vstr_it_instr_slice
			printf( vstr_it_instr_slice );
#endif
			//printf( "INDEX	VOLUME	NNA	ENV	NAME\n" );
		}
		
		// read instruments
		for( x = 0; x < itm->inst_count; x++ )
		{
		//	if( verbose )
		//		printf( "%i	", x+1 );
			mm_file_seek_read( parap_inst[x], SEEK_SET );
			Load_IT_Instrument( &itm->instruments[x], verbose, x );
		}

		if( verbose )
		{
			printf( vstr_it_instr_bottom );
		}
	}

	if( verbose )
	{
		printf( "Loading Samples...\n" );
		printf( vstr_it_samp_top );
		printf( vstr_it_samp_head );
#ifdef vstr_it_samp_slice
		printf( vstr_it_samp_slice );
#endif
		//printf( "INDEX	VOLUME	DVOLUME	LOOP	MID-C	NAME\n" );
	}
	
	// read samples
	for( x = 0; x < itm->samp_count; x++ )
	{
		mm_file_seek_read( parap_samp[x], SEEK_SET );
		Load_IT_Sample( &itm->samples[x] );
		if( verbose )
		{
			printf( vstr_it_samp, x+1, (itm->samples[x].global_volume * 100) / 64, (itm->samples[x].default_volume * 100) / 64, itm->samples[x].loop_type == 0 ? "None" : (itm->samples[x].loop_type == 1 ? "Forw" : "BIDI"), itm->samples[x].frequency, itm->samples[x].name );
			//printf( "%i	%i%%	%i%%	%s	%ihz	%s\n", x+1, (itm->samples[x].global_volume*100) / 64, (itm->samples[x].default_volume*100) / 64, itm->samples[x].loop_type == 0 ? "None" : (itm->samples[x].loop_type == 1 ? "Yes" : "BIDI"), itm->samples[x].frequency, itm->samples[x].name );
		}
	}

	if( verbose )
	{
		printf( vstr_it_samp_bottom );
	}

	if( !instr_mode )
	{
		if( verbose )
		{
			printf( "Adding Instrument Templates...\n" );
			printf( vstr_it_div );
		}
		itm->inst_count = itm->samp_count;
		itm->instruments = (Instrument*)malloc( itm->inst_count * sizeof( Instrument ) );
		cc=0;
		for( x = 0; x < itm->samp_count; x++ )
		{
			if( verbose )
			{
				printf( " * %2i", x+1 );
				cc++;
				if( cc == 15)
				{
					cc=0;
					printf("\n");
				}
			}
			Create_IT_Instrument( &itm->instruments[x], x+1 );
		}
		if( verbose )
		{
			if( cc != 0 )
				printf( (((x+1)%15)==0)?"":"\n" );
			printf( vstr_it_div );
		}
	}

	if(verbose)
	{
		printf( "Reading Patterns...\n" );
		printf( vstr_it_div );
	}

	// read patterns
	cc=0;
	for( x = 0; x < itm->patt_count; x++ )
	{
		mm_file_seek_read( parap_patt[x], SEEK_SET );
		if( parap_patt[x] != 0 )
		{
			if( verbose )
			{
				printf( vstr_it_pattern, x+1 );
				cc++;
				if( cc == 15 )
				{
					cc=0;
					printf("\n");
				}
			}
			Load_IT_Pattern( &itm->patterns[x] );
			
		}
		else
		{
			Empty_IT_Pattern( &itm->patterns[x] );
			//memset( &itm->patterns[x], 0, sizeof( Pattern ) );
		}
	}
	
	
	if( verbose )
	{
		if( cc != 0 )
			printf( "\n" );
		printf( vstr_it_div );
		printf( "Loading Sample Data...\n" );
	}
	// read sample data
	for( x = 0; x < itm->samp_count; x++ )
	{
		mm_file_seek_read( itm->samples[x].datapointer, SEEK_SET );
		Load_IT_SampleData( &itm->samples[x], cmwt );
	}

	if( verbose )
	{
		printf( vstr_it_div );
	}

	free( parap_inst );
	free( parap_samp );
	free( parap_patt );
	
	return ERR_NONE;
}

/* * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE

 -The following sample decompression code is based on CHIBITRACKER's code (http://chibitracker.berlios.de) which is based on xmp's code.(http://xmp.helllabs.org) which is based in openCP code.

* NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE */

int Load_IT_CompressedSampleBlock( u8** buffer )
{
	u32 size;
	u32 x;
	size = mm_read16();
	(*buffer) = (u8*)malloc( size+4 );
	(*buffer)[size+0]=0;
	(*buffer)[size+1]=0;
	(*buffer)[size+2]=0;
	(*buffer)[size+3]=0;
	for( x = 0; x < size; x++ )
		(*buffer)[x] = mm_read8();
	return ERR_NONE;
}

int Load_IT_Sample_CMP( u8 *p_dest_buffer, int samp_len, u16 cmwt, bool bit16 )
{
	u8*	c_buffer=NULL;
   	u16 block_length;		// length of compressed data block in samples
	u16 block_position;		// position in block 
	u8 bit_width;			// actual "bit width" 
	u32 aux_value;			// value read from file to be processed 
	s16 d1, d2;		// integrator buffers (d2 for it2.15) 
	s8 d18, d28;
	s8 v8;			// sample value 
	s16 v16;		// sample value 16 bit
	bool it215; // is this an it215 module?
	u16 border;
	u8 tmp_shift;
	u32 bit_readpos=0;
	int i;
	
	u32 nbits, dsize;

	u8 *dest8_write = (u8 *)p_dest_buffer;
	u16 *dest16_write = (u16 *)p_dest_buffer;

	nbits = bit16 ? 16 : 8;
	dsize = bit16 ? 4 : 3;
	for (i=0;i<samp_len;i++)
		p_dest_buffer[i]=128;
	


	it215=(cmwt==0x215);

	// now unpack data till the dest buffer is full 

	while( samp_len )
	{
	// read a new block of compressed data and reset variables 
		Load_IT_CompressedSampleBlock( &c_buffer );
		bit_readpos=0;
		if( bit16 )
			block_length = (samp_len < 0x4000) ? samp_len : 0x4000;
		else
			block_length = (samp_len < 0x8000) ? samp_len : 0x8000;
		block_position = 0;
		bit_width = nbits+1;		// start with width of 9 bits 
		d1 = d2 = d18=d28=0;		// reset integrator buffers 
		

		// now uncompress the data block 
		while ( block_position < block_length ) {

			aux_value = readbits( c_buffer, bit_readpos, bit_width );			// read bits 
			bit_readpos+=bit_width;
			
			if ( bit_width < 7 ) { // method 1 (1-6 bits) 

				if( bit16 )
				{
					if ( (signed)aux_value == (1 << (bit_width - 1)) ) { // check for "100..." 

						aux_value = readbits( c_buffer, bit_readpos, dsize )+1; //read_n_bits_from_IT_compressed_block(3) + 1; // yes -> read new width; 
						bit_readpos += dsize;
		    			bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
								// and expand it 
		    				continue; // ... next value 
					}
				}
				else
				{
					if ( aux_value == ((u32)1 << ((u32)bit_width - 1)) ) { // check for "100..." 

						aux_value = readbits( c_buffer, bit_readpos, dsize )+1; //read_n_bits_from_IT_compressed_block(3) + 1; // yes -> read new width; 
						bit_readpos += dsize;
		    			bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
								// and expand it 
		    				continue; // ... next value 
					}
				}

			} else if ( bit_width < nbits + 1 ) { // method 2 (7-8 bits) 

				if( bit16 )
				{
					border = (0xFFFF >> ((nbits+1) - bit_width)) - (nbits/2);
								// lower border for width chg 

					if ( (int)aux_value > (int)border && (int)aux_value <= (int)((int)border + nbits) ) {

						aux_value -= border; // convert width to 1-8 
						bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
								// and expand it 
		    				continue; // ... next value 
					}	
				}
				else
				{
					border = (0xFF >> ((nbits+1) - bit_width)) - (nbits/2);
								// lower border for width chg 

					if ( aux_value > border && aux_value <= (border + nbits) ) {

						aux_value -= border; // convert width to 1-8 
						bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
								// and expand it 
		    				continue; // ... next value 
					}	
				}

			} else if ( bit_width == nbits+1 ) { // method 3 (9 bits) 

				if ( aux_value & (1<<nbits) ) {			// bit 8 set? 

					bit_width = (aux_value + 1) & 0xff;		// new width... 
		    			continue;				// ... and next value 
				}

			} else { // illegal width, abort 

				if( c_buffer ) {
					free( c_buffer ); c_buffer=NULL; }
				return ERR_UNKNOWNSAMPLE;
			}

			// now expand value to signed byte 
			if ( bit_width < nbits ) {

				tmp_shift = nbits - bit_width;
				if( bit16 )
				{
					v16=(aux_value << tmp_shift);
					v16>>=tmp_shift;
				}
				else
				{
					v8=(aux_value << tmp_shift);
					v8>>=tmp_shift;
				}

			}
			else
			{
				if( bit16 )
					v16 = (s16) aux_value;
				else
					v8 = (s8) aux_value;
			}
			
			if( bit16 )
			{
				// integrate upon the sample values 
				d1 += v16;
	    		d2 += d1;

				// ... and store it into the buffer
				*(dest16_write++) = (it215 ? d2+32768 : d1+32768);
			}
			else
			{
				// integrate upon the sample values 
				d18 += v8;
	    		d28 += d18;
				
				// ... and store it into the buffer
				*(dest8_write)++ = (it215 ? (int)d28+128 : (int)d18+128);
			}
				block_position++;

		}

		// now subtract block lenght from total length and go on 
		if( c_buffer ) {
			free( c_buffer ); c_buffer=NULL; }
		samp_len -= block_length;
	}
	
	return ERR_NONE;
}
