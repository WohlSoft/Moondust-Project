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

#include "defs.h"
#include "files.h"

static FILE *fin;
static FILE *fout;

static int file_byte_count;

bool mm_file_exists( char* filename )
{
	fin = fopen( filename, "rb" );
	if( !fin )
		return false;
	mm_file_close_read();
	return true;
}

int mm_file_size( char* filename )
{
	FILE* f;
	int a;
	f = fopen( filename, "rb" );
	if( !f )
		return 0;
	fseek( f, 0, SEEK_END );
	a = ftell( f );
	fclose( f );
	return a;
}

int mm_file_open_read( char* filename )
{
	fin = fopen( filename, "rb" );
	if( !fin )
		return FILE_OPEN_ERROR;
	return FILE_OPEN_OKAY;
}

int mm_file_open_write( char* filename )
{
	fout = fopen( filename, "wb" );
	if( !fout )
		return FILE_OPEN_ERROR;
	return FILE_OPEN_OKAY;
}

int mm_file_open_write_end( char* filename )
{
	fout = fopen( filename, "r+b" );
	if( !fout )
		return FILE_OPEN_ERROR;
	fseek( fout, 0, SEEK_END );
	return FILE_OPEN_OKAY;
}

void mm_file_close_read( void )
{
	fclose( fin );
}

void mm_file_close_write( void )
{
	fclose( fout );
}

int mm_file_seek_read( int offset, int mode )
{
	return fseek( fin, offset, mode );
}

int mm_file_seek_write( int offset, int mode )
{
	return fseek( fout, offset, mode );
}

unsigned int mm_file_tell_read( void )
{
	return (unsigned int)ftell( fin );
}

unsigned int mm_file_tell_write( void )
{
	return (unsigned int)ftell( fout );
}

int mm_file_tell_size( void )
{
	int pos = ftell(fin);
	fseek( fin, 0, SEEK_END );
	int size = ftell(fin);
	fseek(fin, pos, SEEK_SET );
	return size;
}

u8 mm_read8( void )
{
	u8 a;
	fread( &a, 1, 1, fin );
	return a;
}

u16 mm_read16( void )
{
	u16 a;
	a  =	mm_read8();
	a |=	((u16)mm_read8()) << 8;
	return a;
}

u32 mm_read24( void )
{
	u32 a;
	a  =	mm_read8();
	a |=	((u32)mm_read8())<<8;
	a |=	((u32)mm_read8())<<16;
	return a;
}

u32 mm_read32( void )
{
	u32 a;
	a  =	mm_read16();
	a |=	((u32)mm_read16()) << 16;
	return a;
}

u8 mm_read8f( FILE* p_fin )
{
	u8 a;
	fread( &a, 1, 1, p_fin );
	return a;
}

u16 mm_read16f( FILE* p_fin )
{
	u16 a;
	a  =	mm_read8f( p_fin );
	a |=	((u16)mm_read8f( p_fin )) << 8;
	return a;
}

u32 mm_read32f( FILE* p_fin )
{
	u32 a;
	a  =	mm_read16f( p_fin );
	a |=	((u32)mm_read16f( p_fin )) << 16;
	return a;
}

void mm_write8( u8 p_v )
{
	fwrite( &p_v, 1, 1, fout );
	file_byte_count++;
}

void mm_write16( u16 p_v )
{
	mm_write8( (u8)(p_v & 0xFF) );
	mm_write8( (u8)(p_v >> 8) );
	file_byte_count += 2;
}

void mm_write24( u32 p_v )
{
	mm_write8( (u8)(p_v & 0xFF) );
	mm_write8( (u8)((p_v >> 8) & 0xFF) );
	mm_write8( (u8)((p_v >> 16) & 0xFF) );
	file_byte_count += 3;
}

void mm_write32( u32 p_v )
{
	mm_write16( (u16)(p_v & 0xFFFF) );
	mm_write16( (u16)(p_v >> 16) );
	file_byte_count += 4;
}

void mm_align16( void )
{
	if( ftell( fout ) & 1 )
		mm_write8( BYTESMASHER );
}

void mm_align32( void )
{
	if( ftell( fout ) & 3 )
		mm_write8( BYTESMASHER );
	if( ftell( fout ) & 3 )
		mm_write8( BYTESMASHER );
	if( ftell( fout ) & 3 )
		mm_write8( BYTESMASHER );
}

void mm_align32f( FILE* p_file )
{
	if( ftell( p_file ) & 3 )
		mm_write8( BYTESMASHER );
	if( ftell( p_file ) & 3 )
		mm_write8( BYTESMASHER );
	if( ftell( p_file ) & 3 )
		mm_write8( BYTESMASHER );
}

void mm_skip8( u32 count )
{
	fseek( fin, count, SEEK_CUR );
//	while( count )
//	{
//		//mm_read8();
//		count--;
//	}
}

void mm_skip8f( u32 count, FILE* p_file )
{
	fseek( p_file, count, SEEK_CUR );
//	while( count )		// this was a major slowdown!
//	{
//		//mm_read8f( p_file );
//		
//		count--;
//	}
}
 
void mm_file_delete( char* filename )
{
	if( mm_file_exists( filename ) )
		remove( filename );
}

int mm_file_get_byte_count( )
{
	int a = file_byte_count;
	file_byte_count = 0;
	return a;
}
