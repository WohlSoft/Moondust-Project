/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#ifndef SPC2ITTYPES_H
#define SPC2ITTYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef double f64;

typedef s16 pcm_t;

#define OneKB (1024)

#define Mem64k (OneKB * 64)

typedef struct
{
	char magic[4]; // IMPM
	char songName[26];
	u16 PHiligt; // Pattern row hilight stuff
	u16 OrderNumber; // Number of orders in song
	u16 InstrumentNumber; // Number of instruments
	u16 SampleNumber; // Number of samples
	u16 PatternNumber; // Number of patterns
	u16 TrackerCreatorVersion; // The version of the tracker that created the IT file
	u16 TrackerFormatVersion; // Format version
	u16 Flags; // Information flags
	u16 Special; // Special st0ff
	u8 GlobalVolume; // Global volume
	u8 MixVolume; // Mix volume
	u8 InitialSpeed; // Initial speed
	u8 InitialTempo; // Initial tempo
	u8 PanningSeperation; // Panning separation between channels
	u8 PitchWheelDepth; // Pitch wheel depth for MIDI controllers
	u16 MessageLength; // Length of message if Bit 0 of Special is 1
	u32 MessageOffset; // Offset of message if Bit 0 of Special is 1
	u32 Reserved; // Reserved stuff
	u8 ChannelPan[64]; // Channel pan
	u8 ChannelVolume[64]; // Channel volume
} ITFileHeader;

typedef struct
{
	char magic[4]; // IMPS
	char fileName[13]; // 8.3 DOS filename (including null termating)
	u8 GlobalVolume; // Global volume for sample
	u8 Flags;
	u8 Volume; // Default volume
	char SampleName[26];
	u8 Convert;
	u8 DefaultPan;
	u32 NumberOfSamples; // not bytes! in samples!
	u32 LoopBeginning; // not bytes! in samples!
	u32 LoopEnd; // not bytes! in samples!
	u32 C5Speed; // Num of bytes a second for C-5
	u32 SustainLoopBeginning;
	u32 SustainLoopEnd;
	u32 SampleOffset; // the offset of the sample in file
	u8 VibratoSpeed;
	u8 VibratoDepth;
	u8 VibratoRate;
	u8 VibratoType;
} ITFileSample;

typedef struct
{
	u16 Length;
	u16 Rows;
	u8 Padding[4];
} ITFilePattern;

typedef struct
{
	u8 PC[2]; // Don't change this to u16, because of the way structs work, that will misalign the struct with the file!
	u8 A;
	u8 X;
	u8 Y;
	u8 PSW;
	u8 SP;
	u8 Reserved[2];
} SPCFileRegisters;

typedef struct
{
	char SongTitle[32];
	char GameTitle[32];
	char DumperName[16];
	char Comment[32];
	char Date[11];
	char SongLength[3];
	char FadeLength[5];
	char Artist[32];
	u8 ChannelDisabled;
	u8 EmulatorDumpedWith; //0 unknown, 1 ZSNES, 2 Snes9x
	u8 Reserved[45]; 
} SPCFileInformation;

typedef struct
{
	char FileTag[33]; // SNES-SPC700 Sound File Data v0.30
	u8 FileTagTerminator[2]; // 0x1A, 0x1A
	u8 ContainsID666; // 0x1A for contains ID666, 0x1B for no ID666
	u8 Version; //Version minor (30)
	SPCFileRegisters Registers; // 9bytes
	SPCFileInformation Information; // 163bytes
	u8 RAM[65536];
	u8 DSPBuffer[128];
} SPCFile;

typedef struct
{
	u8 Channel;
	u8 Mask;
	u8 Note;
	u8 Sample;
	u8 Volume;
	u8 Command;
	u8 CommandValue;
} ITPatternInfo;

typedef struct
{
	s32 mask, pitch, lvol, rvol;
	u8 note;
} itdata;

typedef struct
{
	s32 length;
	s32 loopto;
	s16 *buf;
	s32 freq;
} sndsamp;

typedef struct
{
	s32 ave;
	u32 envx, envcyc;
	s32 envstate;
	u32 ar, dr, sl, sr, gn;
} sndvoice;

#endif
