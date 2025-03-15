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

#ifndef SIMPLE_H
#define SIMPLE_H

#define INPUT_TYPE_MOD	0
#define INPUT_TYPE_S3M	1
#define INPUT_TYPE_XM	2
#define INPUT_TYPE_IT	3
#define INPUT_TYPE_WAV	4
#define INPUT_TYPE_TXT	5
#define INPUT_TYPE_UNK	6
#define INPUT_TYPE_H	7
#define INPUT_TYPE_MSL	8

int get_ext( char* filename );
u32 calc_samplooplen( Sample* s );
u32 calc_samplen( Sample* s );
u32 calc_samplen_ex2( Sample* s );
int clamp_s8( int value );
int clamp_u8( int value );
u32 readbits(u8* buffer, unsigned int pos, unsigned int size);

u8 sample_dsformat( Sample* samp );
u8 sample_dsreptype( Sample* samp );

#endif
