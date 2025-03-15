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

// MAXMOD SOUNDBANK

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "defs.h"
#include "files.h"
#include "mas.h"
#include "mod.h"
#include "s3m.h"
#include "xm.h"
#include "it.h"
#include "wav.h"
#include "simple.h"
#include "version.h"
#include "systems.h"
#include "samplefix.h"

int mm_target_system = SYSTEM_NDS;

bool mm_ignore_sflags = false;
int MM_PANNING_SEP = 128;

// static FILE*	F_SCRIPT=NULL;

static FILE*	F_SAMP=NULL;
// static FILE*	F_SONG=NULL;

static FILE*	F_HEADER=NULL;
static FILE*	F_INI=NULL;

static char    INI_STATE='\0';

static u16		MSL_SAMPS_START;
static u16		MSL_SONGS_START;

static u16		MSL_NSAMPS;
static u16		MSL_NSONGS;

static char	str_msl[256];

#define TMP_SAMP "sampJ328G54AU3.tmp"
#define TMP_SONG "songDJ34957FAI.tmp"

void MSL_PrintDefinition( char* filename, u16 id, char* prefix );

#define SAMPLE_HEADER_SIZE (12 + (( mm_target_system == SYSTEM_NDS ) ? 4:0))

void MSL_Erase( u16 start_sample, u16 start_mod )
{
	MSL_SAMPS_START = start_sample;
	MSL_SONGS_START = start_mod;
	MSL_NSAMPS = MSL_SAMPS_START;
	MSL_NSONGS = MSL_SONGS_START;
	mm_file_delete( TMP_SAMP );
	mm_file_delete( TMP_SONG );
}

u16 MSL_AddSample( Sample* samp )
{
	u32 sample_length;
	// u32 x; // UNUSED
	mm_file_open_write_end( TMP_SAMP );

	sample_length = samp->sample_length;

	mm_write32( ((samp->format & SAMPF_16BIT) ? sample_length*2 : sample_length ) + SAMPLE_HEADER_SIZE  +4); // +4 for sample padding
	mm_write8 ( (mm_target_system == SYSTEM_GBA) ? MAS_TYPE_SAMPLE_GBA : MAS_TYPE_SAMPLE_NDS );
	mm_write8( MAS_VERSION );
	mm_write8( samp->filename[0] == '#' ? 1 : 0);
	mm_write8( BYTESMASHER );

	Write_SampleData(samp);

	mm_file_close_write();
	MSL_NSAMPS++;
	return MSL_NSAMPS-1;
}

u16 MSL_AddSampleC( Sample* samp )
{
	u32 st;
	u32 samp_len;
	u32 samp_llen;
	u8 sformat;
	u8 target_sformat;

	u32 h_filesize;
	int samp_id;
	bool samp_match;
		
	int fsize=mm_file_size( TMP_SAMP );
	if( fsize == 0 )
	{
		return MSL_AddSample( samp );
	}
	F_SAMP = fopen( TMP_SAMP, "rb" );
	fseek( F_SAMP, 0, SEEK_SET );
	samp_id = 0;
	while( ftell( F_SAMP ) < fsize )
	{
		h_filesize = mm_read32f( F_SAMP );
		mm_read32f( F_SAMP );
		samp_len = mm_read32f( F_SAMP );
		samp_llen = mm_read32f( F_SAMP );
		sformat = mm_read8f( F_SAMP );
		mm_skip8f( 3, F_SAMP );
		if( mm_target_system == SYSTEM_NDS )
		{
			target_sformat = sample_dsformat( samp );
			mm_skip8f(4,F_SAMP);
		}
		else
		{
			target_sformat = SAMP_FORMAT_U8;
		}

		samp_match=true;
		if( samp->sample_length == samp_len && ( samp->loop_type ? samp->loop_end-samp->loop_start : 0xFFFFFFFF ) == samp_llen && sformat == target_sformat )
		{
			// verify sample data
			if( samp->format & SAMPF_16BIT )
			{
				for( st=0; st<samp_len; st++ )
				{
					if( mm_read16f( F_SAMP ) != ((u16*)samp->data)[st] )
					{
						samp_match = false;
						break;
					}
				}
			}
			else
			{
				for( st=0; st<samp_len; st++ )
				{
					if( mm_read8f( F_SAMP ) != ((u8*)samp->data)[st] )
					{
						samp_match = false;
						break;
					}
				}
			}
			if( samp_match )
			{
				fclose( F_SAMP );
				return samp_id;
			}
			else
			{
				mm_skip8f( (h_filesize-SAMPLE_HEADER_SIZE ) - (st+1)  , F_SAMP );		// +4 to skip padding
			}
		}
		else
		{
			mm_skip8f( h_filesize- SAMPLE_HEADER_SIZE , F_SAMP ); // +4 to skip padding
		}
		samp_id++;
	}
	fclose( F_SAMP );
	return MSL_AddSample( samp );
}

