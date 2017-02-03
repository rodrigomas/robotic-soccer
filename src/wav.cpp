#include "wav.h"

bit wavParseFile(const char* filename, wavSFileInfo* fileinfo)
{
	wavSFileHeader	header;
	wavSRIFFChunk	riffChunk;
	wavSFMT			waveFmt;
	bit result = false;

	if (!filename || !fileinfo)
	{
		return result;
	}
	memset(fileinfo, 0, sizeof(wavSFileInfo));

	fileinfo->File = fopen(filename, "rb");
	if (fileinfo->File)
	{
		fread(&header, 1, sizeof(wavSFileHeader), fileinfo->File);
		if ( !_strnicmp(header.RIFF, "RIFF", 4) && 
			 !_strnicmp(header.WAVE, "WAVE", 4) )
		{
			while (fread(&riffChunk, 1, sizeof(wavSRIFFChunk), fileinfo->File) == sizeof(wavSRIFFChunk))
			{
				if (!_strnicmp(riffChunk.ChunkName, "fmt ", 4))
				{
					if (riffChunk.ChunkSize <= sizeof(wavSFMT))
					{
						fread(&waveFmt, 1, riffChunk.ChunkSize, fileinfo->File);

						if (waveFmt.FormatTag == WAV_DEF_FORMAT_PCM)
						{
							fileinfo->Type = WF_EX;
							memcpy(&fileinfo->EXT.Format, &waveFmt, sizeof(wavSPCMFormat));
						}
						else if (waveFmt.FormatTag == WAV_DEF_FORMAT_EXTENSIBLE)
						{
							fileinfo->Type = WF_EXT;
							memcpy(&fileinfo->EXT, &waveFmt, sizeof(wavSFormatExtensible));
						}
					}
					else
					{
						fseek(fileinfo->File, riffChunk.ChunkSize, SEEK_CUR);
					}
				}
				else if (!_strnicmp(riffChunk.ChunkName, "data", 4))
				{
					fileinfo->DataSize = riffChunk.ChunkSize;
					fileinfo->DataOffset = ftell(fileinfo->File);
					fseek(fileinfo->File, riffChunk.ChunkSize, SEEK_CUR);
				}
				else
				{
					fseek(fileinfo->File, riffChunk.ChunkSize, SEEK_CUR);
				}

				// Ensure that we are correctly aligned for next chunk
				if (riffChunk.ChunkSize & 1)
				{
					fseek(fileinfo->File, 1, SEEK_CUR);
				}
			}

			if ( fileinfo->DataSize && fileinfo->DataOffset && 
				 ((fileinfo->Type == WF_EX) || (fileinfo->Type == WF_EXT)))
			{
				result = true;
			}
			else
			{
				fclose(fileinfo->File);
			}
		}
	}

	return result;
}

char* loadWAVData(const char* filename, int& outsize, int& freq, int &format)
{
	wavSFileInfo fileinfo;
	char* result = NULL;
	if( wavParseFile(filename,&fileinfo) )
	{
		fileinfo.Data = new u8[fileinfo.DataSize];
		if (fileinfo.Data)
		{
			// Seek to start of audio data
			fseek(fileinfo.File, fileinfo.DataOffset, SEEK_SET);

			// Read Sample Data
			if (fread(fileinfo.Data, 1, fileinfo.DataSize, fileinfo.File) != fileinfo.DataSize)
			{
				delete[] fileinfo.Data;
				result = NULL;
			}
			else
			{
				freq    = fileinfo.EXT.Format.SamplesPerSec;
				outsize = fileinfo.DataSize;
				result  = (char*)fileinfo.Data;
			
				if( fileinfo.EXT.Format.Channels == 1 ) {
					if( fileinfo.EXT.Format.BlockAlign == 1 ) {
						format  = AL_FORMAT_MONO8;
					} else {
						format  = AL_FORMAT_MONO16;
					}
				} else {
					if( fileinfo.EXT.Format.BlockAlign == 1 ) {
						format  = AL_FORMAT_STEREO8;
					} else {
						format  = AL_FORMAT_STEREO16;
					}
				}
			}
		}
		fclose(fileinfo.File);
	}
	return result;
}
