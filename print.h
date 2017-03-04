#ifndef PRINT_H_
#define PRINT_H_

#ifndef _NO_PRINTF
int prints(char *string, unsigned char width, unsigned char pad);
int printi(long int i, unsigned char b, unsigned char sg, unsigned char width, 
			unsigned char pad, unsigned char letbase);
int Printf(char *format, ...);			
#else

#define Printf(x, ...)

#endif

#endif /*PRINT_H_*/
