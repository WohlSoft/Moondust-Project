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

#define	MAS_C

#include <stdlib.h>
#include "defs.h"
#include "files.h"
#include "mas.h"
#include "simple.h"
#include "systems.h"
#include "version.h"

u32 MAS_OFFSET;
u32 MM_MAS_FILESIZE;

static int CalcEnvelopeSize( Instrument_Envelope* env )
{
	return (env->node_count*4) + 8;
}

static int CalcInstrumentSize( Instrument* instr )
{
	int size;
	size = 12;
	if( instr->env_flags & 1 )	// volume envelope exists
		size += CalcEnvelopeSize( &instr->envelope_volume );
	if( instr->env_flags & 2 )	// panning envelope exists
		size += CalcEnvelopeSize( &instr->envelope_pan );
	if( instr->env_flags & 4 )	// pitch envelope exists
		size += CalcEnvelopeSize( &instr->envelope_pitch );
	return size;
}

void Write_Instrument_Envelope( Instrument_Envelope* env )
{
	int x;
	mm_write8( (u8)(env->node_count*4 + 8) ); // maximum is 6+75
	mm_write8( env->loop_start );
	mm_write8( env->loop_end );
	mm_write8( env->sus_start );
	mm_write8( env->sus_end );
	mm_write8( env->node_count );
	mm_write8( env->env_filter );
	mm_write8( BYTESMASHER );
	if( env->node_count > 1 )
	{
		int delta;
		int base;
		int range;
		for( x = 0; x < env->node_count; x++ )
		{
			base = env->node_y[x];
			if( x != env->node_count-1 )
			{
				
				range = env->node_x[x+1] - env->node_x[x];
				if( range > 511 ) range = 511;
				if( range < 1 ) range = 1;
				delta = (((env->node_y[x+1] - base) * 512) + (range/2)) / range;
				if( delta > 32767 ) delta = 32767;
				if( delta < -32768 ) delta = -32768;
				while( (base+ ((delta*range)>>9)) > 64 ) delta--;
				while( (base+ ((delta*range)>>9)) < 0 ) delta++;
			}
			else
			{
				range = 0;
				delta=0;
			}
			mm_write16( (u16)delta );
			mm_write16( (u16)(base | (range<<7)) );
		}
	}
}

void Write_Instrument( Instrument* inst )
{
	int y;
	int full_notemap;
	int first_notemap_samp;
	mm_align32();
	inst->parapointer = mm_file_tell_write()-MAS_OFFSET;
	/*mm_write8( inst->global_volume );
	mm_write8( (u8)inst->fadeout );
	mm_write8( inst->random_volume );
	mm_write8( inst->nna );
	mm_write8( inst->dct );
	mm_write8( inst->dca );
	mm_write8( inst->env_flags );
	mm_write8( inst->setpan );*/

	mm_write8( inst->global_volume );
	mm_write8( (u8)inst->fadeout );
	mm_write8( inst->random_volume );
	mm_write8( inst->dct );
	mm_write8( inst->nna );
	mm_write8( inst->env_flags );
	mm_write8( inst->setpan );
	mm_write8( inst->dca );

	full_notemap = 0;
	first_notemap_samp = (inst->notemap[0] >> 8);
	for( y = 0; y < 120; y++ )
	{
		if( ((inst->notemap[y] & 0xFF) != y) ||
			((inst->notemap[y] >> 8) != first_notemap_samp) )
		{
			full_notemap = 1;
			break;
		}
	}
	
	if( full_notemap )
	{
		// full notemap
		// write offset here
		mm_write16( (u16)CalcInstrumentSize( inst ) );
	}
	else
	{
		// single notemap entry
		mm_write16( (u16)(0x8000 | first_notemap_samp) );
	}

	mm_write16( 0 );	// reserved space
	
	if( inst->env_flags & 1 )	// write volume envelope
		Write_Instrument_Envelope( &inst->envelope_volume );
	if( inst->env_flags & 2 )	// write panning envelope
		Write_Instrument_Envelope( &inst->envelope_pan );
	if( inst->env_flags & 4 )	// write pitch envelope
		Write_Instrument_Envelope( &inst->envelope_pitch );

	if( full_notemap )
	{
		for( y = 0; y < 120; y++ )
			mm_write16( inst->notemap[y] );
	}
}

