#include "TM4C123GH6PM.h"

void Init(void);
void Delay_ms(int n);

int main(void)
{
	Init();
	while(1)
	{
		int SW1 = GPIOF->DATA&0x10;					// read PF4 into SW1
    int SW2 = GPIOF->DATA&0x01;					// read PF0 into SW2
    if((!SW1)&(!SW2))										// both pressed
		{									
      GPIOF->DATA = 0x02;								// rage mode
			Delay_ms(1000000);
			GPIOF->DATA = 0x04;
			Delay_ms(1000000);
			GPIOF->DATA = 0x08;
			Delay_ms(1000000);
    } 
		else
		{
      if((!SW1)&SW2)              	  // just SW1 pressed
			{
				GPIOF->DATA= 0xE;								// turn white
			} 
			else
			{
				if(SW1&(!SW2))    	          // just SW2 pressed
				{
					GPIOF->DATA= 0x06;						// turn pink
				}
				else
				{													// neither switch
					GPIOF->DATA = 0x02;						// blink normal
					Delay_ms(10000000);
					GPIOF->DATA = 0x04;
					Delay_ms(10000000);
					GPIOF->DATA = 0x08;
					Delay_ms(10000000);
				}
			}
		}
	}
}

void Init(void)
{
	SYSCTL->RCGCGPIO |= 0x20;																// enable Port F
	GPIOF->DIR |= 0xE;																			// set PF1, PF2, PF3 as output and PF0, PF4 as input
	GPIOF->PUR |= 0x11;																			// enable pull up resistor for PF0, PF4
	GPIOF->DEN |= 0x1F;																			// enable digital functionality of PF0, PF1, PF2, PF3, PF4
}

void Delay_ms(int n)
{
	int j;
	for(j=0; j<n; j++) {}
}
