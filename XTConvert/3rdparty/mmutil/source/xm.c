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

// information from XM.TXT by Mr.H of Triton

#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "mas.h"
#include "xm.h"
#include "files.h"
#include "simple.h"
#include "errors.h"
#include "math.h"
#include "samplefix.h"

#define cho 64

#ifdef SUPER_ASCII
#define vstr_xm_samp "  %2i   │   %s%s   │ %-22s │\n"
#define vstr_xm_nosamp  "  --   │   --   │ %-22s │\n"
#define vstr_xm_div "────────────────────────────────────────────\n"
#define vstr_xm_patt " \x0e %2i "
#define vstr_xm_samp_top "┌─────┬───────┬────────┬────────────────────────┐\n"
#define vstr_xm_samp_header "│INDEX│SAMPLES│ENVELOPE│          NAME          │\n"
#define vstr_xm_samp_prefix "│%3i  │"
#define vstr_xm_samp_slice "├─────┼───────┼────────┼────────────────────────┤\n"
#define vstr_xm_samp_bottom "└─────┴───────┴────────┴────────────────────────┘\n"
#else
#define vstr_xm_samp "  %2i   |   %s%s   | %-22s |\n"
#define vstr_xm_nosamp "  --   |   --   | %-22s |\n"
#define vstr_xm_div "--------------------------------------------\n"
#define vstr_xm_patt " Pattern %2i "
#define vstr_xm_samp_top ".-----------------------------------------------.\n"
#define vstr_xm_samp_header "|INDEX|SAMPLES|ENVELOPE|          NAME          |\n"
#define vstr_xm_samp_prefix "|%3i  |"
#define vstr_xm_samp_slice "|-----+-------+--------+------------------------|\n"
#define vstr_xm_samp_bottom "`-----------------------------------------------'\n"
#endif

int Get_XM_Frequency( s8 relnote, s8 finetune )
{
	double rn=relnote;
	double ft=finetune;
	double middle_c;
	double freq;
	middle_c = 8363.0f;
	freq = middle_c * pow( 2.0, (1.0/12.0)*rn + (1.0/(12.0*128.0))*ft );
	return (int)freq;
}

