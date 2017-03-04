#include <msp430.h>

#include <msp430g2553.h>
#include "HWserial.h"

#define RX_BUFFER_LEN 16
volatile unsigned char gRXBuffer[RX_BUFFER_LEN];
volatile unsigned char gBufferCount;

void HWInitSerial(void)
{
//	return;

	gBufferCount=0;
	  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
	  UCA0CTL1 |= UCSSEL_2;                     // SMCLK

	  //115200 baud, clocked from 16MHz SMCLK
	  UCA0BR0   = 0x8a;                         // 115200 baud if SMCLK@16MHz
	  UCA0BR1   = 0x00;
	  UCA0MCTL  = UCBRS_7;

	  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void HWSerial_xmit(unsigned char byte)
{
	//return;

  while (!(IFG2 & UCA0TXIFG));         // USCI_A0 TX buffer ready?
  UCA0TXBUF = byte;                    // TX -> RXed character
}

volatile unsigned char gSerialByte;
unsigned char HWSerial_read(void)
{
	//return 0;

	unsigned char byte=gRXBuffer[gSerialByte];
	gRXBuffer[gSerialByte]=0;
	gSerialByte=(gSerialByte+1)&(RX_BUFFER_LEN-1);
	return byte;
}

// Echo back RXed character, confirm TX buffer is ready first
#ifdef __TI_COMPILER_VERSION__
#pragma vector=USCIAB0RX_VECTOR
__interrupt 
#else //gcc syntax
__attribute__((interrupt(USCIAB0RX_VECTOR)))
#endif
void USCI0RX_ISR(void)
{
	gRXBuffer[gBufferCount]=UCA0RXBUF;
	gBufferCount=(gBufferCount+1)&(RX_BUFFER_LEN-1);

	while (!(IFG2 & UCA0TXIFG));              // USCI_A0 TX buffer ready?
	UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
  //copy re
}
