#ifndef WAV_H
#define WAV_H

#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <AL/al.h>

#ifndef base_datatypes
#define base_datatypes

typedef unsigned char      u8 ;
typedef unsigned short int u16;
typedef unsigned int       u32;
//typedef unsigned __int64   u64;
typedef char               i8 ;
typedef short int          i16;
typedef int                i32;
//typedef __int64            i64;
typedef float              f32;
typedef double             f64;
typedef long double        f80;
typedef bool               bit;

#ifndef NULL
#define NULL 0
#endif

#define IN 
#define OUT 
#endif

#define WAV_DEF_FORMAT_PCM        1
#define WAV_DEF_FORMAT_EXTENSIBLE 0xFFFE

#ifndef HAVE__STRNICMP
#define _strnicmp strncasecmp
#endif

enum wavEType
{
	WF_EX  = 1,
	WF_EXT = 2
};

struct wavSGUID 
{
	u32 Data1;
	u16 Data2;
	u16 Data3;
	u8  Data4[ 8 ];
};

struct wavSFormatEx
{
	u16 FormatTag;         /* format type */
	u16 Channels;          /* number of channels (i.e. mono, stereo...) */
	u32 SamplesPerSec;     /* sample rate */
	u32 AvgBytesPerSec;    /* for buffer estimation */
	u16 BlockAlign;        /* block size of data */
	u16 BitsPerSample;     /* number of bits per sample of mono data */
	u16 Size;              /* the count in bytes of the size of */
	                       /* extra information (after cbSize) */
};

struct wavSFormat
{
	u16 FormatTag;         /* format type */
	u16 Channels;          /* number of channels (i.e. mono, stereo...) */
	u32 SamplesPerSec;     /* sample rate */
	u32 AvgBytesPerSec;    /* for buffer estimation */
	u16 BlockAlign;        /* block size of data */
	u16 BitsPerSample;     /* number of bits per sample of mono data */
	u16 Size;              /* the count in bytes of the size of */
	/* extra information (after cbSize) */
};

struct wavSFormatExtensible
{
	wavSFormatEx Format;
	union 
	{
		u16 ValidBitsPerSample;       /* bits of precision  */
		u16 SamplesPerBlock;          /* valid if wBitsPerSample==0 */
		u16 Reserved;                 /* If neither applies, set to zero. */
	} Samples;
	u32 dwChannelMask;      /* which channels are */
	wavSGUID SubFormat;
};

struct wavSPCMFormat
{
	wavSFormat  wf;
	u16         BitsPerSample;
};

struct wavSFileInfo
{
	wavEType Type;
	wavSFormatExtensible EXT;
	u8*   Data;
	u32	  DataSize;
	FILE* File;
	u32   DataOffset;
};

struct wavSFileHeader
{
	i8	RIFF[4];
	u32	RIFFSize;
	i8	WAVE[4];
};

struct wavSRIFFChunk
{
	i8	ChunkName[4];
	u32	ChunkSize;
};

struct wavSFMT
{
	u16	FormatTag;
	u16	Channels;
	u32	SamplesPerSec;
	u32	AvgBytesPerSec;
	u16	BlockAlign;
	u16	BitsPerSample;
	u16	Size;
	u16 Reserved;
	u32	ChannelMask;
	wavSGUID guidSubFormat;
};

bit wavParseFile(const char* filename, wavSFileInfo* fileinfo);

char* loadWAVData(const char* filename, int& outsize, int& freq, int &format);
#endif
