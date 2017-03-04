#ifndef WAVE_H
#define WAVE_H
/*
"RIFF" file description header 	4 bytes - FOURCC 	The ascii text string "RIFF". 
size of file 	                4 bytes - DWORD 	The file size LESS the size of the "RIFF" description (4 bytes) 
                                                    and the size of file description (4 bytes). This is usually 
                                                    file size - 8.
"WAVE" description header 	    4 bytes - FOURCC 	The ascii text string "WAVE". 
"fmt " description header 	    4 bytes - FOURCC 	The ascii text string "fmt " (note the trailing space). 
size of WAVE section chunck 	4 bytes - DWORD 	The size of the WAVE type format 
                                                    (2 bytes) + mono/stereo flag (2 bytes) + 
                                                    sample rate (4 bytes) + bytes/sec (4 bytes) + 
                                                    block alignment (2 bytes) + bits/sample (2 bytes). 
                                                    This is usually 16 (or 0x10).
WAVE type format 	            2 bytes - WORD 	    Type of WAVE format. This is a PCM header, or a value of 0x01.
mono/stereo 	                2 bytes - WORD 	    mono (0x01) or stereo (0x02)
sample rate 	                4 bytes - DWORD 	Sample rate.
bytes/sec 	                    4 bytes - DWORD 	Bytes/Second
Block alignment 	            2 bytes - WORD 	    Block alignment
Bits/sample 	                2 bytes - WORD 	    Bits/Sample
"data" description header 	    4 bytes - FOURCC 	The ascii text string "data". 
size of data chunk 	            4 bytes - DWORD 	Number of bytes of data is included in the data section.
Data 	                        Unspecified data buffer 	Your data.

44 byte header
*/

extern unsigned char OpenWave(char* fileName);
extern unsigned char readSample();
extern void readSamples(unsigned char* buffer, int buffersize);

//access data via global variable to save on ram usage
extern volatile unsigned int gSampleRate;
extern volatile unsigned long gSizeOfSample;

#endif //WAVE_H