void Write_SampleData( Sample* samp )
{
	u32 x;
	u32 sample_length = samp->sample_length;
	u32 sample_looplen = samp->loop_end - samp->loop_start;

	if( mm_target_system == SYSTEM_GBA )
	{
		mm_write32( sample_length );
		mm_write32( samp->loop_type ? sample_looplen : 0xFFFFFFFF );
		mm_write8( SAMP_FORMAT_U8 );
		mm_write8( BYTESMASHER );
		mm_write16( (u16) ((samp->frequency * 1024 + (15768/2)) / 15768) );
	//	mm_write32( 0);
	}
	else
	{
		if( samp->format & SAMPF_16BIT )
		{
			if( samp->loop_type )
			{
				mm_write32( samp->loop_start / 2 );
				mm_write32( (samp->loop_end-samp->loop_start) / 2 );
			}
			else
			{
				mm_write32( 0 );
				mm_write32( sample_length/2 );
			}
		}
		else
		{
			if( samp->loop_type )
			{
				mm_write32( samp->loop_start / 4 );
				mm_write32( (samp->loop_end-samp->loop_start) / 4 );
			}
			else
			{
				mm_write32( 0 );
				mm_write32( sample_length/4 );
			}
		}
		mm_write8( sample_dsformat( samp ) );
		mm_write8( sample_dsreptype( samp ) );
		mm_write16( (u16) ((samp->frequency * 1024 + (32768/2)) / 32768) );
		mm_write32( 0);
	}
	
	// write sample data
	if( samp->format & SAMPF_16BIT )
	{
		for( x = 0; x < sample_length; x++ )
			mm_write16( ((u16*)samp->data)[x] );

		// add padding data
		if( samp->loop_type && sample_length >= (samp->loop_start + 2) )
		{
			mm_write16( ((u16*)samp->data)[samp->loop_start] );
			mm_write16( ((u16*)samp->data)[samp->loop_start+1] );
		}
		else
		{
			mm_write16( 0 );
			mm_write16( 0 );
		}
	}
	else
	{
		for( x = 0; x < sample_length; x++ )
			mm_write8( ((u8*)samp->data)[x] );

		// add padding data
		if( samp->loop_type && sample_length >= (samp->loop_start + 4) )
		{
			mm_write8( ((u8*)samp->data)[samp->loop_start] );
			mm_write8( ((u8*)samp->data)[samp->loop_start+1] );
			mm_write8( ((u8*)samp->data)[samp->loop_start+2] );
			mm_write8( ((u8*)samp->data)[samp->loop_start+3] );
		}
		else
		{
			for ( x = 0; x < 4; x++ )
				mm_write8( (mm_target_system == SYSTEM_GBA) ? 128 : 0 );
		}
	}
}

void Write_Sample( Sample* samp )
{
	mm_align32(); // align data by 32 bits
	samp->parapointer = mm_file_tell_write()-MAS_OFFSET;

	mm_write8( samp->default_volume );
	mm_write8( samp->default_panning );
	mm_write16( (u16)(samp->frequency/4) );
	mm_write8( samp->vibtype );
	mm_write8( samp->vibdepth );
	mm_write8( samp->vibspeed );
	mm_write8( samp->global_volume );
	mm_write16( samp->vibrate );
	
	mm_write16( samp->msl_index );
	
	if( samp->msl_index == 0xFFFF )
		Write_SampleData(samp);
}

