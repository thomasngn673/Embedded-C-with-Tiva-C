// Disclaimer: this file is strictly for learning ADC in the Tiva C Series Launchpad
// This file will not compile is ONLY for note taking

// This Program controls the onboard green LED based on discrete digital value of ADC controlled by potentiometer
// If AN2 channel value is less 2048 digital value, then LED turns off, otherwise remain on


#include "TM4C123GH6PM.h"
#include <stdio.h>

int main(void)
{
    // initialize result for ADC
    unsigned int adcResult = 0;

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ADC MODULE INITIALIZATION (Pg. 817)
    // 1. Enable the ADC clock using the RCGCADC register (see page 352).
    // There are 2 ADC modules, ADC0 & ADC1
    SYSCTL->RCGCADC = (1<<1); // enables ADC1 clock
    // ADC1 = 0010

    // 2. Enable the clock to the appropriate GPIO modules via the RCGCGPIO register (see page 340). To find out which GPIO ports to enable, refer to “Signal Description” on page 801.
    // Enable clock to GPIO pin which will now be used as analog input
    // Reference "GPIO Pins and Alt Functions" (Pg. 651/801) to find which GPIO pin can be used for analong input (AIN#)
    // For this example, we will use Port E --> AIN0, AIN1, AIN2, AIN3, AIN8, AIN9
    SYSCTL->RCGCGPIO = (1<<4); // enables Port E

    // 3. Set the GPIO AFSEL bits for the ADC input pins (see page 671). To determine which GPIOs to configure, see Table 23-4 on page 1344.
    // Port E is enabled = GPIOE
    // When AFSEL (Alternate Function Select) bit is enabled, GPIO line is controlled by associated peripheral
    // Reference (Pg. 651) and select which IO pin you want from PE# (Port E)
    GPIOE->AFSEL = (1<<1); // selects PE1 (AIN2)

    // 4. Configure the AINx signals to be analog inputs by clearing the corresponding DEN bit in the GPIO Digital Enable (GPIODEN) register (see page 682).
    GPIOE->DEN &= ~(1<<1); // disables digital functionality for PE1 of Port E
    // &= ensures that 'digital enable' is always disabled/off
    // |= ensures that bit is always on
    
    // 5. Disable the analog isolation circuit for all ADC input pins that are to be used by writing a 1 to the appropriate bits of the GPIOAMSEL register (see page 687) in the associated GPIO block.
    // AMSEL (Analog Mode Select) is only valid for ports/pins that can be used as ADC AINx inputs. (Pg. 687)
    // Enables analog function of GPIO pins
    GPIOE->AMSEL = (1<<1); // enables PE1 analog function
    
    // 6. If required by the application, reconfigure the sample sequencer priorities in the ADCSSPRI register. The default configuration has Sample Sequencer 0 with the highest priority 
    // and Sample Sequencer 3 as the lowest priority.
    // Sample sequencers are a part of ADC modules and get ADC samples and store conversion results in FIFO.

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // SAMPLE SEQUENCER CONFIGURATION (Pg. 818)
    // 1. Ensure that the sample sequencer is disabled by clearing the corresponding ASENn bit in the ADCACTSS register. Programming of the sample sequencers is allowed without having them enabled. 
    // Disabling the sequencer during programming prevents erroneous execution if a trigger event were to occur during the configuration process.
    // ADCACTSS (ADC Active Sample Sequencer) controls the activation of the sample sequencers (Pg. 821)
    ADC1->ACTSS &= ~(1<<3); // ensures that SS3 is always disabled/off

    // 2. Configure the trigger event for the sample sequencer in the ADCEMUX register.
    // ADCEMUX (ADC Event Multiplexer Select) selects the event (trigger) that initiates sampling for each sample sequencer (Pg. 833)
    ADC1->EMUX = (0xF<<12); // enables continuous sample
    // 0xF = 1111
    // 1111.0000.0000.0000 enables the continuous SS3 Trigger Select

    // 3. When using a PWM generator as the trigger source, use the ADC Trigger Source Select (ADCTSSEL) register to specify in which PWM module the generator is located. 
    // The default register reset selects PWM module 0 for all generators.
    // N/A for this configuration

    // 4. For each sample in the sample sequence, configure the corresponding input source in the ADCSSMUXn register.
    // ADCSSMUX3 (ADC Sample Sequence Input Multiplexer Select) defines analog input configuration for each sample (Pg. 851/875)
    ADC1->SSMUX3 = 2; // SSMUX3 is SS3, 2 is AIN2 which was from selecting PE1 of Port E

    // 5. For each sample in the sample sequence, configure the sample control bits in the corresponding nibble in the ADCSSCTLn register. 
    // When programming the last nibble, ensure that the END bit is set. Failure to set the END bit causes unpredictable behavior.
    // ADCSSCTL3 (ADC Sample Sequence Control) contains the configuration information for each sample executed with a sample sequencer (Pg. 876)
    ADC1->SSCTL3 = (0x6); // enables sample interrupt & END of sequence
    // 0x6 = 0110

    // 6. If interrupts are to be used, set the corresponding MASK bit in the ADCIM register.
    // This register controls whether the sample sequencer and digital comparator raw interrupt signals are sent to the interrupt controller. 
    ADC1->IM = (1<<3); // enables SS3 interrupt mask 
    // "The raw interrupt signal from Sample Sequencer 3 is sent to the interrupt controller." (Pg. 826)

    // 7. Enable the sample sequencer logic by setting the corresponding ASENn bit in the ADCACTSS register.
    // Renable the bit that was disabled in step 1.
    ADC1->ACTSS |= (1<<3); // enables SS3 (Sample Sequencer 3)

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // GPIO LED INITIALIZATION
    // PCB Data Sheet shows that LEDs use GPIO Port F
    // 1. Enable the clock to the port by setting the appropriate bits in the RCGCGPIO register (see page 340).
    SYSCTL->RCGCGPIO = (1<<5); // enables GPIO Port F
    // (1<<5) = 100000

    // 2. Set the direction of the GPIO port pins by programming the GPIODIR register. A write of a 1 indicates output and a write of a 0 indicates input.
    // "GPIODIR register is the data direction register. All GPIO pins are inputs by default since 0 is input." (Pg. 663)
    // PCB Data Sheet (Pg. 9) shows that Green LED is GPIO Pin PF3
    GPIOF->DIR = (1<<3); // enables PF3 of Port F
    // (1<<3) = 1000

    // 6. To enable GPIO pins as digital I/Os, set the appropriate DEN bit in the GPIODEN register. 
    // To enable GPIO pins to their analog function (if available), set the GPIOAMSEL bit in the GPIOAMSEL register.
    // LED Pins use digital outputs, so the DEN bit will be set.
    GPIOF->DEN = (1<<3); // enables digital function of PF3 of Port F
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    while(1)
    {
        // SAMPLE SEQUENCER DATA INITIATION

        // "Sampling is then initiated by setting the SSn bit in the ADC Processor Sample Sequence Initiate (ADCPSSI) register." (Pg. 802)
        // "This register also provides a means to configure and then initiate concurrent sampling on all ADC modules." (Pg. 845)
        ADC1->PSSI = (1<<3); // enable SS3 initiate

        // "ADCRIS (ADC Raw Interrupt Status) shows the status of the raw interrupt signal of each sample sequencer. May be polled by software to look for interrupt conditions
        // without sending the interrupts to the interrupt controller." (Pg. 823)
        // 8 = 1000
        // ADC1->RIS &= 8 enables SS3 Raw Interrupt Status
        while((ADC1->RIS &= (1<<3)) == 0); // whenever SS3 Raw Interrupt status has not occurred 

        // ADC1->SSFIFO3 contains the conversion results for samples collected with the SS3 sample sequencer FIFO
        adcResult = ADC1->SSFIFO3; // assigns sample sequence result to 'adcResult'

        // "The ADC Interrupt Status and Clear (ADCISC) register shows active interrupts that are enabled by the ADCIM register. 
        // Sequencer interrupts are cleared by writing a 1 to the corresponding bit in ADCISC." (Pg. 803)
        // "This register provides the mechanism for clearing sample sequencer interrupt conditions and shows the status of interrupts generated by the sample sequencers.
        // If software is polling the ASDCRIS instead of generating interrupts the sample sequence INRn bits are still cleared via ADCISC register." (Pg. 828)
        ADC1->ISC = (1<<3); // clears bit

        if(adcResult >= 2048)
        {
            GPIOF->DATA = (1<<3); // turn on green LED at PF3
        }
        else if(adcResult <= 2048)
        {
            GPIOF->DATA = 0x00; // turn off green LED at PF3
        }
    }
}

void SystemInit(void)
{
    SCB->CPACR |= 0x00f00000;
}
