#include "TM4C123.h" 
#include <stdlib.h>
#include <stdint.h>


#define  RCGCSSI    (*((volatile unsigned long*) (0x400FE000+0x61C)))
#define  RCGCGPIO   (*((volatile unsigned long*)   (0x400FE000+ 0x608)))//port a 2-5
#define  PORTA_BASE    (0x40004000)
#define  PORTA_AFSEL_R  (*((volatile unsigned long*)  (PORTA_BASE +0x420)))
#define  PORTA_PCTL_R   (*((volatile unsigned long*)  (PORTA_BASE +0x52C)))
#define  PORTA_DIR_R    (*((volatile unsigned long*)  (PORTA_BASE + 0x400)))
#define  PORTA_DEN_R    (*((volatile unsigned long*)  (PORTA_BASE + 0x51C)))
#define  PORTA_DATA_R   (*((volatile unsigned long*)  PORTA_BASE  ))
#define  PORTA_AMSEL_R   (*((volatile unsigned long*)  (PORTA_BASE + 0x528) ))

#define  PORTF_BASE      (0x40025000)
#define  PORTF_DIR_R    (*((volatile unsigned long*)  (PORTF_BASE + 0x400)))
#define  PORTF_DEN_R    (*((volatile unsigned long*)  (PORTF_BASE + 0x51C)))
#define  PORTF_DATA_R   (*((volatile unsigned long*)  (PORTF_BASE + 0x00) ))
#define  PORTF_AFSEL_R  (*((volatile unsigned long*)  (PORTF_BASE +0x420)))
#define  PORTF_PCTL_R   (*((volatile unsigned long*)  (PORTF_BASE +0x52C)))
#define  PORTF_AMSEL_R   (*((volatile unsigned long*)  (PORTF_BASE + 0x528) ))

#define SSI0_BASE  (0x40008000)
#define SSI_CR0_R  (*((volatile unsigned long*)  (SSI0_BASE + 0x00) ))
#define SSI_CR1_R  (*((volatile unsigned long*)  (SSI0_BASE + 0x004) ))
#define SSI_CC_R    (*((volatile unsigned long*)  (SSI0_BASE + 0xFC8) ))
#define SSI_CPSR_R  (*((volatile unsigned long*)  (SSI0_BASE + 0x010) ))
#define SSI_DR_R    (*((volatile unsigned long *) (SSI0_BASE + 0x008) ))
#define SSI_SR_R    (*((volatile unsigned long *) (SSI0_BASE + 0x00C) ))

 
char ssi_s_read(void);

int main()  
{
	RCGCSSI |= 0x1; // Enable and provide a clock to SPI0
 	RCGCGPIO |= 0x21; // Enable and provide a clock to GPIO PortA and PortF 
	
	PORTA_AFSEL_R |= 0x3C; // Enable alternate functions on PA2, PA3, PA4, PA5  
	PORTA_PCTL_R |= 0x222200; // Assign SPI signals to PA2, PA3, PA4, PA5   
	PORTA_DEN_R |= 0x3C|(1U<<7); // Enable digital functions for PA2, PA3, PA4, PA5    AND PA7
	PORTA_DIR_R &= ~(1U<<7); // Set PA7 as input 
	
	SSI_CR1_R = 0x4; // Disable SPI and configure it as a slave 
	SSI_CC_R= 0x0; // Select the SPI Baud Clock Source as system clock 
	SSI_CPSR_R = 0x10; // Set the clock frequency to 1 MHz 
	SSI_CR0_R = 0x7; // Freescale SPI mode, 1 Mbps bit rate, 8 data bits 
  SSI_CR1_R |= 0x2; // Enable SPI    
	
	PORTF_DIR_R |= 0xE; // Configure LED pins(PF1, PF2, PF3) to be an output
	PORTF_DEN_R |= 0xE; // Enable digital functions for LED pins 
	PORTF_DATA_R &= ~(0x0E); // Turn off the LEDs 
	
	while(1)
	{
			char data=ssi_s_read();
			if(data== 'R'){PORTF_DATA_R = 0x2;} // If 'R' is received, turn on the red LED  
			else if (data == 'G'){PORTF_DATA_R = 0x8;} // If 'G' is received, turn on the green LED 
			else if (data == 'B'){PORTF_DATA_R= 0x4;} // If 'B' is received, turn on the blue LED 		
	}
}

char ssi_s_read(void){
	while((SSI_SR_R & 0x4) == 0x0){} // Wait until receive FIFO is not empty 
  return (char)	SSI0->DR&0xFF;
}
