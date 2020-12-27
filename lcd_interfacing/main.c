// LCD 4-Bit Mode Interfacing with Tiva C
// ---------------------------------------------------------------------------------------------------
// Notes:
// 4-Bit mode means that data is transferred 4 bits at a time
// The LCD display works at a lower speed than Tiva C, meaning a delay is needed between consecutive commands to LCD
// RS low = send commands to LCD, RS high = send data to LCD
// RW low = write data to LCD, RW high = read data from LCD
// E low = LCD off, E high = LCD high
#include "TM4C123GH6PM.h"
#define RS 0x20
#define RW 0x40
#define EN 0x80

void Delay_ms(int n);
void Delay_us(int n);
void LCD_Init(void);
void LCD_Cmd(unsigned char command);
void LCD_Data(unsigned char data);

int main(void)
{
	LCD_Init();
	LCD_Cmd(0x01);																	// Command: Clear display
	LCD_Cmd(0x80);																	// Command: Set first row
	Delay_ms(500);
	
	LCD_Data('A');
	LCD_Data('B');
	Delay_ms(500);
}

void LCD_Init(void)
{
	SYSCTL->RCGCGPIO |= 0x03;												// enable GPIOA and GPIOB for data port
	while((SYSCTL->RCGCGPIO & 0x03)==0);						// wait for clock to be enabled
	GPIOB->DIR |= 0xFF;															// enable all pins as output
	GPIOA->DIR |= 0xFF;															// enable all pins as output
	GPIOA->DEN |= 0xFF;															// enable digital functionality for all pins
	GPIOB->DEN |= 0xFF;															// enable digital functionality for all pins
	LCD_Cmd(0x20);																	// Command: Set 5x7 Font Size
	LCD_Cmd(0x28);																	// Command: Set 4-Bit mode
	LCD_Cmd(0x06);																	// Command: Move cursor to the right
	LCD_Cmd(0x01);																	// Command: Clear Display
	LCD_Cmd(0x0F);																	// Command: Blink cursor
}
	
void LCD_Cmd(unsigned char command)								// [Function to send commands to LCD]
{
	unsigned char temp;
	
	// Send lower nibble to LCD
	GPIOA->DATA = 0x00;															// send command to LCD, write data to LCD, LCD off
	temp = command >> 4; 														
	GPIOB->DATA = temp;															// send 'temp' to LCD
	GPIOA->DATA = EN;																// turn on LCD
	Delay_us(1);
	GPIOA->DATA = 0x00;															// send command to LCD, write data to LCD, LCD off
	
	// Send upper nibble to LCD
	GPIOA->DATA = 0x00;															// send command to LCD, write data to LCD, LCD off
	GPIOB->DATA = command;													// send 'command'	to LCD
	GPIOA->DATA = EN;																// turn on LCD
	Delay_us(1);
	GPIOA->DATA = 0x00;															// send command to LCD, write data to LCD, LCD off
	
	if(command < 4)
		Delay_ms(2);																	// 2 ms delay for "clear display" and "return home"
	else
		Delay_us(40);																	// 40 us delay for all other commands
}

void LCD_Data(unsigned char data)									// [Function to send data to LCD]
{
	unsigned char temp;
	
	// Send lower nibble to LCD
	GPIOA->DATA = RS;																// send data to LCD
	temp = data >> 4;
	GPIOB->DATA = temp;
	GPIOA->DATA = RS | EN;													// send data to LCD, turn on LCD
	Delay_ms(10);
	GPIOA->DATA = RS;																// send data to LCD, turn off LCD
	Delay_us(40);
	
	// Send upper nibble to LCD
	GPIOA->DATA = RS;																// send data to LCD
	GPIOB->DATA = data;
	GPIOA->DATA = RS | EN;													// send data to LCD, turn on LCD
	Delay_us(10);
	GPIOA->DATA = 0x00;															// send commands to LCD, turn off LCD
	Delay_us(40);
}	

void Delay_ms(int n)
{
	int i,j;
	for(i=0; i<n; i++)
	{
		for(j=0; j<3180; j++) {}
	}
}

void Delay_us(int n)
{
	int i,j;
	for(i=0; i<n; i++)
	{
		for(j=0; j<3; j++) {}
	}
}
