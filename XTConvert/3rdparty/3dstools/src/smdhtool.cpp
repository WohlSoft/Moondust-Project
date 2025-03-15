/*
 * smdhtool.c
 * Copyright (C) 2014 - plutoo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "lodepng/lodepng.h"
#ifdef WIN32
#include <wchar.h>
#define osmain wmain
#define oschar wchar_t
#define osfopen _wfopen
#define osfprintf fwprintf
#define osstrcmp wcscmp
#define osstr(_n) L##_n
#else
#define osmain main
#define oschar char
#define osfopen fopen
#define osfprintf fprintf
#define osstrcmp strcmp
#define osstr(_n) _n
#endif

#define SMDH_MAGIC "SMDH"

enum
{
	LANG_JAPANESE,
	LANG_ENGLISH,
	LANG_GERMAN,
	LANG_ITALIAN,
	LANG_SPANISH,
	LANG_SIMPLIFIED_CHINESE,
	LANG_KOREAN,
	LANG_DUTCH,
	LANG_PORTUGESE,
	LANG_RUSSIAN,
	LANG_TRADITIONAL_CHINESE,
};

enum
{
	REGION_JAPAN     = BIT(0),
	REGION_USA       = BIT(1),
	REGION_EUROPE    = BIT(2),
	REGION_AUSTRALIA = BIT(3),
	REGION_CHINA     = BIT(4),
	REGION_TAIWAN    = BIT(5),
	REGION_KOREA     = BIT(6),
	REGION_ALL       = 0xFFFFFFFF,
};

enum
{
	RATING_CERO      = 0,
	RATING_ESRB      = 1,
	RATING_USK       = 3,
	RATING_PEGI_GEN  = 4,
	RATING_PEGI_PRT  = 6,
	RATING_PEGI_BBFC = 7,
	RATING_COB       = 8,
	RATING_GRB       = 9,
	RATING_CGSRR     = 10,
};

enum
{
	RATING_FLAG_NO_RESTRICTION = BIT(5),
	RATING_FLAG_RATING_PENDING = BIT(6),
	RATING_FLAG_MASK = RATING_FLAG_RATING_PENDING - 1,
	RATING_FLAG_ENABLED = BIT(7),
};

enum
{
	SMDH_VISIBLE                  = BIT(0),
	SMDH_AUTOBOOT                 = BIT(1),
	SMDH_USE_3D_EFFECT            = BIT(2),
	SMDH_REQUIRE_ACCEPT_EULA      = BIT(3),
	SMDH_AUTOSAVE_ON_EXIT         = BIT(4),
	SMDH_USE_EXTENDED_BANNER      = BIT(5),
	SMDH_REGION_RATING_USED       = BIT(6),
	SMDH_USE_SAVE_DATA            = BIT(7),
	SMDH_RECORD_USAGE             = BIT(8),
	SMDH_DISABLE_SAVE_DATA_BACKUP = BIT(9),
};

typedef struct {
	u16 short_desc[0x40];
	u16 long_desc[0x80];
	u16 publisher[0x40];
} smdh_title;

typedef struct {
	u8 ratings[0x10];
	u32 region_lockout;
	u8 matchmaker_id[0xC];
	u32 flags;
	u16 eula_ver;
	u16 zero;
	u32 optimal_bannerframe;
	u32 streetpass_id;
} smdh_settings;

typedef struct {
	u32 magic;
	u16 version;
	u16 zero;
	smdh_title titles[16];
	smdh_settings settings;
	u8 zero2[0x8];
} smdh_header;

// stolen shamelessly from 3ds_hb_menu
static const u8 tile_order[] =
{
	0, 1, 8, 9, 2, 3, 10, 11, 16, 17, 24, 25, 18, 19, 26, 27,
	4, 5, 12, 13, 6, 7, 14, 15, 20, 21, 28, 29, 22, 23, 30, 31,
	32, 33, 40, 41, 34, 35, 42, 43, 48, 49, 56, 57, 50, 51, 58, 59,
	36, 37, 44, 45, 38, 39, 46, 47, 52, 53, 60, 61, 54, 55, 62, 63
};

void usage(oschar* argv[])
{
	osfprintf(stderr,
		osstr("USAGE:\n"
		"%s --create <name> <long description> <author> <icon.png> <outfile> [<smallicon.png>]\n"
		"\n"
		"FLAGS:\n"
		"    --create : Create SMDH for use with 3DS homebrew applications.\n"),
		argv[0]);
	exit(1);
}

#ifndef WIN32

static int decode_utf8(u32 *out, const char *in)
{
	u8 code1, code2, code3, code4;

	code1 = *in++;
	if(code1 < 0x80)
	{
		/* 1-byte sequence */
		*out = code1;
		return 1;
	}
	else if(code1 < 0xC2)
		return -1;
	else if(code1 < 0xE0)
	{
		/* 2-byte sequence */
		code2 = *in++;
		if((code2 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 6) + code2 - 0x3080;
		return 2;
	}
	else if(code1 < 0xF0)
	{
		/* 3-byte sequence */
		code2 = *in++;
		if((code2 & 0xC0) != 0x80)
			return -1;
		if(code1 == 0xE0 && code2 < 0xA0)
			return -1;

		code3 = *in++;
		if((code3 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 12) + (code2 << 6) + code3 - 0xE2080;
		return 3;
	}
	else if(code1 < 0xF5)
	{
		/* 4-byte sequence */
		code2 = *in++;
		if((code2 & 0xC0) != 0x80)
			return -1;
		if(code1 == 0xF0 && code2 < 0x90)
			return -1;
		if(code1 == 0xF4 && code2 >= 0x90)
			return -1;

		code3 = *in++;
		if((code3 & 0xC0) != 0x80)
			return -1;

		code4 = *in++;
		if((code4 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 18) + (code2 << 12) + (code3 << 6) + code4 - 0x3C82080;
		return 4;
	}

	return -1;
}

static int encode_utf16(u16 *out, u32 in)
{
	if(in < 0x10000)
	{
		if(out != NULL)
			*out++ = le_hword(in);
		return 1;
	}
	else if(in < 0x110000)
	{
		if(out != NULL)
		{
			*out++ = le_hword((in >> 10) + 0xD7C0);
			*out++ = le_hword((in & 0x3FF) + 0xDC00);
		}
		return 2;
	}

	return -1;
}

int utf8_to_utf16(u16 *out, const char *in, u32 len)
{
	int rc = 0;
	int units;
	u32 code;
	u16 encoded[2];

	do
	{
		units = decode_utf8(&code, in);
		if(units == -1)
			return -1;

		if(code > 0)
		{
			in += units;

			units = encode_utf16(encoded, code);
			if(units == -1)
				return -1;

			if(out != NULL)
			{
				if(rc + units <= len)
				{
					*out++ = encoded[0];
					if(units > 1)
						*out++ = encoded[1];
				}
			}

			rc += units;
		}
	} while(code > 0);

	return rc;
}

#endif

u16 conv_argb_to_rgb565(u8 a, u8 r, u8 g, u8 b)
{
	r = 1.0f*r*a/255.0f;
	g = 1.0f*g*a/255.0f;
	b = 1.0f*b*a/255.0f;

	r = (r >> 3);
	g = (g >> 2);
	b = (b >> 3);

	return (r << 11) | (g << 5) | b;
}

u8 blend_color(u8 a, u8 b, u8 c, u8 d)
{
	int x = 0;
	x += a;
	x += b;
	x += c;
	x += d;
	return (x+2)/4;
}

static unsigned char* load_png(const oschar* icon, int side)
{
	unsigned char* img;
	unsigned int width, height;
	int rc;

	FILE* f = osfopen(icon, osstr("rb"));
	if (!f) {
		osfprintf(stderr, osstr("Could not load PNG: %s\n"), icon);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	u32 size = ftell(f);
	rewind(f);

	void* mem = malloc(size);
	if (!mem) {
		fclose(f);
		osfprintf(stderr, osstr("Mem alloc error.\n"));
		return NULL;
	}

	fread(mem, 1, size, f);
	fclose(f);

	rc = lodepng_decode32(&img, &width, &height, (const unsigned char*)mem, size);
	free(mem);
	if(rc) {
		osfprintf(stderr, osstr("Could not decode PNG: %s\n"), icon);
		return NULL;
	}

	if(width != side || height != side) {
		osfprintf(stderr, osstr("Icon size is incorrect (expected %dx%d "
		                  "pixels, got %dx%d instead).\n"),
		          side, side, width, height);
		free(img);
		return NULL;
	}

	return img;
}

int convert_png_to_icon(FILE* fd, const oschar* icon, const oschar* icon2)
{
	unsigned char* img;

	img = load_png(icon, 48);
	if (!img)
		return 1;

	u16 large_icon[48*48];
	u16 small_icon[24*24];
	unsigned int x, y, xx, yy, k;
	unsigned int n = 0;

	for(y=0; y<48; y+=8) {
		for(x=0; x<48; x+=8) {
			for(k=0; k<8*8; k++) {
				xx = (tile_order[k] & 0x7);
				yy = (tile_order[k] >> 3);

				u8* rgba = &img[4*(48*(y+yy) + (x+xx))];
				u8 r = rgba[0];
				u8 g = rgba[1];
				u8 b = rgba[2];
				u8 a = rgba[3];

				large_icon[n++] = le_hword(conv_argb_to_rgb565(a, r, g, b));
			}
		}
	}

	if (icon2)
	{
		free(img);
		img = load_png(icon2, 24);
		if (!img)
			return 1;
	}

	n = 0;
	for(y=0; y<24; y+=8) {
		for(x=0; x<24; x+=8) {
			for(k=0; k<8*8; k++) {
				xx = (tile_order[k] & 0x7);
				yy = (tile_order[k] >> 3);

				u8 r,g,b,a;

				if (icon2) {
					u8* rgba = &img[4*(24*(y+yy) + (x+xx))];

					r = rgba[0];
					g = rgba[1];
					b = rgba[2];
					a = rgba[3];
				} else {
					u8* rgba0 = &img[4*(48*2*(y+yy)     + 2*(x+xx))];
					u8* rgba1 = &img[4*(48*2*(y+yy)     + 2*(x+xx))];
					u8* rgba2 = &img[4*(48*(2*(y+yy)+1) + 2*(x+xx)+1)];
					u8* rgba3 = &img[4*(48*(2*(y+yy)+1) + 2*(x+xx)+1)];

					r = blend_color(rgba0[0], rgba1[0], rgba2[0], rgba3[0]);
					g = blend_color(rgba0[1], rgba1[1], rgba2[1], rgba3[1]);
					b = blend_color(rgba0[2], rgba1[2], rgba2[2], rgba3[2]);
					a = blend_color(rgba0[3], rgba1[3], rgba2[3], rgba3[3]);
				}

				small_icon[n++] = le_hword(conv_argb_to_rgb565(a, r, g, b));
			}
		}
	}

	fwrite(small_icon, sizeof(small_icon), 1, fd);
	fwrite(large_icon, sizeof(large_icon), 1, fd);

	free(img);
	return 0;
}

int create_hb_banner(oschar* argv[])
{
	smdh_header hdr;
	memset(&hdr, 0, sizeof(hdr));

	memcpy(&hdr.magic, SMDH_MAGIC, 4);
	hdr.version = 0;

	size_t i;
	for(i=0; i<16; i++) {
#ifndef WIN32
		utf8_to_utf16(hdr.titles[i].short_desc, argv[2], 0x40);
		utf8_to_utf16(hdr.titles[i].long_desc,  argv[3], 0x80);
		utf8_to_utf16(hdr.titles[i].publisher,  argv[4], 0x40);
#else
		wcsncpy((oschar*)hdr.titles[i].short_desc, argv[2], 0x40);
		wcsncpy((oschar*)hdr.titles[i].long_desc,  argv[3], 0x80);
		wcsncpy((oschar*)hdr.titles[i].publisher,  argv[4], 0x40);
#endif
	}

	// Initialize flags
	hdr.settings.flags                     = le_word(SMDH_VISIBLE | SMDH_RECORD_USAGE | SMDH_REGION_RATING_USED);
	hdr.settings.region_lockout            = le_word(REGION_ALL);
	hdr.settings.ratings[RATING_CERO]      = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_ESRB]      = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_USK]       = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_PEGI_GEN]  = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_PEGI_PRT]  = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_PEGI_BBFC] = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_COB]       = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_GRB]       = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;
	hdr.settings.ratings[RATING_CGSRR]     = RATING_FLAG_ENABLED | RATING_FLAG_NO_RESTRICTION;

	FILE* fd = osfopen(argv[6], osstr("wb"));
	if(fd == NULL) {
		perror("fopen");
		return 1;
	}

	fwrite(&hdr, sizeof(hdr), 1, fd);
	if(convert_png_to_icon(fd, argv[5], argv[7]) != 0) {
		fclose(fd);
		return 1;
	}

	fclose(fd);
	return 0;
}

int osmain(int argc, oschar* argv[])
{
	if(argc < 2)
		usage(argv);

	if(osstrcmp(argv[1], osstr("--create")) == 0) {
		if(argc != 7 && argc != 8) {
			fprintf(stderr, "Expected 6 or 7 args.\n");
			return 1;
		}

		return create_hb_banner(argv);
	}
	else usage(argv);

	return 0;
}

#ifdef WIN32
// Code taken from http://www.coderforlife.com/projects/utilities/mingw-unicode.c
extern int _CRT_glob;
extern "C" void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*);

int main(int xargc, char** xargv) {
	wchar_t **enpv, **argv;
	int argc, si = 0;

	__wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si); // this also creates the global variable __wargv
	return wmain(argc, argv);
}
#endif
