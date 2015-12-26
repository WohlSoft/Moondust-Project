// File_Extractor 0.4.0. http://www.slack.net/~ant/

#include "Data_Reader.h"

#include "blargg_endian.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* Copyright (C) 2005-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#include "blargg_source.h"

#ifdef HAVE_ZLIB_H
#include "ZLib/zlib.h"
#endif

const char Data_Reader::eof_error [] = "Unexpected end of file";

blargg_err_t Data_Reader::read( void* p, long s )
{
	long result = read_avail( p, s );
	if ( result != s )
	{
		if ( result >= 0 && result < s )
			return eof_error;
		
		return "Read error";
	}
	
	return 0;
}

blargg_err_t Data_Reader::skip( long count )
{
	char buf [512];
	while ( count )
	{
		long n = sizeof buf;
		if ( n > count )
			n = count;
		count -= n;
		RETURN_ERR( read( buf, n ) );
	}
	return 0;
}

long File_Reader::remain() const { return size() - tell(); }

blargg_err_t File_Reader::skip( long n )
{
	assert( n >= 0 );
	if ( !n )
		return 0;
	return seek( tell() + n );
}

// Subset_Reader

Subset_Reader::Subset_Reader( Data_Reader* dr, long size )
{
	in = dr;
	remain_ = dr->remain();
	if ( remain_ > size )
		remain_ = size;
}

long Subset_Reader::remain() const { return remain_; }

long Subset_Reader::read_avail( void* p, long s )
{
	if ( s > remain_ )
		s = remain_;
	remain_ -= s;
	return in->read_avail( p, s );
}

// Remaining_Reader

Remaining_Reader::Remaining_Reader( void const* h, long size, Data_Reader* r )
{
	header = (char const*) h;
	header_end = header + size;
	in = r;
}

long Remaining_Reader::remain() const { return header_end - header + in->remain(); }

long Remaining_Reader::read_first( void* out, long count )
{
	long first = header_end - header;
	if ( first )
	{
		if ( first > count )
			first = count;
		void const* old = header;
		header += first;
		memcpy( out, old, first );
	}
	return first;
}

long Remaining_Reader::read_avail( void* out, long count )
{
	long first = read_first( out, count );
	long second = count - first;
	if ( second )
	{
		second = in->read_avail( (char*) out + first, second );
		if ( second <= 0 )
			return second;
	}
	return first + second;
}

blargg_err_t Remaining_Reader::read( void* out, long count )
{
	long first = read_first( out, count );
	long second = count - first;
	if ( !second )
		return 0;
	return in->read( (char*) out + first, second );
}






// Mem_File_Reader

Mem_File_Reader::Mem_File_Reader( const void* p, long s ) :
	begin( (const char*) p ),
	size_( s )
{
	pos = 0;
}
	
long Mem_File_Reader::size() const { return size_; }

long Mem_File_Reader::read_avail( void* p, long s )
{
	long r = remain();
	if ( s > r )
		s = r;
	memcpy( p, begin + pos, s );
	pos += s;
	return s;
}

long Mem_File_Reader::tell() const { return pos; }

blargg_err_t Mem_File_Reader::seek( long n )
{
	if ( n > size_ )
		return eof_error;
	pos = n;
	return 0;
}

#if 0 //HAVE_ZLIB_H

//TODO: Implement a support of GZIP-compressed range of memory

// GZipMem_File_Reader
GZipMem_File_Reader::GZipMem_File_Reader( const void* p, long s ) :
    begin_compressed( (const char*) p ),
    size_compressed_( s ),
    isGZIP(false)
{
    if( (size_compressed_>4) && (get_be32( begin_compressed )==BLARGG_4CHAR(0x1f,0x8b,0x08, 0)) )
    {
        //Just a copy-pasta from ZLib howo. This crap is unfinished and Unbuildable
        /*
        #define CHUNK 16384
        int ret;
        unsigned have;
        z_stream strm;
        char in[CHUNK];
        char out[CHUNK];
        // allocate inflate state
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        ret = inflateInit(&strm);
        if (ret != Z_OK)
            goto itisNonoGzip;
        // decompress until deflate stream ends or end of file
        do
        {
            strm.avail_in = fread(in, 1, CHUNK, source);
            if (ferror(source)) {
                (void)inflateEnd(&strm);
                goto itisNonoGzip;
            }
            if (strm.avail_in == 0)
                break;
            strm.next_in = in;
            // run inflate() on input until output buffer not full
            do
            {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = inflate(&strm, Z_NO_FLUSH);
                assert(ret != Z_STREAM_ERROR);  // state not clobbered
                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     // and fall through
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
                }
                have = CHUNK - strm.avail_out;
                if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                    (void)inflateEnd(&strm);
                    goto itisNonoGzip;
                }
            } while (strm.avail_out == 0);
            // done when inflate() says it's done
        } while (ret != Z_STREAM_END);
        */
    }
    else
    {
        itisNonoGzip:
        //Bind raw data if impossible to ucompress or if it is not a GZip
        begin = begin_compressed;
        size_ = size_compressed_;
    }
    pos = 0;
}

