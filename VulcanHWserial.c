/**
 * main.c - Launchpad compatible full-duplex software UART example program.
 *
 * SoftSerial:
 * Author: Rick Kimball
 * email: rick@kimballsoftware.com
 * Version: 1.00 Initial version 04-20-2011
 * Version: 1.01 cleanup 04-21-2011
 *
 */

#include <msp430.h>
#include <stdarg.h>
#include "config.h"
#include "Flash.h"

#include "HWserial.h"
#include "Wave.h"
#include "print.h"
#include "drivers/spi.h"
#include "pff2a/src/diskio.h"
#include "pff2a/src/pff.h"

/*
 * Pin layout on sd card notch on the left (metal pins facing away)
 *
 *   P8 P7 P6 P5 P4 P3 P2 P1
 *    |  |  |  |  |  |  |  |
 *   /-  -  -  -  -  -  -  - \
 *  |                        |
 *  |                        \
 *  |                         \
 *  |                          \
 *  |                           |
 *  -----------------------------
 * SPI Mode
 *	Pin   Name     Description
 *	1     NC       not connected
 *	2     CS       Chip Select/Slave Select (SS)
 *	3     DI       Master Out/Slave In (MOSI)
 *	4     VDD      Supply voltage
 *	5     CLK      Clock (SCK)
 *	6     VSS      Supply voltage ground
 *	7     DO       Master In/Slave Out (MISO)
 *  8     RSV      Reserved
 *
 * Pin layout for SPI
 *
 * P2.0-> CS   (SS) (P2)
 * P1.7-> MOSI (DI) (P3)
 *        VDD(+)    (P4)
 * P1.5-> CLK       (P5)
 *        VSS(-)    (P6)
 * P1.6-> MISO (DO) (P7)
 *
 * SD card pin layout (trans flash and Kingston MMC cards)
 *
 *   NC          CS(P2.0)    DI(P1.7)    VDD(+)
 *   1           2 (pin 8)   3 (pin 15)  4 (+)
 *  -|-----------|-----------|-----------|--
 *                                          |
 *          microSD proto board             |
 *                                          |
 *  -|-----------|-----------|-----------|--
 *   8           7 (pin 14)  6 (-)       5 (pin 7)
 *  RSV         DO(P1.6)     VCC(-)      CLK(P1.5)
 *
 * Pin layout for Hardware (115200K 8N1) serial
 *
 * P1.1 = RXD Data (pin 3)
 * P1.2 = TXD Data (pin 4)
 *
 *
 * P1.4 = Button (BIT4) (pin 6)
 *
 *                           SWITCH
 *                           /
 *                          /
 *          33K         |---O   O-----|
 * Vcc <---/\/\/\---|---|             |--> GND(-)
 *                  |   |----|(-------|
 *                  |       470nF
 *                P1.4
 *
 * P2.2 = PWM output (sound) (pin 10)
 */

//volatile unsigned char Line[8];
//volatile unsigned char gNextSample;

//comment this out if want to here the voice samples sequencil in order 
#define SPOCK_IN_ORDER

#define SAMPLE_BUFFER_SIZE 128

volatile unsigned char gSampleBuffer[SAMPLE_BUFFER_SIZE];
volatile unsigned char gAdvanceToNextSample;
volatile unsigned char *g_pSampleBuffer;
volatile unsigned char g_triggerSample;
volatile unsigned char gTimerCount;
//volatile unsigned char gButtonState;

volatile unsigned char uAudioSample1;
volatile unsigned char uAudioSample2;

#define OVER_SAMPLE_SIZE 4
#define WAV_SAMPLE_RATE 11025  //our waves sample rate

volatile char gOverSampleCount;

#define BUTTON BIT4 //Port 1
#define LEDPIN BIT3 //Port 2

//Define what lower we are using
#define LOW_POWER_BIT (LPM4_bits)

static void put_rc (FRESULT rc)
{
	static const char *str[] ={"OK", "DISK_ERR", "NOT_READY", "NO_FILE", "NO_PATH",
								"NOT_OPENED", "NOT_ENABLED", "NO_FILE_SYSTEM"};
								
	if( rc<sizeof(str) )
		Printf("rc=%u FR_%s\n\r", (long int)rc, str[rc]);
	else
		Printf("rc=%u\n\r");

}

