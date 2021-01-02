// UART to PC Communication using Serial line through command line visualizer PuTTy
// When R/G/B is entered into the command line, that respective color is output on the Tiva C launchpad
// This is done by programming to U1 (UART1, Central MCU) and transfer serial data to U2 (UART2, USB MCU)
// This data travels across the UART Module 0 Line, if additional lines are used, one must purchase an adapter
// YouTube Channel Source: AllAboutEE, analyze schematics on pg. 20 & 22 to observe that U1 and U2 use UART0
// Tx = transmitter, Rx = receiver

#include "TM4C123GH6PM.h"

void UART_Init(void);
char readChar(void);
void printChar(char c);
void printString(char * string);

int main(void)
{
	UART_Init();
	while(1)
	{
		char c;
		printString("Enter \"r\", \"g\", or \"b\":\n\r");					// to include quotes in string use \"
													// \n creates a new line, \r moves cursor to beginning of line, combined moves cursor to beginning of new line
		c = readChar();										// read character from command line
		printChar(c);										// print the user input
		printString("\n\r");									// move cursor to beginning of next line
		
		switch(c)
		{
			case 'r':
				GPIOF->DATA = 0x02;							// turn on red LED
				break;
			case 'b':
				GPIOF->DATA = 0x04;							// turn on blue LED
				break;
			case 'g':
				GPIOF->DATA = 0x08;							// turn on green LED
				break;
			default:
				GPIOF->DATA &= ~(0xE);							// turn off all LEDs
				break;
		}
	}
}

void UART_Init(void)
{
	// [CONFIGURE SYSTEM AND PORT CLOCK]
	SYSCTL->RCGCUART |= 0x01;									// enable UART module 0
	SYSCTL->RCGCGPIO |= 0x01;									// enable GPIO Port A
	GPIOA->AFSEL |= 0x03;										// enable PA0, PA1 [alternate function select instead of GPIO], pg. 671
	// Slew rate or current level does not need to be configured here since the GPIO port is not in use
	GPIOA->PCTL |= 0x11;										// set able PMC0, PMC1 (corresponds with PA0, PA1 [port control]
													// PA0, PA1 are under Bit (1) on pg. 1351 -> bit (1) set on PMC0, PMC1
	GPIOA->DEN = 0x03;										// enable digital functionality for PA0, PA1
	
	// [CONFIGURE UART PROTOCOL]
	// [RATE CALCULATIONS]
	// 16 MHz is the default clock frequency of Tiva C, Serial baud rate is set to 9600
	// UARTIBRD = Clock frequency / (16 * baud rate) -> 16 MHz / (16 * 9600) = 104.167
	// UARTFRBD = integer( [fractional portion of IBRD] * 64 + .5) -> integer(.167 * 64 + .5) = 11
	UART0->CTL &= ~(0x01);										// disable UART [UART control]
	UART0->IBRD = 104;										// integer portion of BRD [integer baud-rate divisor]
	UART0->FBRD = 11;										// fractional portion of BRD [fractional baud-rate divisor]
	UART0->LCRH = 0x3<<5|(1<<4);									// 8-bit data, disable stop bits, disable FIFOs, single stop bit at end of frame, normal use [line control register]
	UART0->CC |= 0x00;										// system clock based on clock source and divisor factor [clock configuration]
	UART0->CTL |= 0x301;										// enable UART, enable UART recieve section, enable UART transmit section [UART control]
	
	// [CONFIGURE LED PINS]
	SYSCTL->RCGCGPIO |= 0x20;									// enable GPIO Port F
	GPIOF->DIR |= 0xE;										// enable PF1, PF2, PF3 as output [direction register]
	GPIOF->DEN |= 0xE;										// enable digital functionality for PF1, PF2, PF3 [digital enable]
	GPIOF->DATA &= ~(0xE);										// turn off LEDs [data register]
}

char readChar(void)
{
	char c;												// char is 8 bits of data
	while((UART0->FR&(0x10))!=0);									// wait for the 'recieve holding register' to be not empty (wait for data to be transmitted from user to board) [flag register]
													// When user puts in data, UART0 Bit 4 is 0 (not empty) -> passes through the while() because statement is true
	c = UART0->DR;											// store transmitted data from data register in char c [data register]
													// "For transmitted data, if FIFO is disabled (disabled by default), data is stored in the transmitter holding register."
	return c;
}

void printChar(char c)
{
	while((UART0->FR&(0x20))!=0);									// wait for the 'transmit holding register' to be not full (wait for data to be received/data to be present) [flag register]
	UART0->DR = c;											// write transmitted data to data register [data register]
													// "For received data, if FIFO is dsiabled, data is stored in receiving holding register."
}

void printString(char * string)
{
	while(*string)											// while() continues until null (/0 at the end of string)
	{
		printChar(*(string++));									// The array name is just a pointer to the first element in the array.
													// Ex. 'string' is the same as '&string'
													// Dereferencing a pointer gives the value the pointer is pointing to: *ptr
													// 'string++' is the same as '&string++', the address increments everytime it loops
	}
}
