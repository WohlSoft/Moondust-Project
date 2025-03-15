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

#ifndef MAS_H
#define MAS_H

//#define SAMPLE_FORMAT_U8		3
//#define SAMPLE_FORMAT_S8		0
//#define SAMPLE_FORMAT_S16		1
//#define SAMPLE_FORMAT_ADPCM		2

#define SAMPF_16BIT			0x001
#define SAMPF_SIGNED		0x002
#define SAMPF_COMP			0x004

#define SAMP_FORMAT_U8		(0 )
#define SAMP_FORMAT_U16		(SAMPF_16BIT)
#define SAMP_FORMAT_S8		( SAMPF_SIGNED  )
#define SAMP_FORMAT_S16		(SAMPF_16BIT | SAMPF_SIGNED )
#define SAMP_FORMAT_ADPCM	(SAMPF_COMP)

#define MAS_TYPE_SONG			0
#define MAS_TYPE_SAMPLE_GBA		1
#define MAS_TYPE_SAMPLE_NDS		2

typedef struct tInstrument_Envelope
{
	u8	loop_start;
	u8	loop_end;
	u8	sus_start;
	u8	sus_end;
	u8	node_count;
	u16	node_x[25];
	u8	node_y[25];
	bool env_filter;
	bool env_valid;
	bool env_enabled;
} Instrument_Envelope;

typedef struct tInstrument
{
	u32		parapointer;

	u8		global_volume;
	u8		setpan;
	u16		fadeout;
	u8		random_volume;
	u8		nna;
	u8		dct;
	u8		dca;
	u8		env_flags;
	u16		notemap[120];

	char	name[32];

	Instrument_Envelope		envelope_volume;
	Instrument_Envelope		envelope_pan;
	Instrument_Envelope		envelope_pitch;
} Instrument;

typedef struct tSample
{
	u32		parapointer;
	
	u8		global_volume;
	u8		default_volume;
	u8		default_panning;
	u32		sample_length;
	u32		loop_start;
	u32		loop_end;
	u8		loop_type;
	u32		frequency;
	void*	data;
	
	u8		vibtype;
	u8		vibdepth;
	u8		vibspeed;
	u8		vibrate;
	u16		msl_index;
	u8		rsamp_index;
	
	u8		format;
//	bool	samp_signed;
	
	// file info
	u32		datapointer;
//	bool	bit16;
//	bool	samp_unsigned;
	u8		it_compression;
	char	name[32];
	char	filename[12];
} Sample;

typedef struct tPatternEntry
{
	u8		note;
	u8		inst;
	u8		vol;
	u8		fx;
	u8		param;
} PatternEntry;

typedef struct tPattern
{
	u32		parapointer;
	u16		nrows;
	int		clength;
	PatternEntry	data[MAX_CHANNELS*256];
	bool	cmarks[256];
} Pattern;

typedef struct tMAS_Module
{
	char	title[32];
	u16		order_count;
	u8		inst_count;
	u8		samp_count;
	u8		patt_count;
	u8		restart_pos;
	bool	stereo;
	bool	inst_mode;
	u8		freq_mode;
	bool	old_effects;
	bool	link_gxx;
	bool	xm_mode;
	bool	old_mode;
	u8		global_volume;
	u8		initial_speed;
	u8		initial_tempo;
	u8		channel_volume[MAX_CHANNELS];
	u8		channel_panning[MAX_CHANNELS];
	u8		orders[256];
	Instrument*	instruments;
	Sample*		samples;
	Pattern*	patterns;
} MAS_Module;

void Write_Instrument_Envelope( Instrument_Envelope* env );
void Write_Instrument( Instrument* inst );
void Write_SampleData( Sample* samp );
void Write_Sample( Sample* samp );
void Write_Pattern( Pattern* patt, bool xm_vol );
int Write_MAS( MAS_Module* mod, bool verbose, bool msl_dep );
void Delete_Module( MAS_Module* mod );

extern u32 MM_MAS_FILESIZE;

#endif