#if 1
static void put_dump
(
	const void* buff,		/* Pointer to the byte array to be dumped */
	unsigned long addr,		/* Heading address value */
	int len					/* Number of bytes to be dumped */
)
{
	int i;
	const unsigned char *p = buff;

#if 0
	Printf("%8lX ", addr);		/* address */


	for (i = 0; i < len; i++)		/* data (hexdecimal) */
		Printf(" %2X", p[i]);
#endif
	//SoftSerial_xmit(' ');
	HWSerial_xmit(' ');
	for (i = 0; i < len; i++)		/* data (ascii) */
		//SoftSerial_xmit((p[i] >= ' ' && p[i] <= '~') ? p[i] : '.');
		HWSerial_xmit((p[i] >= ' ' && p[i] <= '~') ? p[i] : '.');

	//SoftSerial_xmit('\n');
	HWSerial_xmit('\n');
}
#endif

/**
 * Setup the button and interupt
 */
void SetupButtonPin(void)
{
      //gButtonState = 0;
    
//    P1SEL &= (~BUTTON); // Set P1.4 SEL as GPIO
//    P1DIR &= (~BUTTON); // Set P1.4 SEL as Input
//    P1IES |= (BUTTON); // Falling Edge 1 -> 0
//    P1REN |= (BUTTON); //Enable resistors
//    P1OUT |= (BUTTON); //Enable pull-up resistor
//    P1IFG &= (~BUTTON); // Clear interrupt flag for P1.4
    
//    P1IE  |= (BUTTON); // Enable interrupt
    
  P1SEL &= (~BUTTON); // Set P1.4 SEL as GPIO
  P1DIR &=~BUTTON;  //P1.4 as input
  P1REN |= BUTTON;                            // P1.4 resistors enabled
  P1OUT |=  BUTTON;                            // P1.4 enable pull resistor
  P1IES |= BUTTON;                            // P1.4 Hi --> lo edge
  P1IFG &= ~BUTTON;                           // P1.4 IFG cleared
  P1IE |= BUTTON;                             // P1.4 interrupt enabled
    
}

#if 0
void ToggleLEDon(void)
{
    P2OUT |= LEDPIN; //set gpio hig
}

void ToggleLEDoff(void)
{
    P2OUT &= (~LEDPIN); //set gpio low
}
#else

#define ToggleLEDon()  P2OUT |= LEDPIN
#define ToggleLEDoff() P2OUT &= (~LEDPIN)

#endif

void SetupLED(void)
{
	//Init led Port2 bit 3
    P2SEL &= (~LEDPIN); //set as gpio
	P2DIR |= LEDPIN;    //set gpio as output
	P2OUT &= (~LEDPIN); //set gpio low
}


/**
* Init PWM - init the Pulse Width Modulation thing using the second time timer
*/

void InitPWM(unsigned int sampleRateMSP)
{
	gOverSampleCount=0;
	 P2DIR |= BIT2;             // P2.2 to output
	 P2SEL |= BIT2;             // P2.2 to TA1.1
	 P2SEL2 &= ~BIT2;           // P2.2 to TA1.1

	 TA1CCTL0 = CCIE;				// CCR0 interrupt enabled

	//over sample by OVER_SAMPLE_SIZE
	 TA1CCR0 = (int)(sampleRateMSP/OVER_SAMPLE_SIZE);     // PWM Period (16Mhz/11025)/4
	 TA1CCTL1 = OUTMOD_7;//+CCIE;     	// TA1CCR1 reset/set
	 TA1CCR1 = 127;  					// TA1CCR1 PWM duty cycle
	 TA1CTL = TASSEL_2 + MC_1;   	// SMCLK, up mode
}

/**
 * setup() - initialize timers and clocks
 */

void setup()
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

    DCOCTL = 0x00;                  // Set DCOCLK to 16MHz
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

    SetupButtonPin();
    SetupLED();

    //SoftSerial_init();              // Configure TIMERA
    HWInitSerial();

	//Init PMW with a period of 11025
	InitPWM(16000000/WAV_SAMPLE_RATE);

    
    /**
     * Setting these flags allows you to easily measure the actual SMCLK and ACLK
     * frequencies using a multimeter or oscilloscope.  You should verify that SMCLK
     * and your desired F_CPU are the same.
     */

    // |= BIT0;
    //P1SEL |= BIT0;   // measure P1.0 for actual ACLK

    // |= BIT4;
    //P1SEL |= BIT4;   // measure P1.4 for actual SMCLK

    _enable_interrupts(); // let the timers do their work
}

static void put_drc(long int res)
{
	Printf("rc=%d\n\r", res);
}