int Load_XM_Instrument( Instrument* inst, MAS_Module* mas, u8* p_nextsample, bool verbose )
{
	
	int inst_size;
	int nsamples;
	
	int ns;

	int samp_headstart;
	int samp_headsize;
	int inst_headstart;

	int sample_old;

	int x,y;
	u32 t;

	u8 vibtype;
	u8 vibsweep;
	u8 vibdepth;
	u8 vibrate;

	s8 finetune;
	s8 relnote;
	u8 loopbits;

	u8 volbits;
	u8 panbits;

	Sample* samp;

	ns = *p_nextsample;
	
	memset( inst, 0, sizeof( Instrument ) );

	inst_headstart = mm_file_tell_read();
	inst_size = mm_read32();
	
	for( x = 0; x < 22; x++ )
		inst->name[x] = mm_read8(); // instrument name
//	if( verbose )
//		printf( "  Name=\"%s\"\n", inst->name );
//	if( mm_read8() != 0 )
//		return ERR_UNKNOWNINST;
	mm_read8(); // instrument type, SUPPOSED TO ALWAYS BE 0...
	nsamples = mm_read16();
	if( nsamples > 0 )
	{
		samp_headsize = mm_read32();
		// read sample map
		for( x = 0; x < 96; x++ )
		{
			inst->notemap[x+12] = ((mm_read8()+ns+1)*256) | (x+12);
		}
		for( x=0; x < 12; x++ )
			inst->notemap[x] =( inst->notemap[12]&0xFF00) | x;
		for( x=96;x<120;x++)
			inst->notemap[x] =( inst->notemap[12]&0xFF00) | x;
		for( x = 0; x < 12; x++ )
		{
			inst->envelope_volume.node_x[x] = mm_read16();
			inst->envelope_volume.node_y[x] = (u8)mm_read16();
		}
		for( x = 0; x < 12; x++ )
		{
			inst->envelope_pan.node_x[x] = mm_read16();
			inst->envelope_pan.node_y[x] = (u8)mm_read16();
		}
		inst->global_volume = 128;
		inst->envelope_volume.node_count = mm_read8();
		inst->envelope_pan.node_count = mm_read8();
		inst->envelope_volume.sus_start = inst->envelope_volume.sus_end = mm_read8();
		inst->envelope_volume.loop_start = mm_read8();
		inst->envelope_volume.loop_end = mm_read8();
		inst->envelope_pan.sus_start = inst->envelope_pan.sus_end = mm_read8();
		inst->envelope_pan.loop_start = mm_read8();
		inst->envelope_pan.loop_end = mm_read8();
		volbits = mm_read8();
		panbits = mm_read8();
		inst->env_flags = 0;
		if( volbits & 1 )
			inst->env_flags |= 1|8;
		if( panbits & 1 )
			inst->env_flags |= 2;
		
		if( !(volbits & 2) )
			inst->envelope_volume.sus_start=inst->envelope_volume.sus_end=255;
		if( !(panbits & 2) )
			inst->envelope_pan.sus_start=inst->envelope_pan.sus_end=255;
		
		if( !(volbits & 4) )
			inst->envelope_volume.loop_start=inst->envelope_volume.loop_end=255;
		if( !(panbits & 4) )
			inst->envelope_pan.loop_start=inst->envelope_pan.loop_end=255;
		
		vibtype=mm_read8();
		vibsweep=32768/(mm_read8()+1);
		vibdepth=mm_read8();
		vibrate=mm_read8();
		inst->fadeout = mm_read16()/32;			// apply scalar!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		mm_file_seek_read( inst_headstart+inst_size, SEEK_SET );
			
/*		if( verbose )
		{
			if( volbits & 1 )
				printf( "  Has Volume Envelope\n" );
			if( panbits & 1 )
				printf( "  Has Panning Envelope\n" );
			if( nsamples != 1 )
				printf( "  Contains %i samples...\n", nsamples );
			else
				printf( "  Loading sample\n" );
		}
*/		
		// read sample headers
		for( x = 0; x < nsamples; x++ )
		{
			if( ns+x >= 256 )
				return ERR_TOOMANYSAMPLES;
			samp_headstart = mm_file_tell_read();
			samp = &mas->samples[ns+x];
//			if( verbose && nsamples != 1 )
//				printf( "  Loading sample %i...\n", x+1 );
			memset( samp, 0, sizeof( Sample ) );
			samp->msl_index = 0xFFFF;
			samp->sample_length = mm_read32();
			samp->loop_start = mm_read32();
			samp->loop_end = mm_read32()+samp->loop_start;
			samp->default_volume = mm_read8();
			samp->global_volume = 64;

			samp->vibtype = vibtype;
			samp->vibdepth = vibdepth;
			samp->vibspeed = vibrate;
			samp->vibrate = vibsweep;

			finetune = (s8)mm_read8();
			loopbits = mm_read8();
			samp->default_panning = (mm_read8()>>1) | 128;
			relnote = (s8)mm_read8();
			mm_read8();		// reserved

			for( y =0; y <22; y ++)
			{
				samp->name[y] = mm_read8();
				if( y < 12 )
					samp->filename[y] = samp->name[y];
			}
			
			samp->frequency = Get_XM_Frequency( relnote, finetune );
			
//			samp->bit16 = loopbits & 16 ? true : false;
			samp->format = loopbits & 16 ? SAMP_FORMAT_U16 : SAMP_FORMAT_U8;
			if( samp->format & SAMPF_16BIT )
			{
				samp->sample_length /= 2;
				samp->loop_start /= 2;
				samp->loop_end /= 2;
			}
			samp->loop_type = loopbits & 3;
			mm_file_seek_read( samp_headstart + samp_headsize, SEEK_SET );
/*
			if( verbose )
			{
				printf( "    Length........%i\n", samp->sample_length );
				if( samp->loop_type == 0 )
					printf( "    Loop..........Disabled\n" );
				else if( samp->loop_type == 1 )
					printf( "    Loop..........Forward %i->%i\n", samp->loop_start, samp->loop_end );
				else if( samp->loop_type == 2 )
					printf( "    Loop..........BIDI %i->%i\n", samp->loop_start, samp->loop_end );
				printf( "    Volume........%i\n", samp->default_volume );
				printf( "    Panning.......%i\n", samp->default_panning & 127 );
				printf( "    Middle C......%ihz\n", samp->frequency );
				printf( "    16 bit........%s\n", (samp->format & SAMPF_16BIT) ? "yes (will be converted)" : "no" );
			}*/
		}
		
		// read sample
		for( x = 0; x < nsamples; x++ )
		{
			samp = &mas->samples[ns+x];
			if( samp->sample_length == 0 )
				continue;
			
			sample_old = 0;
			if( samp->format & SAMPF_16BIT )
			{
				samp->data = (u16*)malloc( samp->sample_length*2 );
				for( t = 0; t < samp->sample_length; t++ )
				{
					sample_old = (s16)((s16)mm_read16() + sample_old);
					((u16*)samp->data)[t] = sample_old + 32768;
				}
			}
			else
			{
				samp->data = (u8*)malloc( samp->sample_length );
				for( t = 0; t < samp->sample_length; t++ )
				{
					sample_old = (s8)((s8)mm_read8() + sample_old);
					((u8*)samp->data)[t] = sample_old + 128;
				}
			}
			FixSample( samp );
		}
		*p_nextsample = ns+nsamples;


		if( verbose )
		{
			printf( vstr_xm_samp, nsamples, (volbits&1)?"V":"-", (panbits&1)?"P":"-", inst->name );
		}
	}
	else
	{
		mm_file_seek_read( inst_headstart+inst_size, SEEK_SET );
		if( verbose )
			printf( vstr_xm_nosamp, inst->name );
	}
	
	return ERR_NONE;
}

