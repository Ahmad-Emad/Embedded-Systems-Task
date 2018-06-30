#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

void SystemInit(){}

int result=0;

void PORTF_Init()
{
	SYSCTL_RCGCGPIO_R |= 0x20;					// activate clock for port F
	while((SYSCTL_PRGPIO_R &0x20)==0){};  				// wait for PORTF ready
	GPIO_PORTF_LOCK_R =0x4C4F434B;   				// unlock port F 
	GPIO_PORTF_CR_R|= 0x03;						// allow changes for PF0,PF1 								
	GPIO_PORTF_AFSEL_R&=~0x03;					// disable alt funct for PF0,PF1
	GPIO_PORTF_PCTL_R &=~0x03;					// GPIO
	GPIO_PORTF_AMSEL_R &=~0x03;					// disable analog funct for PF0,PF1
	GPIO_PORTF_DIR_R |=0x03;					// PF0:IN, PF1:OUT
	GPIO_PORTF_DEN_R |=0x03;					// enable digital IO PF0,PF1
	GPIO_PORTF_PUR_R |=0x01;					// enable pull up res for PF0
	GPIO_PORTF_IS_R &=~0x01;					// PF0 edge senstive
	GPIO_PORTF_IBE_R |=0x01;					// PF0 both edges
	GPIO_PORTF_ICR_R = 0x01;					// clear flag 
	GPIO_PORTF_IM_R |= 0x01;					// arm interrupt PF0
	NVIC_EN0_R = 0x40000000;      					// enable interrupt 30 for PORTF
	__enable_irq();							// enable interrupts
		
}	
	
void TIMER0_Init(){
	
  SYSCTL_RCGCTIMER_R |= 0x01;   					// activate TIMER0
  TIMER0_CTL_R = 0;					    		// disable TIMER0A during setup
  TIMER0_CFG_R = 0;    							// 32-bit mode
  TIMER0_TAMR_R = 0x02;   						// periodic mode
  TIMER0_TAILR_R = 48000000-1;    					// reload value, set to 3 secs
  TIMER0_TAPR_R = 0;            					// bus clock resolution
  TIMER0_ICR_R = 0x00000001;    					// clear TIMER0 timeout flag
  TIMER0_IMR_R = 0x00000001;    					// arm timeout interrupt
  NVIC_EN0_R = 1<<19;           					// enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    					// enable TIMER0A
  __enable_irq();
}



void ADC0_Init()
{
	SYSCTL_RCGCADC_R |=0x01;					// avtivate ADC0
	SYSCTL_RCGCGPIO_R |= 0x10;					// activate PORTE
	while((SYSCTL_PRGPIO_R &0x10)==0){};				// wait for PORTE ready
	GPIO_PORTE_DIR_R &= ~0x04;      				// PE3 input
 	GPIO_PORTE_AFSEL_R |= 0x04;     				// enable alt funct on PE2
  	GPIO_PORTE_DEN_R &= ~0x04;       				// disable digital I/O on PE2
  	GPIO_PORTE_AMSEL_R |= 0x04;			      		// enable analog function on PE2
	ADC0_ACTSS_R &= ~0x0008;       		 			// disable sample sequencer 3
	ADC0_SSPRI_R = 0x0123;          				// Seq 3 is highest priority
	ADC0_EMUX_R &= ~0xF000;         				// seq3 is software trigger
	ADC0_SSMUX3_R = 0;					 	// channel Ain1 (PE2)
	ADC0_SSCTL3_R = 0x0006;         				// no TS0 D0, yes IE0 END0
  	ADC0_ACTSS_R |= 0x0008;	        				// enable sample sequencer 3
}

void UART0_Init()
{
	SYSCTL_RCGCUART_R|= 0x01;					// activate UART0
	SYSCTL_RCGCGPIO_R |= 0x01;					// activate PORTA
	while((SYSCTL_PRGPIO_R &0x01)==0){};  				// wait for PORTA ready
	GPIO_PORTA_DEN_R=0x03;						// PA0,PA1 digital
	GPIO_PORTA_AFSEL_R=0x03;					// enable alt funct
	GPIO_PORTA_PCTL_R=0x11;						// PA0,PA1 UART
	UART0_CTL_R=0;							// disable UART
	UART0_IBRD_R=104;						// 9600 baudrate
	UART0_FBRD_R=11;
	UART0_LCRH_R= 0x70;						// 8bit word, no parity, one stop bit , enable FIFO
	UART0_CTL_R=0x301;						// enable UART0,Tx,Rx
	
}


void UART_OutChar(char data){
  while((UART0_FR_R&0x20) != 0);					// wait until TxFF 
  UART0_DR_R = data;
	
}

void UART_OutString(char ch [],int s)
{	
	int i;
	for(i=0;i<s;i++)
		{
			UART_OutChar(ch[i]);
		}
}


void TIMER0A_Handler(void)
{
	int x=result;
	char buf [20];
	TIMER0_ICR_R = 0x01;						// acknowledge TIMER0
	sprintf(buf,"Temp= %d",x);
	UART_OutString(buf,sizeof(buf));
	UART_OutChar('\n');
}

void GPIOF_Handler()
{
	GPIO_PORTF_ICR_R=0x01;						// clear flag
	GPIO_PORTF_DATA_R^=0x02;					// led on 
	if((GPIO_PORTF_DATA_R &0x11)==0x10)				// switch pressed
		{
			UART_OutString("PRESSED",7);
			UART_OutChar('\n');
			UART_OutString("LED ON",6);
			UART_OutChar('\n');
		}
	else 
		{
			UART_OutString("LED OFF",7);
			UART_OutChar('\n');
		}

}

int main()
{
	PORTF_Init();
	TIMER0_Init();
	ADC0_Init();
	UART0_Init();
	while(1)
	{ 
		ADC0_PSSI_R=0x08;					// start conversion
		while((ADC0_RIS_R&0x08)==0){};				// wait conversion complete
		result=ADC0_SSFIFO3_R&0xFFF;				// read result
		result=( (result*330) / 4096);				// 3.3V , 12bit resolution
		ADC0_ISC_R=0x08;					// clear flag

	}
}



