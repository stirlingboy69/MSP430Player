#include "pff2a/src/pff.h"
#include "print.h"

#include "Wave.h"

volatile unsigned int gSampleRate;
volatile unsigned long gSizeOfSample;
char isChunkID(char* pData, unsigned char *chunkID);

char isChunkID(char* pData, unsigned char *chunkID)
{
	if( pData[0]!=chunkID[0] && pData[1]!=chunkID[1] && pData[2]!=chunkID[2] && pData[3]!=chunkID[3] ) //no match
	{
		return 0;
	}
	return 1;
}
unsigned char OpenWave(char* fileName)
{
	FRESULT res;
	DWORD u32data;
	WORD u16data;
	WORD bytesRead;
	unsigned char temp[4];
	unsigned char* pData=(void*)&temp;

	res=pf_open(fileName);
    if(res)
	{
		Printf("FRESULT=%d on file %s\n\r", (long int)res, fileName);
		return 0;
	}
	
	res=pf_read(pData, 4, &bytesRead); // 'RIFF' id
	if( !isChunkID("RIFF", pData) ) //not a riff file
	{
		return 0;
	}
	res=pf_read(&u32data, 4, &bytesRead); //File size
	res=pf_read(&u32data, 4, &bytesRead); // 'WAVE' id
	res=pf_read(&u32data, 4, &bytesRead); // 'fmt ' id
	res=pf_read(&u32data, 4, &bytesRead); // Size of wave type format (should be 16 bytes)

	if( u32data!=16 )
	{
		Printf("Not a PCM wave file\n\r");
		return 0;
	}
	res=pf_read(&u16data, 2, &bytesRead); // Wave type (1 for PCM)
	res=pf_read(&u16data, 2, &bytesRead); // Mono (1) /stereo (2)
	res=pf_read(&u32data, 4, &bytesRead); // Sample rate
	gSampleRate=(unsigned int)u32data; //save sample rate
	res=pf_read(&u32data, 4, &bytesRead); // Bytes per second
	res=pf_read(&u16data, 2, &bytesRead); // Block alignment
	res=pf_read(&u16data, 2, &bytesRead); // Bits per sample

	res=pf_read(pData, 4, &bytesRead); // 'data' data text id
	
	//find the data chunk
	while(!isChunkID("data", pData) )
	{
		unsigned int i;
		//read in chunk size
		res=pf_read(&u32data, 4, &bytesRead); //
		//skip data
		for(i=0; i<u32data; i++)
		{
			res=pf_read(&temp, 1, &bytesRead); //
		}
		//read in next chunkID
		res=pf_read(pData, 4, &bytesRead); // 'data' data text id

	}
	
	res=pf_read(&u32data, 4, &bytesRead); // size of data
	gSizeOfSample = u32data; //save size of sample data
	
	return 1;
}

//
unsigned char readSample()
{
	unsigned char sample=0;
	WORD bytesRead;
	pf_read_fast((void*)&sample, 1, &bytesRead); // size of data
	
	return sample;
}

void readSamples(unsigned char* buffer, int buffersize)
{
	WORD bytesRead;
	//FRESULT res;

	pf_read_fast((void*)buffer, buffersize, &bytesRead); // size of data
}
