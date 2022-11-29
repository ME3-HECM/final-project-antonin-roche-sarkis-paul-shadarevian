#include <xc.h>
#include "timer0.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * individual interrupt sources are enabled in their respective module init functions
************************************/
void Interrupts_init(void)
{
    PIE0bits.TMR0IE=1;  //interrupt on timer0 overflow (or match in 8bit mode)
    INTCONbits.PEIE=1;  //peripheral interrupts enabled (controls anything in PIE1+)
    INTCONbits.IPEN=0;  //high priority only
    INTCONbits.GIE=1;   //global interrupt enabled
}

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
/************************************
Aim: To calculate a suitable prescaler such that the timer overflows in approx. 50ms

 * Closest prescaler for 100ms is 1:16, taking 65.535ms for overflow
 * Time interval between counts is 0.001ms
 * Initial value must therefore be 15535 such that time passed until overflow is 50ms
 
************************************/
    
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; //
    T0CON1bits.T0CKPS=0b0100; // 1:16 prescaler
    T0CON0bits.T016BIT=1;	//16bit mode to allow a better accuracy of 100ms per (accuracy of 10?s)
	
    //initialising the timer 
    TMR0H=15535>>8;      //initial value will allow for overflow to occur at exactly 131ms      
    TMR0L=15535; 
    T0CON0bits.T0EN=0;	//keep timer off for now, timer will only be started every time the robot goes fullspeedahead to save battery
}

/************************************
 * Function to write a full 16bit timer value
 * Note TMR0L and TMR0H must be written in the correct order, or TMR0H will not contain the correct value
************************************/

void write16bitTMR0val(unsigned int tmp)
{
    TMR0H=tmp>>8; //MSB written
    TMR0L=tmp; //LSB written and timer updated
}

unsigned int get16bitTMR0val(void)
{
	//add your code to get the full 16 bit timer value here
   unsigned int a;
   
    // it's a good idea to initialise the timer registers so we know we are at 0
    a = TMR0L;       
    a = a|(TMR0H<<8);
    
    return a;
}