void Write_Pattern( Pattern* patt, bool xm_vol )
{
	int row;
	int col;
	
	u16 last_mask[MAX_CHANNELS];
	u16 last_note[MAX_CHANNELS];
	u16 last_inst[MAX_CHANNELS];
	u16 last_vol[MAX_CHANNELS];
	u16 last_fx[MAX_CHANNELS];
	u16 last_param[MAX_CHANNELS];
	
	u8 chanvar;
	u8 maskvar;

	u8 emptyvol;
	
	PatternEntry* pe;
	
	patt->parapointer = mm_file_tell_write()-MAS_OFFSET;
	mm_write8( (u8)(patt->nrows-1) );
	
	patt->cmarks[0] = true;
	emptyvol = xm_vol ? 0 : 255;
	
	// using IT pattern compression
	
	for( row = 0; row < patt->nrows; row++ )
	{
		if( patt->cmarks[row] )
		{
			for( col=0;col<MAX_CHANNELS;col++ )
			{
				last_mask[col] = 256; // row is marked, clear previous data
				last_note[col] = 256;
				last_inst[col] = 256;
				last_vol[col]= 256;
				last_fx[col] = 256;
				last_param[col] = 256;
			}
		}
		for( col = 0; col < MAX_CHANNELS; col++ )
		{
			pe = &patt->data[row*MAX_CHANNELS+col];
			if( ((pe->note != 250) || (pe->inst != 0) || (pe->vol != emptyvol) || (pe->fx != 0) || (pe->param != 0)) )
			{
				maskvar = 0;
				chanvar = col+1;
				if( pe->note != 250 )
					maskvar |= 1|16;
				
				if( pe->inst != 0 )
					maskvar |= 2|32;

				if( pe->note > 250 )	// noteoff/cut disabled start+reset
					maskvar &= ~(16|32);
				
				if( pe->vol != emptyvol )
					maskvar |= 4|64;
				
				if( pe->fx != 0 || pe->param != 0 )
					maskvar |= 8|128;
				
				if( maskvar & 1 )
				{
					if( pe->note == last_note[col] )
					{
						maskvar &= ~1;
					}
					else
					{
						last_note[col] = pe->note;
						if( last_note[col] == 254 || last_note[col] == 255 )		// DONT LET NOTEOFF/NOTECUT USE PREVIOUS PARAMETERS!
							last_note[col] = 256;
					}
				}
				
				if( maskvar & 2 )
				{
					if( pe->inst == last_inst[col] )
					{
						maskvar &= ~2;
					}
					else
					{
						last_inst[col] = pe->inst;
					}
				}
				
				if( maskvar & 4 )
				{
					if( pe->vol == last_vol[col] )
					{
						maskvar &= ~4;
					}
					else
					{
						last_vol[col] = pe->vol;
					}
				}
				
				if( maskvar & 8 )
				{
					if( (pe->fx == last_fx[col]) && (pe->param == last_param[col]) )
					{
						maskvar &= ~8;
					}
					else
					{
						last_fx[col] = pe->fx;
						last_param[col] = pe->param;
					}
				}
				
				if( maskvar != last_mask[col] )
				{
					chanvar |= 128;
					last_mask[col] = maskvar;
				}

				mm_write8( chanvar );
				if( chanvar & 128 )
					mm_write8( maskvar );
				if( maskvar & 1 )
					mm_write8( pe->note );
				if( maskvar & 2 )
					mm_write8( pe->inst );
				if( maskvar & 4 )
					mm_write8( pe->vol );
				if( maskvar & 8 )
				{
					mm_write8( pe->fx );
					mm_write8( pe->param );
				}
			}
			else
			{
				continue;
			}
		}
		mm_write8( 0 );
	}
	
}

void Mark_Pattern_Row( MAS_Module* mod, int order, int row )
{
	Pattern* p;
	
	if( row >= 256 )
		return;
	if( mod->orders[order] == 255 )
		order = 0;
	while( mod->orders[order] >= 254 )
	{
		if( mod->orders[order] == 255 )
			return;
		if( mod->orders[order] == 254 )
			order++;
	}
	p = &(mod->patterns[mod->orders[order]]);
	p->cmarks[row] = true;
}

void Mark_Patterns( MAS_Module* mod )
{
	
	int o;
	int p;
	int row;
	int col;
	
	PatternEntry* pe;
	
	for( o = 0; o < mod->order_count; o++ )
	{
		p = mod->orders[o];
		if( p == 255 )
			break;
		if( p == 254 )
			continue;
		if( p >= mod->patt_count )
			continue;
		for( row = 0; row < mod->patterns[p].nrows; row++ )
		{
			for( col = 0; col < MAX_CHANNELS; col++ )
			{
				pe = &(mod->patterns[p].data[row*MAX_CHANNELS+col]);
				if( pe->fx == 3 )	// PATTERN BREAK
				{
					if( pe->param != 0 ) // if param != 0 then mark next pattern
					{
						Mark_Pattern_Row( mod, o+1, pe->param );
					}
				}
				else if( pe->fx == 19 )
				{
					if( pe->param == 0xB0 )
					{
						Mark_Pattern_Row( mod, o, row );
					}
				}
			}
		}
	}
}