void CONV_XM_EFFECT( u8* fx, u8* param )
{
	int wfx, wpm;
	wfx=*fx;
	wpm=*param;

	switch( wfx )
	{
	case 0: //   0xy arpeggio
		if( wpm != 0)
			wfx = 'J'-cho;
		else
			wfx=wpm=0;
		break;
	
	case 1: //   1xx porta up
		wfx = 'F'-cho;
		if( wpm >= 0xE0 )
			wpm = 0xDF;
		break;
	
	case 2: //   2xx porta down
		wfx = 'E'-cho;
		if( wpm >= 0xE0 )
			wpm = 0xDF;
		break;
	
	case 3: //   3xx porta to note
		wfx = 'G'-cho;
		break;
	
	case 4: //   4xy vibrato
		wfx = 'H'-cho;
		break;
	
	case 5: //   5xy volslide+glissando
		wfx = 'L'-cho;
		break;
	
	case 6: //   6xy volslide+vibrato
		wfx = 'K'-cho;
		break;
	
	case 7: //   7xy tremolo
		wfx = 'R'-cho;
		break;
	
	case 8: //   8xx set panning
		wfx = 'X'-cho;
		break;
	
	case 9: //   9xx set offset
		wfx = 'O'-cho;
		break;
	
	case 0xA: // Axy volume slide
		wfx = 'D'-cho;
		break;
	
	case 0xB: // Bxx position jump
		wfx = 'B'-cho;
		break;
	
	case 0xC: // Cxx set volume
		wfx = 27;	// compatibility effect
		break;
	
	case 0xD: // Dxx pattern break
		wfx = 'C'-cho;
		wpm = (wpm&0xF) + (wpm>>4) * 10;
		/*if( wpm >= 0xF0 )               what the fuck is this
		{
			wpm = 0xE0 | (wpm&0xF);
		}
		if( wpm & 0xF == 0xF )
		{
			wpm = 0x0F | (wpm&0xF0);
		}*/
		break;
	
	case 0xE: // Exy extended
//		if( (wpm & 0xF0) != 0xC0 )
//		{int foo = 1;}
		switch( wpm >> 4 )
		{
		case 1:						// fine porta up
			wfx = 'F'-cho;
			wpm = 0xF0 | (wpm&0xF);
			break;

		case 2:						// fine porta down
			wfx = 'E'-cho;
			wpm = 0xF0 | (wpm&0xF);
			break;

		case 3:						// glissando control
		case 5:						// set finetune
			// UNSUPPORTED :(
			wfx=0;
			wpm=0;
			break;
			
		case 4:						// vibrato control
			wfx = 'S'-cho;
			wpm = 0x30 | (wpm&0xF);
			break;

		case 6:						// pattern loop
			wfx = 'S'-cho;
			wpm = 0xB0 | (wpm&0xF);
			break;
		
		case 7:						// tremolo control
			wfx = 'S'-cho;
			wpm = 0x40 | (wpm&0xF);
			break;

		case 8:						// set panning
			wfx = 'X'-cho;
			wpm = (wpm&0xF) * 16;
			break;
			
		case 9:						// old retrig
			wfx = 'S'-cho;
			wpm = 0x20 | (wpm&0xF);
			break;

		case 10:					// fine volslide up
			wfx = 'S'-cho;
			wpm = 0x00 | (wpm&0xF);
			break;

		case 11:					// fine volslide down
			wfx = 'S'-cho;
			wpm = 0x10 | (wpm&0xF);
			break;

		case 12:					// note cut
			wfx = 'S'-cho;
			wpm = 0xC0 | (wpm&0xF);
			break;
			
		case 13:					// note delay
			wfx = 'S'-cho;
			wpm = 0xD0 | (wpm&0xF);
			break;

		case 14:					// pattern delay
			wfx = 'S'-cho;
			wpm = 0xE0 | (wpm&0xF);
			break;
		case 15:					// event
			wfx = 'S'-cho;
			wpm = wpm;
			break;
		case 0:						// set filter
			wfx=0;
			wpm=0;

		}
		break;
	
	case 0xF: // Fxx set speed
		if( wpm >= 32 )
			wfx = 'T'-cho;
		else
			wfx = 'A'-cho;
		break;
	
	case 16: // Gxx set global volume
		wfx = 'V'-cho;
		wpm=wpm;
		break;
	
	case 17: // Hxx global volume slide
		wfx = 'W'-cho;
		break;

	case 18: // Ixx unused
	case 19: // Jxx unused
	case 22: // Mxx unused
	case 23: // Nxx unused
	case 24: // Oxx unused
	case 26: // Qxx unused
	case 28: // Sxx unused
	case 30: // Uxx unused
	case 31: // Vxx unused
	case 32: // Wxx unused
	case 34: // Yxx unused
	case 35: // Zxx unused
		wfx = 0;
		wpm = 0;
		break;
		
	case 20: // Kxx key off
		wfx = 28;
		break;
	
	case 21: // Lxx set envelope position
		wfx = 29;
		break;
	
	case 25: // Pxx panning slide
		wfx = 'P'-cho;
		break;

	
	
	case 27: // Rxx retrigger note
		wfx = 'Q'-cho;
		break;
		
	case 29: // Txx tremor
		wfx = 30;
		break;
		
	case 33: // Xxx extra fine slide
		if( (wpm>>4) == 1 )
		{
			wfx = 'F'-cho;
			wpm = 0xE0 | (wpm & 0xF);
		}
		else if( (wpm>>4) == 2 )
		{
			wfx = 'E'-cho;
			wpm = 0xE0 | (wpm & 0xF);
		}
		else
		{
			wfx=0;
			wpm=0;
		}
		break;
	}
	*fx = wfx;
	*param = wpm;
}