//Simple Linear feedback shift register for creating pseudo random numbers
//(Code from wiki place)
uint16_t g_lfsr; //Must be set to non-zero value
void linearShift(void)
{
    uint16_t bit;        /* Must be 16bit to allow bit<<15 later in the code */

    /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit  = ((g_lfsr >> 0) ^ (g_lfsr >> 2) ^ (g_lfsr >> 3) ^ (g_lfsr >> 5) ) & 1;
    g_lfsr =  (g_lfsr >> 1) | (bit << 15);
    //Printf("g_lfsr:%d\n",(long int)g_lfsr);
}

void simplePrint(char *message)
{
    while(*message)
    {
        HWSerial_xmit((unsigned char)*message++);
    }
    HWSerial_xmit('\n');
    HWSerial_xmit('\r');
}
/**
 * main - sample program main loop
 *
 */
void main (void)
{
	FRESULT res;		/* result from file system */
	FATFS fs;			/* File system object */
	DIR dir;			/* Directory object */
	FILINFO fno;		/* File information */
	
	uint16_t i;
	uint16_t fileToOpen=0x0;
	uint16_t numfiles=0;
    uint16_t currentFile=0;

	setup();

	g_pSampleBuffer=0;
	gAdvanceToNextSample=0;
	g_triggerSample =0 ;

	//spi_initialize();
	spi_initialize2();

	//wait for 75 milli seconds @16Mhz
	__delay_cycles(1200000);
	Printf("\n\rSpock says\n\r");
    //Printf("File to open %d\n\r",(long int)fileToOpen);

	//init the disk
	while((res=disk_initialize())!=FR_OK)
	{
		__delay_cycles(600000);
		Printf("\n\rDisk_init:RC=%d\n\r",(long int)res);
	}
	//put_drc(res);

	//mount the file system
	while((res=pf_mount(&fs))!=FR_OK)
	{
		__delay_cycles(600000);
		Printf("\n\rMount_FS:RC=%d\n\r",(long int)res);
	}
	//put_rc(res);
	
	//_enable_interrupts(); // let the timers do their work

	//scan the top level directory
	res = pf_opendir(&dir, " ");
	if (res) 
	{ 
		put_rc(res); 
		while(1);
	}

	//count how many files are on the card
	while(1)
	{
		res = pf_readdir(&dir, &fno);
		if (res != FR_OK)
		{
			put_rc(res);
			break;
		}
		if (!fno.fname[0])
			break;

		if(!(fno.fattrib & AM_DIR) )
		{
			numfiles++;
		}
	}
	Printf("numfiles=%d\n\r", (long int)numfiles);
    
    while(1)
    {
        //read linear feedback number and compute a random number
    	//
        g_lfsr = *((uint16_t*)(SegmentDPtr8));
#ifndef SPOCK_IN_ORDER
        fileToOpen = g_lfsr%numfiles; //file to open
#else
        ++fileToOpen;
        if( fileToOpen>numfiles )
            fileToOpen=0;
#endif
        //generate next linear feedback shift number
        linearShift();
            
        res = pf_opendir(&dir, " ");

        //now find that file
        currentFile=0;
        for(i=0; i<numfiles; i++)
        {
            res = pf_readdir(&dir, &fno);
            if (res != FR_OK)
            {
                put_rc(res);
                while(1);
            }
            //don't count directories in the file search
            if( !(fno.fattrib & AM_DIR) )
            {
                if (currentFile==fileToOpen ) //&& !fno.fname[0])
                    break;
                currentFile++;
            }
        }

		if( OpenWave(fno.fname) )
		{
            //Display an error if the waves' sample rate is not 11025
			if( gSampleRate!=WAV_SAMPLE_RATE )
			{
				Printf("File %s is NOT 11025hz! %u given\n\r",fno.fname, (long int)gSampleRate);
				while(1);
			}
			Printf("playing wave: %s (%d)\n\r", fno.fname, (long int)fileToOpen);

			//fill the first half of sample buffer
			char bufferPage;

			gOverSampleCount=0;
			gAdvanceToNextSample=0;
			g_pSampleBuffer = 0;

			TA1CCR1 = 127;  					// TA1CCR1 PWM duty cycle

			//copy the sample in page 0 of the buffer
			readSamples((unsigned char*)&gSampleBuffer[0], SAMPLE_BUFFER_SIZE>>1);

			//decrement the sample size
			gSizeOfSample -= SAMPLE_BUFFER_SIZE>>1;

			//kick off the playing of the sample
			//g_pSampleBuffer=&gSampleBuffer[0];

			uAudioSample1 = gSampleBuffer[0];
			uAudioSample2 = gSampleBuffer[1];

			bufferPage=1;
			while(gSizeOfSample>(SAMPLE_BUFFER_SIZE>>1))
			{
                //Printf("sample size %d\n\r",(long int)gSizeOfSample);
				//decrement the sample size
				gSizeOfSample -= SAMPLE_BUFFER_SIZE>>1;

				if(bufferPage==0)
				{
					//start playing the sample in page 1 of buffer
					g_pSampleBuffer=&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];
					g_triggerSample = 1;

					//load in the sample into page 0
					readSamples((unsigned char*)&gSampleBuffer[0], SAMPLE_BUFFER_SIZE>>1);

					//wait for interrupt to finish playing page 1
                    //Printf("waiting for page 1\n\r");
					//ToggleLEDon();
					while(g_pSampleBuffer!=0);

					//enter low power mode 0 (LPM0), timer ISR will continue for the next sample
					//__bis_SR_register(LPM0_bits + GIE);

					//next page
					bufferPage=1;

					uAudioSample1 = gSampleBuffer[SAMPLE_BUFFER_SIZE-1];
					uAudioSample2 = gSampleBuffer[0];

					//g_pSampleBuffer=&gSampleBuffer[0];
				}
				else
				{
					//start playing of the sample in page 0 of buffer
					g_pSampleBuffer=&gSampleBuffer[0];
					g_triggerSample = 1;

					//read the sample into page 1 of the buffer
					readSamples((unsigned char*)&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1], SAMPLE_BUFFER_SIZE>>1);

					//wait for interrupt to finish playing page 0 of buffer
                    //Printf("waiting for page 0\n\r");
					//ToggleLEDoff();
					while(g_pSampleBuffer!=0);

					//enter low power mode 0 (LPM0), timer ISR will continue for the next sample
					//__bis_SR_register(LPM0_bits + GIE);

					//next page
					bufferPage=0;

					uAudioSample1 = gSampleBuffer[(SAMPLE_BUFFER_SIZE>>1)-1];;
					uAudioSample2 = gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];

					//g_pSampleBuffer=&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];
				}
			} //while

			//now play the last bits of the sample(if any)
			if( gSizeOfSample>0 )
			{
				if(bufferPage==0)
				{
					//start playing the sample in page 1 of buffer
					g_pSampleBuffer=&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];
					g_triggerSample = 1;

					//fill the buffer with the remaining samples
					readSamples((unsigned char*)&gSampleBuffer[0], gSizeOfSample);

					//pad the rest of the buffer with zeros
					for(i=gSizeOfSample; i<SAMPLE_BUFFER_SIZE>>1; i++)
					{
						gSampleBuffer[i]=0x80;
					}

					//wait for interrupt to finish playing current buffer
					while(g_pSampleBuffer!=0);

					//enter low power mode 0 (LPM0), timer ISR will continue for the next sample
					//__bis_SR_register(LPM0_bits + GIE);

					uAudioSample1 = gSampleBuffer[SAMPLE_BUFFER_SIZE-1];
					uAudioSample2 = gSampleBuffer[0];

					//play the last bits of the sample
					g_pSampleBuffer=&gSampleBuffer[0];
					g_triggerSample = 1;
				}
				else
				{
					//start playing of the sample in page 0 of buffer
					g_pSampleBuffer=&gSampleBuffer[0];
					g_triggerSample = 1;

					readSamples((unsigned char*)&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1], gSizeOfSample);

					//pad the rest of the buffer with zeros
					for(i=gSizeOfSample; i<SAMPLE_BUFFER_SIZE>>1; i++)
					{
						//upper half
						gSampleBuffer[i+(SAMPLE_BUFFER_SIZE>>1)]=0x80;
					}

					//wait for interrupt to finish playing current buffer
					while(g_pSampleBuffer!=0);

					//enter low power mode 0 (LPM0), timer ISR will continue for the next sample
					//__bis_SR_register(LPM0_bits + GIE);

					uAudioSample1 = gSampleBuffer[(SAMPLE_BUFFER_SIZE>>1)-1];;
					uAudioSample2 = gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];

					//kick off the last bits of the sample
					g_pSampleBuffer=&gSampleBuffer[SAMPLE_BUFFER_SIZE>>1];
					g_triggerSample = 1;
				}
			}

			//wait for interrupt to finish playing current buffer
			while(g_pSampleBuffer!=0);

			TA1CCR1 = 127;  					// TA1CCR1 PWM duty cycle

			//delay a bit and flash the red LED
			ToggleLEDoff(); //P2OUT &= ~BIT3;

		} //if( OpenWave(fno.fname) )

        //save the linear shift number onto flash memory
        write_Seg(SegmentDPtr8, (uint8_t *)&g_lfsr, sizeof(g_lfsr) );

        Printf("Waiting...\n\r");

        //go into low power mode and wait for button interrupt
        __bis_SR_register(LOW_POWER_BIT + GIE);

    } //while
}