int Write_MAS( MAS_Module* mod, bool verbose, bool msl_dep )
{
	// SEE MAS_FORM.TXT
	int x; //,y;
	int fpos_pointer;
//	u8 rsamp=0;
//	u16 rsamps[200];
//	bool unique=false;

	mm_file_get_byte_count();

	mm_write32( BYTESMASHER );
	mm_write8( MAS_TYPE_SONG );
	mm_write8(	MAS_VERSION );
	mm_write8( BYTESMASHER );
	mm_write8( BYTESMASHER );
	
	MAS_OFFSET = mm_file_tell_write();

	mm_write8( (u8)mod->order_count );
	mm_write8( mod->inst_count );
	mm_write8( mod->samp_count );
	mm_write8( mod->patt_count );
	mm_write8( (u8)((mod->link_gxx ? 1 : 0) | (mod->old_effects ? 2 : 0) | (mod->freq_mode ? 4 : 0) | (mod->xm_mode ? 8 : 0) | (msl_dep ? 16 : 0) | (mod->old_mode ? 32 : 0)) );
	mm_write8( mod->global_volume );
	mm_write8( mod->initial_speed );
	mm_write8( mod->initial_tempo );
	mm_write8( mod->restart_pos );
	
/*	for( x = 0; x < mod->samp_count; x++ )
	{
		unique=true;
		for( y = x-1; y >= 0; y-- )
		{
			if( mod->samples[x].msl_index == mod->samples[y].msl_index )
			{
				mod->samples[x].rsamp_index = mod->samples[y].rsamp_index;
				unique=false;
				break;
			}
		}
		if( unique )
		{
			rsamps[rsamp]=mod->samples[x].msl_index;
			mod->samples[x].rsamp_index = rsamp;
			rsamp++;
		}
	}
	mm_write8( rsamp );*/
	mm_write8( BYTESMASHER );
	mm_write8( BYTESMASHER );mm_write8( BYTESMASHER );
	for( x = 0; x < MAX_CHANNELS; x++ )
		mm_write8( mod->channel_volume[x] );
	for( x = 0; x < MAX_CHANNELS; x++ )
		mm_write8( mod->channel_panning[x] );
	for( x = 0; x < mod->order_count; x++ )
	{
		if( mod->orders[x] < 254 )
		{
			if( mod->orders[x] < mod->patt_count )
				mm_write8( mod->orders[x] );
			else
				mm_write8( 254 );
		}
		else
		{
			mm_write8( mod->orders[x] );
		}
	}
	for( ; x < 200; x++ )
		mm_write8( 255 );
	// reserve space for offsets
	fpos_pointer = mm_file_tell_write();
	for( x = 0; x < mod->inst_count*4+mod->samp_count*4+mod->patt_count*4; x++ )
		mm_write8( BYTESMASHER ); // BA BA BLACK SHEEP
/*	if( msl_dep && mm_target_system == SYSTEM_NDS )
	{
		for( x = 0; x < rsamp; x++ )	// write sample indeces
			mm_write16( rsamps[x] );
	}*/
	// WRITE INSTRUMENTS

	if( verbose )
		printf("Header: %i bytes\n", mm_file_get_byte_count() );

	for( x = 0; x < mod->inst_count; x++ )
		Write_Instrument( &mod->instruments[x] );

	

	for( x = 0; x < mod->samp_count; x++ )
		Write_Sample( &mod->samples[x] );

	if( verbose )
		printf("Instruments: %i bytes\n", mm_file_get_byte_count() );
	
	Mark_Patterns( mod );
	for( x = 0; x < mod->patt_count; x++ )
	{
//		for( y = 0; y < mod->order_count; y++ )
//		{
//			if( mod->orders[y] == x )
				Write_Pattern( &mod->patterns[x], mod->xm_mode );
//		}
	}
	mm_align32();

	if( verbose )
		printf("Patterns: %i bytes\n", mm_file_get_byte_count() );
	MM_MAS_FILESIZE = mm_file_tell_write() - MAS_OFFSET;
	mm_file_seek_write( MAS_OFFSET-8, SEEK_SET );
	mm_write32( MM_MAS_FILESIZE );
	mm_file_seek_write( fpos_pointer, SEEK_SET );
	for( x = 0; x < mod->inst_count; x++ )
		mm_write32( mod->instruments[x].parapointer );
	for( x = 0; x < mod->samp_count; x++ )
	{
		if( verbose )
		{
			printf("sample %s is at %d/%d of %d\n", mod->samples[x].name, mod->samples[x].parapointer,
				mm_file_tell_write(), mod->samples[x].sample_length);
		}

		mm_write32( mod->samples[x].parapointer );
	}
	for( x = 0; x < mod->patt_count; x++ )
		mm_write32( mod->patterns[x].parapointer );
	return MM_MAS_FILESIZE;
}

void Delete_Module( MAS_Module* mod )
{
	int x;
	if( mod->instruments )
		free( mod->instruments );
	if( mod->samples )
	{
		for( x = 0; x < mod->samp_count; x++ )
		{
			if( mod->samples[x].data )
				free( mod->samples[x].data );
		}
		free( mod->samples );
	}
	if( mod->patterns )
	{
		free( mod->patterns );
	}
}
