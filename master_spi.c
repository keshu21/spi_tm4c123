
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

void Ssi_Init(void){
   RCGCSSI|=0x01;
   RCGCGPIO |= 0x01;
   PORTA_AFSEL_R|=0x03C;//SET ANALOG FUNCTION FOR CLK AND TRNSMISSION LINE
   PORTA_DEN_R|=0x3C|1U<<7;
   PORTA_PCTL_R|=0x00222200;
   //CONFIGURE THE SALVE SELECT LINE PA7
   PORTA_DIR_R|=1U<<7;
	 PORTA_DATA_R|=1U<<7;
   //PORTA_DEN_R|=0x08;
   SSI_CR1_R&=0;//disable spi 
   SSI_CR1_R|=0x00;//CONFIGURE IT AS A MASTER 
   SSI_CC_R=0;//selecting internal clk
   SSI_CPSR_R=0x10;//set prescaler=8 make clock rate as 2Mhz

   SSI_CR0_R|=0x007;//DATA SIZE =8,4-5 FOR FRAME,6-CLOCK POLARITY ,7-CLOCK PHASE
  

   SSI_CR1_R|=2;//enable the spi 

}
void SPI_Write(char c);   
void delay(void); 
void Ssi_Init(void);

void SPI_Write(char c)
{
	PORTA_DATA_R&= ~(1U<<7); // Select the slave by making active-low slave select line low 
	while((SSI_SR_R & 0x2) == 0x0); // wait until transmit FIFO is not full   
	SSI_DR_R = c; // send the data 
	while((SSI_SR_R & 0x10) == 0x10); //  wait until busy bit becomes 0 
  	PORTA_DATA_R|= 1U<<7; // Deselect the slave   
}

void delay()
{
	int i; 
	for(i=0; i<1000000; i++);  
} 

int main()  
{
	char red = 'R';
	char green = 'G'; 
	char blue = 'B'; 
	Ssi_Init();
	
	while(1)
	{
		SPI_Write(red); 
		delay(); 
		SPI_Write(green); 
		delay(); 
		SPI_Write(blue);  
		delay(); 
	}
}