// Port 1 interrupt service routine
#ifdef __TI_COMPILER_VERSION__
#pragma vector=PORT1_VECTOR
__interrupt
#else //gcc syntax
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void Port_1(void)
{

    P1IFG &= ~BUTTON; // P1.4 IFG cleared
    //P1IES ^= BUTTON; // toggle the interrupt edge,

    ToggleLEDon();

    //exit low power mode and play next sample
    __bic_SR_register_on_exit(LOW_POWER_BIT);

}

/**
* TimerA0 interrupt service routine
**/
unsigned char gVoiceLEDonTime;
unsigned char gVoiceLEDoffTime;
unsigned char gVoiceLEDstart;

#ifdef __TI_COMPILER_VERSION__
#pragma vector=TIMER1_A0_VECTOR
__interrupt 
#else //gcc syntax
__attribute__((interrupt(TIMER1_A0_VECTOR)))
#endif
void TIMER1_A0_ISR(void)
{
	unsigned short sampleAvg;
	unsigned char s1;
	unsigned char s2;

	gTimerCount++;
	if( g_triggerSample==0 )
		return;

	//if(g_pSampleBuffer==0)
	//	return;

	s1=uAudioSample1;
	s2=uAudioSample2;
	switch(gOverSampleCount)
	{
		case 0: sampleAvg = s1;
				break;
							//S1*3/4 + S2*1/4
		case 1: sampleAvg = (s1>>2) + (s1>>1) + (s2>>2);  //( (s1 + (s1<<1) ) >> 2 ) + (s2>>2) ;
				break;
							 //S1*1/2 + S2*1/2
		case 2: sampleAvg =  (s1>>1) + (s2>>1); //(s1 + s2 )>>1;
				break;
							//S1*1/4 + S2*3/4
		case 3: sampleAvg = (s1>>2) + (s2>>2) + (s2>>1);  //(s1>>2) +  ( (s2 + (s2<<1) )>>2 );
				break;
	}

	if( ((short)sampleAvg) +128>= 1451/4 ) //cap
		TA1CCR1 = (1451/4)-1;
	else
		TA1CCR1 = ((short)sampleAvg) + 128;

	gOverSampleCount++;
	if(gOverSampleCount==OVER_SAMPLE_SIZE)
	{
		//TA1CCR1 = g_pSampleBuffer[gAdvanceToNextSample++];
		//enter low power mode 0 (LPM0), timer ISR will force exit when gCount==4
		//__bis_SR_register(LPM0_bits + GIE);

		uAudioSample1 = uAudioSample2;
		uAudioSample2 = g_pSampleBuffer[++gAdvanceToNextSample];

		gOverSampleCount=0;
		if( gAdvanceToNextSample==(SAMPLE_BUFFER_SIZE>>1) )
		{
			g_triggerSample=0;
			g_pSampleBuffer=0;
			gAdvanceToNextSample=0;
			//__bic_SR_register_on_exit(LPM0_bits);     // Clear LPM0 bits from 0(SR)
		}
		if(gVoiceLEDstart==0)
		{
			char sample = (char)g_pSampleBuffer[gAdvanceToNextSample];
			//if(sample>(char)-32 && sample<(char)32)
			{
				gVoiceLEDoffTime=255;
				gVoiceLEDonTime =(sample&127)>>1;
				gVoiceLEDstart = 1;
				ToggleLEDoff(); //P2OUT &= ~BIT3;
			}
			//gVoiceLEDonTime = (g_pSampleBuffer[gAdvanceToNextSample]);
			//gVoiceLEDoffTime = (g_pSampleBuffer[gAdvanceToNextSample])&0x7F;
		}

		if(gVoiceLEDonTime)
		{
			ToggleLEDon(); //P2OUT |= BIT3;
			gVoiceLEDonTime--;
		}
		else
		if(gVoiceLEDoffTime)
		{
			ToggleLEDoff(); //P2OUT &= ~BIT3;
			gVoiceLEDoffTime--;
		}

		if( gVoiceLEDoffTime==0 && gVoiceLEDonTime==0 )
			gVoiceLEDstart = 0;

	}
}



//#ifdef RANDOM_PLAY


//#endif //
/**
 * TimerA1 Interrupt Vector (TAIV) handler
 **/

/*
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{

	switch( TAIV )
	{
		case  2:				// CCR1 interrupt
			P1OUT &= ~BIT0;		// Clear P1.0 to determine duty cycle.
			//CCR1 = 62500>>1;
			//turn_led=1;
			break;
		default:
			break;
	}

}
*/
