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

/* NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE 
  SOME CODE WAS STOLEN FROM NDSTOOL.CPP
NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE  */

#include <stdlib.h>
#include "defs.h"
#include "files.h"
#include "mas.h"

#include "arm7_st.inc"
#include "arm9_st.inc"

const u8 nds_logo[];

#define ARM7_SIZE sizeof(arm7_bin)
#define ARM9_SIZE sizeof(arm9_bin)

const unsigned short crc16tab[] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

void Write_NDS( void )
{
	const char game_title[] = "SOUND TEST ";

	for( unsigned int x = 0; x < 12; x++ )
		mm_write8( game_title[x] );

	mm_write32( '####' );	// gamecode
	mm_write16( 0 );	// makercode
	mm_write8( 0 );	// unit code (Nintendo DS)
	mm_write8( 0 );	// encryption seed
	mm_write8( 0 );	// device capability
	for( unsigned int x = 0; x < 9; x++ )
		mm_write8( 0 );	// reserved
	mm_write8( 0 );	// rom version
	mm_write8( 4 );	// autostart

//-----------------------------------------------------------
	mm_write32( 512+32768 ); // arm9 ROM offset
	mm_write32( 0x2000000 ); // arm9 entry point
	mm_write32( 0x2000000 ); // arm9 ram address
	mm_write32( 65536 ); // arm9 binary size (modify later)
//-----------------------------------------------------------
	mm_write32( 512 );			// arm7 ROM offset
	mm_write32( 0x37f8000 );	// arm7 entry point
	mm_write32( 0x37f8000 );	// arm7 ram address
	mm_write32( 32768 );	// arm7 binary size
//-----------------------------------------------------------
	
	mm_write32( 0 );			// fnt address
	mm_write32( 0 );			// fnt size
	mm_write32( 0 );			// fat address
	mm_write32( 0 );			// fat size

	mm_write32( 0 );			// file arm9 overlay address
	mm_write32( 0 );			// file arm9 overlay size
	mm_write32( 0 );			// file arm7 overlay address
	mm_write32( 0 );			// file arm7 overlay size
	
	mm_write32( 0x7f7fff );	// port 40001a4 setting for normal commands
	mm_write32( 0x203f1fff );	// port 40001a4 setting for KEY1 commands
	
	mm_write32( 0 );			// icon (no icon)
	
	mm_write16( 0 );			// secure area checksum
	mm_write16( 0x51e );		// secure area loading timeout
	mm_write32( 0 );			// arm9 auto load list RAM address (?)
	mm_write32( 0 );			// arm7 auto load list RAM address (?)
	mm_write32( 0 );			// secure area disable
	mm_write32( 0 );			// secure area disable
	
	mm_write32( 0x14400 );		// total used ROM size
	mm_write32( 0x200 );		// rom header size
	
	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 'MARS' );
	mm_write32( '11V_' );
	mm_write8( '0' );
	mm_write8( 0 );mm_write8( 0 );mm_write8( 0 );
	mm_write32( 'SSAP' );
	mm_write8( '0' );mm_write8( '1' );mm_write8( 0x96 );mm_write8( 0 );

	mm_write32( 0 );
	mm_write32( 0 );
	mm_write32( 0 );

	for( unsigned int x = 0; x < 0x9c; x++ )
		mm_write8( nds_logo[x] );		// NINTENDO LOGO

	mm_write16( 0x9e1a );		// nintendo logo checksum
	mm_write16( 0 );			// header checksum (modify later)
	mm_write32( 0 );			// debug rom offset
	mm_write32( 0 );			// debug rom size
	mm_write32( 0 );			// debug ram address

	mm_write32( 0 );			// reserved

	for( unsigned int x = 0; x < 0x90; x++ )
		mm_write8( 0 );

	// write binaries
	for( unsigned int x = 0; x < ARM7_SIZE; x++ )
		mm_write8( arm7_bin[x] );
	for( unsigned int x = 0; x < 32768-ARM7_SIZE; x++ )	// pad to 32k
		mm_write8( (u8)x );
	for( unsigned int x = 0; x < ARM9_SIZE; x++ )
		mm_write8( arm9_bin[x] );
	for( unsigned int x = 0; x < 65536-ARM9_SIZE; x++ )	// pad to 64k
		mm_write8( (u8)x );
}

unsigned short calculate_crc( void )
{
	int i;
	unsigned short crc = 0xFFFF;
	for ( i=0; i<0x15E; i++)
	{
		unsigned char data = mm_read8();
		crc = (crc >> 8) ^ crc16tab[(crc ^ data) & 0xFF];
	}
	return crc;
}

void Validate_NDS( char* filename, int output_size )
{
	int arm9_size;
	int header_crc;
	arm9_size = 65536 + output_size;
	mm_file_open_write_end( filename );
	mm_file_seek_write( 0x2c, SEEK_SET );
	mm_write32( arm9_size );
	mm_file_seek_write( 0x80, SEEK_SET );
	mm_write32( arm9_size+32768 );		// total used rom size
	mm_file_close_write();

	mm_file_open_read( filename );
	header_crc=calculate_crc();
	mm_file_close_read();
	header_crc &= 0xFFFF;

	mm_file_open_write_end( filename );
	mm_file_seek_write( 0x15e, SEEK_SET );
	mm_write16( (u16)header_crc );
	mm_file_close_write();
	/*
	// write arm7 rom_offset
	mm_file_seek_write( 0x30, SEEK_SET );
	mm_write32( 512 );

	// write arm9 rom_offset
	mm_file_seek_write( 0x20, SEEK_SET );
	mm_write32( 512 + 0x4000 );

	// write arm9 size
	mm_file_seek_write( 0x2C, SEEK_SET );
	mm_write32( 0x10000 + output_size );
	*/
}

const u8 nds_logo[] = {
	0xC8, 0x60, 0x4F, 0xE2, 0x1, 0x70, 0x8F, 0xE2, 0x17, 0xFF, 0x2F, 0xE1, 0x12, 0x4F, 0x11, 0x48, 0x12, 0x4C, 0x20, 0x60, 0x64, 0x60, 0x7C, 0x62, 0x30, 0x1C, 0x39, 0x1C, 0x10, 0x4A, 0x0, 0xF0, 0x14, 0xF8, 
0x30, 0x6A, 0x80, 0x19, 0xB1, 0x6A, 0xF2, 0x6A, 0x0, 0xF0, 0xB, 0xF8, 0x30, 0x6B, 0x80, 0x19, 0xB1, 0x6B, 0xF2, 0x6B, 0x0, 0xF0, 0x8, 0xF8, 0x70, 0x6A, 0x77, 0x6B, 0x7, 0x4C, 0x60, 0x60, 0x38, 0x47, 0x7, 
0x4B, 0xD2, 0x18, 0x9A, 0x43, 0x7, 0x4B, 0x92, 0x8, 0xD2, 0x18, 0xC, 0xDF, 0xF7, 0x46, 0x4, 0xF0, 0x1F, 0xE5, 0x0, 0xFE, 0x7F, 0x2, 0xF0, 0xFF, 0x7F, 0x2, 0xF0, 0x1, 0x0, 0x0, 0xFF, 0x1, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, };