u16 MSL_AddModule( MAS_Module* mod )
{
	int x;
	int samp_id;
	// ADD SAMPLES
	for( x = 0; x < mod->samp_count; x++ )
	{
		samp_id = MSL_AddSampleC( &mod->samples[x] );
		if( mod->samples[x].filename[0] == '#' )
			MSL_PrintDefinition( mod->samples[x].filename+1, (u16)samp_id, "SFX_" );
		mod->samples[x].msl_index = samp_id;
	}
	
	mm_file_open_write_end( TMP_SONG );
	Write_MAS( mod, false, true );
	mm_file_close_write();
	MSL_NSONGS++;
	return MSL_NSONGS-1;
}

int MSL_Export( char* filename )
{
	u32 x;
	u32 y;
	u32 mm_file_size;

	u32* parap_samp;
	u32* parap_song;

	if(mm_file_open_write( filename ) != FILE_OPEN_OKAY)
		return FILE_OPEN_ERROR;

	mm_write16( MSL_NSAMPS );
	mm_write16( MSL_NSONGS );
	mm_write8( '*' );
	mm_write8( 'm' );
	mm_write8( 'a' );
	mm_write8( 'x' );
	mm_write8( 'm' );
	mm_write8( 'o' );
	mm_write8( 'd' );
	mm_write8( '*' );
	
	parap_samp = (u32*)malloc( MSL_NSAMPS * sizeof( u32 ) );
	parap_song = (u32*)malloc( MSL_NSONGS * sizeof( u32 ) );
	
	// reserve space for parapointers
	for( x = MSL_SAMPS_START; x < MSL_NSAMPS; x++ )
		mm_write32( 0xAAAAAAAA );
	for( x = MSL_SONGS_START; x < MSL_NSONGS; x++ )
		mm_write32( 0xAAAAAAAA );
	// copy samples
	mm_file_open_read( TMP_SAMP );
	for( x = MSL_SAMPS_START; x < MSL_NSAMPS; x++ )
	{
		mm_align32();
		parap_samp[x] = mm_file_tell_write();
		mm_file_size = mm_read32();
		mm_write32( mm_file_size );
		for( y = 0; y < mm_file_size+4; y++ )
			mm_write8( mm_read8() );
	}
	mm_file_close_read();
	
	mm_file_open_read( TMP_SONG );
	for( x = MSL_SONGS_START; x < MSL_NSONGS; x++ )
	{
		mm_align32();
		parap_song[x] = mm_file_tell_write();
		mm_file_size = mm_read32();
		mm_write32( mm_file_size );
		for( y = 0; y < mm_file_size+4; y++ )
			mm_write8( mm_read8() );
	}
	mm_file_close_read();
	
	mm_file_seek_write( 0x0C, SEEK_SET );
	for( x = MSL_SAMPS_START; x < MSL_NSAMPS; x++ )
		mm_write32( parap_samp[x] );
	for( x=  MSL_SONGS_START; x < MSL_NSONGS; x++ )
		mm_write32( parap_song[x] );

	mm_file_close_write();

	if( parap_samp )
		free( parap_samp );
	if( parap_song )
		free( parap_song );

	return ERR_NONE;
}

void MSL_PrintDefinition( char* filename, u16 id, char* prefix )
{
	char newtitle[64];
	int x,s=0;
	if( filename[0] == 0 )	// empty string
		return;

	if( F_INI )
	{
		if( INI_STATE != prefix[0] )
		{
			if( INI_STATE != 0 )
				fprintf( F_INI, "\n" );

			if( prefix[0] == 'M' )
				fprintf( F_INI, "[modules]\n" );
			else
				fprintf( F_INI, "[samples]\n" );

			INI_STATE = prefix[0];
		}

		fprintf( F_INI, "%s = %d\n", filename, (int)id );
	}

	for( x = 0; x < (int)strlen( filename ); x++ )
	{
		if( filename[x] == '\\' || filename[x] == '/' ) s = x+1; 
	}
	for( x = s; x < (int)strlen( filename ); x++ )
	{
		if( filename[x] != '.' )
		{
			newtitle[x-s] = toupper(filename[x]);
			if( newtitle[x-s] >= ' ' && newtitle[x-s] <= '/' )
				newtitle[x-s] = '_';
			if( newtitle[x-s] >= ':' && newtitle[x-s] <= '@' )
				newtitle[x-s] = '_';
			if( newtitle[x-s] >= '[' && newtitle[x-s] <= '`' )
				newtitle[x-s] = '_';
			if( newtitle[x-s] >= '{' )
				newtitle[x-s] = '_';
		}
		else
		{
			break;
		}
	}
	newtitle[x-s] = 0;
	if( F_HEADER )
	{
		fprintf( F_HEADER, "#define %s%s	%i\r\n", prefix, newtitle, id );
	}
}