GZipMem_File_Reader::~GZipMem_File_Reader()
{
    if(isGZIP) free(begin);
}

long GZipMem_File_Reader::size() const { return size_; }

long GZipMem_File_Reader::read_avail( void* p, long s )
{
    long r = remain();
    if ( s > r )
        s = r;
    memcpy( p, begin + pos, s );
    pos += s;
    return s;
}

long GZipMem_File_Reader::tell() const { return pos; }

blargg_err_t GZipMem_File_Reader::seek( long n )
{
    if ( n > size_ )
        return eof_error;
    pos = n;
    return 0;
}
#endif






// Callback_Reader

Callback_Reader::Callback_Reader( callback_t c, long size, void* d ) :
	callback( c ),
	data( d )
{
	remain_ = size;
}

long Callback_Reader::remain() const { return remain_; }

long Callback_Reader::read_avail( void* out, long count )
{
	if ( count > remain_ )
		count = remain_;
	if ( Callback_Reader::read( out, count ) )
		count = -1;
	return count;
}

blargg_err_t Callback_Reader::read( void* out, long count )
{
	if ( count > remain_ )
		return eof_error;
	return callback( data, out, count );
}

// Std_File_Reader

Std_File_Reader::Std_File_Reader() : file_( 0 ) { }

Std_File_Reader::~Std_File_Reader() { close(); }

blargg_err_t Std_File_Reader::open( const char* path )
{
	file_ = fopen( path, "rb" );
	if ( !file_ )
		return "Couldn't open file";
	return 0;
}

long Std_File_Reader::size() const
{
	long pos = tell();
	fseek( (FILE*) file_, 0, SEEK_END );
	long result = tell();
	fseek( (FILE*) file_, pos, SEEK_SET );
	return result;
}

long Std_File_Reader::read_avail( void* p, long s )
{
	return fread( p, 1, s, (FILE*) file_ );
}

blargg_err_t Std_File_Reader::read( void* p, long s )
{
	if ( s == (long) fread( p, 1, s, (FILE*) file_ ) )
		return 0;
	if ( feof( (FILE*) file_ ) )
		return eof_error;
	return "Couldn't read from file";
}

long Std_File_Reader::tell() const { return ftell( (FILE*) file_ ); }

blargg_err_t Std_File_Reader::seek( long n )
{
	if ( !fseek( (FILE*) file_, n, SEEK_SET ) )
		return 0;
	if ( n > size() )
		return eof_error;
	return "Error seeking in file";
}

void Std_File_Reader::close()
{
	if ( file_ )
	{
		fclose( (FILE*) file_ );
		file_ = 0;
	}
}

// Gzip_File_Reader

#ifdef HAVE_ZLIB_H

static const char* get_gzip_eof( const char* path, long* eof )
{
	FILE* file = fopen( path, "rb" );
	if ( !file )
		return "Couldn't open file";
	
	unsigned char buf [4];
    if ( fread( buf, 2, 1, file ) > 0 && buf [0] == 0x1F && buf [1] == 0x8B )
	{
		fseek( file, -4, SEEK_END );
		fread( buf, 4, 1, file );
		*eof = get_le32( buf );
	}
	else
	{
		fseek( file, 0, SEEK_END );
		*eof = ftell( file );
	}
	const char* err = (ferror( file ) || feof( file )) ? "Couldn't get file size" : 0;
	fclose( file );
	return err;
}

Gzip_File_Reader::Gzip_File_Reader() : file_( 0 ) { }

Gzip_File_Reader::~Gzip_File_Reader() { close(); }

blargg_err_t Gzip_File_Reader::open( const char* path )
{
	close();
	
	RETURN_ERR( get_gzip_eof( path, &size_ ) );
	
	file_ = gzopen( path, "rb" );
	if ( !file_ )
		return "Couldn't open file";
	
	return 0;
}

long Gzip_File_Reader::size() const { return size_; }

long Gzip_File_Reader::read_avail( void* p, long s ) { return gzread( file_, p, s ); }

long Gzip_File_Reader::tell() const { return gztell( file_ ); }

blargg_err_t Gzip_File_Reader::seek( long n )
{
	if ( gzseek( file_, n, SEEK_SET ) >= 0 )
		return 0;
	if ( n > size_ )
		return eof_error;
	return "Error seeking in file";
}

void Gzip_File_Reader::close()
{
	if ( file_ )
	{
		gzclose( file_ );
		file_ = 0;
	}
}

#endif