int Load_XM_Pattern( Pattern* patt, u32 nchannels, bool verbose )
{
	u32 headsize;
	u32 headstart;
	u16	clength;
	u16 row, col;
	u8	b;
	u32 e;
	u8 fx,param;

	headstart = mm_file_tell_read();
	headsize = mm_read32();
	
	if( mm_read8() != 0 )
		return ERR_UNKNOWNPATTERN;

	memset( patt, 0, sizeof( Pattern ) );
	
	patt->nrows = mm_read16();
	clength = mm_read16();
	
	if( verbose )
		printf( "- %i rows, %.2f KB\n", patt->nrows, (float)(clength)/1000 );


	for( row = 0; row < patt->nrows*MAX_CHANNELS; row++ )
	{
		patt->data[row].note = 250;
		patt->data[row].vol = 0;
	}

	mm_file_seek_read( headstart+headsize, SEEK_SET );
	
	if( clength == 0 )
	{
		// pattern is empty
		return ERR_NONE;
	}
	
	// read pattern data
	for( row = 0; row < patt->nrows; row++ )
	{
		for( col = 0; col < nchannels; col++ )
		{
			e = row*MAX_CHANNELS+col;
			b = mm_read8();
			if( b & 128 )	// packed
			{
				if( b & 1 )			// bit 0 set: Note follows
				{
					patt->data[e].note = mm_read8();			// (byte) Note (1-96, 1 = C-0)
					if( patt->data[e].note == 97 )
						patt->data[e].note = 255;
					else
						patt->data[e].note += 12-1;
				}
				if( b & 2 )			// 1 set: Instrument follows
				{
					patt->data[e].inst = mm_read8();			// (byte) Instrument (1-128)
				}
				if( b & 4 )			// 2 set: Volume column byte follows
				{
					patt->data[e].vol = mm_read8();	// (byte) Volume column byte
				}
				if( b & 8 )			// 3 set: Effect type follows
					fx = mm_read8();								// (byte) Effect type
				else
					fx=0;
				if( b & 16 )		// 4 set: Guess what!
					param=mm_read8();								// (byte) Effect parameter
				else
					param=0;

				if( fx != 0 || param != 0 )
				{
					CONV_XM_EFFECT( &fx, &param );		// convert effect
					patt->data[e].fx = fx;
					patt->data[e].param = param;
				}
			}
			else			// unpacked
			{
				patt->data[e].note = b;					// (byte) Note (1-96, 1 = C-0)
				if( patt->data[e].note == 97 )
					patt->data[e].note = 255;
				else
					patt->data[e].note += 12-1;
				patt->data[e].inst = mm_read8();			// (byte) Instrument (1-128)
				patt->data[e].vol = mm_read8();				// (byte) Volume column byte (see below)
				fx = mm_read8();								// (byte) Effect type
				param=mm_read8();								// (byte) Effect parameter
				CONV_XM_EFFECT( &fx, &param );				// convert effect
				patt->data[e].fx = fx;
				patt->data[e].param = param;
			}
		}
	}
	return ERR_NONE;
}