void MSL_LoadFile( char* filename, bool verbose )
{
	Sample wav;
	MAS_Module mod;
	int f_ext;
	if( mm_file_open_read( filename ) )
	{
		printf( "Cannot open %s for reading! Skipping.\n", filename );
		return;
	}
	f_ext = get_ext( filename );
	switch( f_ext )
	{
	case INPUT_TYPE_MOD:
		Load_MOD( &mod, verbose );
		MSL_PrintDefinition( filename, MSL_AddModule( &mod ), "MOD_" );
		Delete_Module( &mod );
		break;
	case INPUT_TYPE_S3M:
		Load_S3M( &mod, verbose );
		MSL_PrintDefinition( filename, MSL_AddModule( &mod ), "MOD_" );
		Delete_Module( &mod );
		break;
	case INPUT_TYPE_XM:
		Load_XM( &mod, verbose );
		MSL_PrintDefinition( filename, MSL_AddModule( &mod ), "MOD_" );
		Delete_Module( &mod );
		break;
	case INPUT_TYPE_IT:
		Load_IT( &mod, verbose );
		MSL_PrintDefinition( filename, MSL_AddModule( &mod ), "MOD_" );
		Delete_Module( &mod );
		break;
	case INPUT_TYPE_WAV:
		Load_WAV( &wav, verbose, true );
		wav.filename[0] = '#';	// set SFX flag (for demo)
		MSL_PrintDefinition( filename, MSL_AddSample( &wav ), "SFX_" );
		free( wav.data );
		break;
	default:
		// print error/warning
		printf( "Unknown file %s...\n", filename );
	}
	mm_file_close_read();
	
}

int MMUTIL_Create( char* argv[], int argc, char* output, char* header, char* ini, bool verbose, int start_sample, int start_mod )
{
//	int str_w=0;
//	u8 pmode=0;
//	bool comment=false;

	if( start_sample < 0 )
	{
		printf( "Ignoring invalid start sample %d\n", start_sample );
		start_sample = 0;
	}

	if( start_mod < 0 )
	{
		printf( "Ignoring invalid start sample %d\n", start_mod );
		start_mod = 0;
	}

	int x;

	MSL_Erase( start_sample, start_mod );
	str_msl[0] = 0;

	F_HEADER=NULL;
	if( header )
	{
		F_HEADER = fopen( header, "wb" );
	}

	F_INI=NULL;
	if( ini )
	{
		F_INI = fopen( ini, "wb" );
		INI_STATE = '\0';
	}

//	if( !F_HEADER )
//		return -1;	// needs header file!
	
	mm_file_open_write( TMP_SAMP );
	mm_file_close_write();
	mm_file_open_write( TMP_SONG );
	mm_file_close_write();
	
	for( x = 0; x < argc; x++ )
	{
		if( argv[x][0] == '-' )
		{
			
		}
		else
		{
			MSL_LoadFile( argv[x], verbose );
		}
	}

	if(MSL_Export( output ) != ERR_NONE)
	{
		mm_file_delete( TMP_SAMP );
		mm_file_delete( TMP_SONG );
		printf( "Error: could not open output file [%s] for writing\n", output );
		return FILE_OPEN_ERROR;
	}

	if( F_HEADER )
	{
		if( MSL_SAMPS_START )
		{
			printf( "Warning: header written, but this soundbank is designed for use as an extension bank." );
		}

		fprintf( F_HEADER, "#define MSL_NSONGS	%i\r\n", MSL_NSONGS );
		fprintf( F_HEADER, "#define MSL_NSAMPS	%i\r\n", MSL_NSAMPS );
		fprintf( F_HEADER, "#define MSL_BANKSIZE	%i\r\n", (MSL_NSAMPS+MSL_NSONGS) );
		fclose( F_HEADER );
		F_HEADER=NULL;
	}

	if( F_INI )
	{
		fprintf( F_INI, "\n" );
		fprintf( F_INI, "[header]\n" );
		fprintf( F_INI, "sample-start = %d\n", (int)MSL_SAMPS_START);
		fprintf( F_INI, "sample-count = %d\n", (int)(MSL_NSAMPS - MSL_SAMPS_START));
		fprintf( F_INI, "module-start = %d\n", (int)MSL_SONGS_START);
		fprintf( F_INI, "module-count = %d\n", (int)(MSL_NSONGS - MSL_SONGS_START));
		fclose( F_INI );
		F_INI=NULL;
	}

	mm_file_delete( TMP_SAMP );
	mm_file_delete( TMP_SONG );
	return ERR_NONE;
}
