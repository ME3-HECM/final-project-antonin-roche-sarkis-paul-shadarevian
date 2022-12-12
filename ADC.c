#include <xc.h>
#include "ADC.h"

/************************************
/ ADC_init_init
/ Function used to initialise ADC module and set it up
/ to sample on pin RA3
************************************/
void ADC_init(void)
{
    TRISEbits.TRISE0=1; // Select pin RE3 as input
    ANSELEbits.ANSELE0=1; //Ensure analogue circuitry is active 

    // Set up the ADC module - check section 33 of the datasheet for more details
    ADREFbits.ADNREF = 0; // Use Vss (0V) as negative reference
    ADREFbits.ADPREF = 0b00; // Use Vdd (3.3V) as positive reference
    ADPCH=0b11; // Select channel RA3/ANA3 for ADC
    ADCON0bits.ADFM = 0; // Left-justified result (i.e. no leading 0s)
    ADCON0bits.ADCS = 1; // Use internal Fast RC (FRC) oscillator as clock source for conversion
    ADCON0bits.ADON = 1; // Enable ADC
}

unsigned int ADC_getval(void)
{
    unsigned int tmpval;
       
    ADCON0bits.GO = 1; // Start ADC conversion

    while (ADCON0bits.GO); // Wait until conversion done (bit is cleared automatically when done)
        
    tmpval = ADRESH; // Get 8 most significant bits of the ADC result
    
    return tmpval; //return this value when the function is called
}


