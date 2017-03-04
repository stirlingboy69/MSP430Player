#include <msp430.h>
#include "spi.h"

/*
 * Pin layout for SPI
 *
 * P2.0-> CS
 * P1.5-> CLK
 * P1.6-> MISO
 * P1.7-> MOSI
 *
 */

/* Initialize and enable the SPI module */
void spi_initialize( void )
{
  P2DIR |= 0x01;      // CS Line
  UCB0CTL1 |= UCSWRST;
  UCB0CTL0 |= UCMSB |  UCMST | UCSYNC;
  UCB0CTL1 = UCSSEL_2;
  P1SEL  |= 0xE0; // P1.5, P1.6, P1.7 option select
  P1SEL2 |= 0xE0;
  UCB0CTL1 &= ~UCSWRST;

  P2OUT |= (0x01);       // CS high 

}
  
/* Initialize and enable the SPI module */
void spi_initialize2( void )
{
  P2DIR |= BIT0;      // CS Line
  UCB0CTL1 |= UCSWRST;
  //UCB0CTL0 |= UCMSB |  UCMST | UCMODE_0 | UCSYNC;
  UCB0CTL0 = UCCKPL + UCMSB + UCMST + UCMODE_0 + UCSYNC;
  UCB0CTL1 = UCSSEL_2 | UCSWRST;

  UCB0BR0 = 0x02; // 8MHz
  UCB0BR1 = 0x00;

  P1SEL  |= BIT5 | BIT6 | BIT7; // P1.5, P1.6, P1.7 option select
  P1SEL2 |= BIT5 | BIT6 | BIT7;

  UCB0CTL1 &= ~UCSWRST;

  P2OUT |= BIT0;       // CS high

}  
  
char spi_send( char payload )
{
  //while (!(IFG2 & UCA0TXIFG)); //TX buffer ready

  IFG2 &= ~UCB0RXIFG;  
  //P2OUT &= ~(0x01); 
  UCB0TXBUF = payload;             
  while (!(IFG2 & UCB0RXIFG));  // USCI_B0 TX buffer ready?
  return UCB0RXBUF;
  //P2OUT |= (0x01);  
}

char spi_receive( void )
{
  //while (!(IFG2 & UCA0TXIFG));

  IFG2 &= ~UCB0RXIFG;   
  //P2OUT &= ~(0x01);
  UCB0TXBUF = 0xFF; // Send dummy packet to get data back.
  while (!(IFG2 & UCB0RXIFG));      // USCI_B0 TX buffer ready?
  //P2OUT |= (0x01);  
  return UCB0RXBUF;
}