int Load_XM( MAS_Module* mod, bool verbose )
{
	int x;
	u16 xm_version;
	u32 xm_headsize;
	u16 xm_nchannels;
	u8 next_sample;
	
	memset( mod, 0, sizeof( MAS_Module ) );
	
	mod->old_effects=true;
	mod->xm_mode=true;
	mod->global_volume=64;
	mod->old_mode=false;

	if( mm_read32() != 'etxE' || mm_read32() != 'dedn' || mm_read32() != 'doM ' || mm_read32() != ':elu' || mm_read8() != ' ' )
		return ERR_INVALID_MODULE;
	for( x = 0; x < 20; x++ )
		mod->title[x] = mm_read8();
	if( verbose )
	{
		printf(  vstr_xm_div  );
		printf( "Loading XM, \"%s\"\n", mod->title );
	}
	if( mm_read8() != 0x1a )
		return ERR_INVALID_MODULE;
	mm_skip8( 20 ); // tracker name
	xm_version = mm_read16();
	xm_headsize = mm_read32();
	mod->order_count	= (u8)mm_read16();
	mod->restart_pos	= (u8)mm_read16();
	xm_nchannels		= mm_read16();
	mod->patt_count		= (u8)mm_read16();
	mod->inst_count		= (u8)mm_read16();
	mod->freq_mode		= mm_read16() & 1 ? true : false;		// flags
	mod->initial_speed	= (u8)mm_read16();
	mod->initial_tempo	= (u8)mm_read16();

	if( verbose )
	{
		printf( "Version....%i.%i\n", xm_version>>8 & 0xFF, xm_version & 0xFF );
		printf( "Length.....%i\n", mod->order_count );
		printf( "Restart....%i\n", mod->restart_pos );
		printf( "Channels...%i\n", xm_nchannels );
		printf( "#Patterns..%i\n", mod->patt_count );
		printf( "#Instr.....%i\n", mod->inst_count );
		printf( "Freq Mode..%s\n", mod->freq_mode ? "Linear" : "Amiga" );
		printf( "Speed......%i\n", mod->initial_speed );
		printf( "Tempo......%i\n", mod->initial_tempo );
	}
	
	for( x = 0; x < 32; x++ )
	{
		mod->channel_volume[x] = 64;
		mod->channel_panning[x] = 128;
	}
	if( verbose )
	{
		printf( vstr_xm_div );
		printf( "Reading sequence...\n" );
	}
	for( x = 0; x < 200; x++ )		// read order table
	{
		if( x < mod->order_count )
			mod->orders[x] = mm_read8();
		else
		{
			mm_read8();
			mod->orders[x] = 255;
		}
	}
	
	for( ; x < 256; x++ )			// skip 200->255
		mm_read8();
	mm_file_seek_read( 60+xm_headsize, SEEK_SET );	// or maybe 60..

	if( verbose )
	{
		printf( vstr_xm_div );
		printf( "Loading patterns...\n" );
		printf( vstr_xm_div );
	}
	
	mod->patterns = (Pattern*)malloc( mod->patt_count * sizeof( Pattern ) );
	for( x = 0; x < mod->patt_count; x++ )
	{
		if( verbose )
			printf( vstr_xm_patt, x+1 );
		Load_XM_Pattern( &mod->patterns[x], xm_nchannels, verbose );
	}
	
	mod->instruments = (Instrument*)malloc( mod->inst_count * sizeof( Instrument ) );
	mod->samples = (Sample*)malloc( 256 * sizeof( Sample ) );
	next_sample=0;

	
	if( verbose )
	{
		printf( vstr_xm_div );
		printf( "Loading instruments...\n" );
		printf( vstr_xm_samp_top );
		printf( vstr_xm_samp_header );
		printf( vstr_xm_samp_slice );
	}
	
	for( x = 0; x < mod->inst_count; x++ )
	{
	//	if( verbose )
	//		printf( "Reading Instrument %i...\n", x+1 );
		if( verbose )
			printf( vstr_xm_samp_prefix, x+1 );
		Load_XM_Instrument( &mod->instruments[x], mod, &next_sample, verbose );
	}
	
	if( verbose )
	{
		printf( vstr_xm_samp_bottom );
	}
	
	mod->samp_count = next_sample;
	return ERR_NONE;
}
