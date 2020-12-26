#include "TM4C123GH6PM.h"
#include "stdint.h"
#include "stdlib.h"

// Function Prototypes
void SPI_Init(void);
void send_byte(unsigned char data);
void Delay_ms(int time_ms);

void SPI_Init(void)
{
	// Pg. 965: SSI Initialization and Configuration
	
	// [MASTER SELECT]
	// Enable SSI and GPIO Clock for Port D
	SYSCTL->RCGCSSI |= 0x02;										// enable SSI Module 1 [sychronous serial interface]
	SYSCTL->RCGCGPIO |= 0x08;										// enable GPIO clock Port D for master select [general input/output register]
	
	// Initialize SSI Alternate Function (Master Select)
	GPIOD->AFSEL |= 0x09;											// enable PD0, PD1, PD2, PD3 [alternate function select]. Reference Pg. 1340, 1341
	GPIOD->PCTL &= ~0x000F00F;
	GPIOD->PCTL |= 0x2002;											// enable bit (1) of byte PMC0, PMC1, PMC2, PMC3 (corresponds with AFSEL bits) [port control].
														// Bit (2) is recognized when referencing Pg. 1351. Find SSI1 and look at the column number it is in (1).
	GPIOD->DEN |= 0x09;											// enable digital functionality for PD0, PD1, PD2, PD3 [digital enable]
														// SSI is a digital signal interface
	GPIOD->AMSEL &= ~(0x09);										// disable analog functionality since digital functionality is enabled
	
	// SSI Master/Slave Configuration
	SSI1->CR1 = 0;												// disable SSE bit [synchronous serial port enable] SSI operation for configuration
	SSI1->CC = 0;												// configure SSI clock source [clock configuration]
	SSI1->CPSR = 64;											// configure clock prescale divisor [clock prescale]
														// SSInClk = SysClk / (CPSDVSR * (1 + SCR))
														// The default system clock frequency of the Tiva C is 16 MHz.
	SSI1->CR0 = 0x7;											// SCR (serial clock rate): 0 -> Bit rate = SSInClk = 16 MHz / (64 * (1 + 0)) = 250000 Hz
														// SPH (serial clock phase): 0 -> data captured on first clock edge
														// SPO (serial clock polarity): 0 -> steady state low is placed on SSInClk pin
														// FRF (frame format select): 0x0 -> freescale SPI frame format
														// DSS (data size select): 0x7 -> 8-bit data
	SSI1->CR1 |= 0x2;											// enable SSE bit [synchronous serial port enable] SSI operation for configuration
	
	// [SLAVE SELECT]
	SYSCTL->RCGCGPIO |= 0x20;										// enable GPIO clock Port F for slave select [general input/output register]
	GPIOF->DEN |= 0x04;											// enable digital functionality of PF2
	GPIOF->DIR |= 0x04;											// enable PF2 as output
	GPIOF->DATA |= 0x04;											// set PF2 bit high
}

void Delay_ms(int time_ms)
{
	int i, j;
	for(i=0; i<time_ms; i++)
	{
		for(j=0; j<3180; j++) {}
	}
}

void send_byte(unsigned char data)
{
	GPIOF->DATA &= ~(0x04);											// set PF2 bit low
	while(((SSI1->SR)&(0x2))==0);										// stay in the continuous loop until transmit FIFO is full
	SSI1->DR = data;											// write "data" to the [data register]
	while(SSI1->SR & 0x2);											// stay in the continuous loop until transmit FIFO is not full
	GPIOF->DATA |= 0x04;											// set PF2 bit high
}

int main(void)
{
	unsigned char val1 = 'A';
	unsigned char val2 = 'B';

	SPI_Init();
	while(1)
	{
		send_byte(val1);
		Delay_ms(1000);
		send_byte(val2);
		Delay_ms(1000);
	}
}

// NOTES
// TIVA MCU			ARDUINO
// PD0 = SSI1Clk	->	D13 = SCLK
// PD1 = SSI1Fss
// PD2 = SSI1Rx
// PD3 = SSI1Tx		->	D11 = MOSI (master out slave in)
