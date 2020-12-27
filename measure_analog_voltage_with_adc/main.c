#include <TM4C123GH6PM.h>								// header file, must be in same source group as program file

int main(void)
{
	volatile unsigned int adcResult; 						// define variable prototype
	
	// Enable ADC (ADC1) and GPIO (Port E) Clock
	SYSCTL->RCGCADC |= (1<<1);							// enable ADC1 clock
	SYSCTL->RCGCGPIO |= (1<<4);							// enable Port E [general input/output port]
	
	// Initialize PE1 for ADC1 Input
	GPIOE->AFSEL |= (1<<1);								// enable PE1 (AIN2) [alternate function select]
	GPIOE->DEN &= ~(1<<1);								// disable PE1 [digital enable]
	GPIOE->AMSEL |= (1<<1);								// enable PE1 [analog mode select] 
	
	// Initialize Sample Sequencer (SS3)
	ADC1->ACTSS &= ~(1<<3);								// disable SS3 [active sample sequencer]
	ADC1->EMUX |= (0xF<<12);							// enable continuous sample [event multiplexer select]
	ADC1->SSMUX3 |= 2;								// enable AIN2 of SS3 [sample sequence input multiplexer select]
	ADC1->SSCTL3 |= (0x06);								// enable SS3 sample interrupt & END bit of sequence [sample sequence control]
	ADC1->IM |= (1<<3);								// enable SS3 [interrupt mask]
	ADC1->ACTSS |= (1<<3);								// re-enable SS3 [active sample sequencer]
	
	// GPIO LED Initialization
	SYSCTL->RCGCGPIO |= (1<<5);							// enable Port F [general input/output port]
	GPIOF->DIR |= (1<<3);								// enable PF3 [data direction register]
	GPIOF->DEN |= (1<<3);								// enable PF3 [digital enable]
	
	while(1)
	{
		ADC1->PSSI |= (1<<3);							// initiate SS3 sampling [processor sample sequence initiate]
		while((ADC1->RIS &= (1<<3)) == 0);					// check if SS3 interrupt status is 0/off [raw interrupt status]
		adcResult = ADC1->SSFIFO3;						// assigns SS3 sample sequence result in 'adcResult'
		ADC1->ISC = (1<<3);							// set and clear SS3 [interrupt status and clear]
		
		if(adcResult>=2048)
		{
			GPIOF->DATA |= (1<<3);
		}
		else if(adcResult<2048)
		{
			GPIOF->DATA = 0x00;
		}
	}
}
