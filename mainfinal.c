// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "color.h"
#include "i2c.h"
#include "timer0.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void __interrupt(high_priority) HighISR();


void main(void){    
    
    signed int degree = -90;

    
    Timer0_init(); 
    int timercount;
    Interrupts_init();
    initDCmotorsPWM(199);
    color_click_init();
    unsigned int PWMcycle;
    PWMcycle = 199;
    
    //Pin RA4 on microcontroller is BAT-VSEBSE
    
    //Pin RH1 is HLamps - turns front white LEDs and back red LEDs at reduced brightness  
    LATHbits.LATH1=0;
    TRISHbits.TRISH1 = 0; 
    
    //Pin RD3 - controls brightness of front white LEDs
    LATDbits.LATD3=0;
    TRISDbits.TRISD3 = 0; 
    
    //Pin RD4 - controls brightness of back red LEDs
    LATDbits.LATD4=0; 
    TRISDbits.TRISD4 = 0;     
    
    
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 

    motorR.power = 0;
    motorL.direction = 1; //forward is 1, reverse is 0
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)

    motorL.power = 0; 
    motorR.direction = 1; 
    motorR.PWMperiod=PWMcycle; 
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP3 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP4 duty high byte 
    
    char path[50];
    char timearray[50];    
    
    //16-bit integer counter value for the number of timer overflows (16-bit integer because the counter value could be high based on the maze)
    
    while(1){
    
    //LATHbits.LATH1=1; //turns on front white and back red LEDs
    //LATDbits.LATD3=1; //high brightness of front white LEDs
    //LATDbits.LATD4=1; //high brightness of back red LEDs
    
    fullSpeedAhead(&motorL,&motorR);
    savepath(path, 1) //store in path so that it can be recalled in the return
    
    if (1){ //red
    timercount = savetime(timearray, timercount); //store the value of timer indicating for how long robot went fullSpeedAhead & reset timer value
    turnRight90(&motorL,&motorR);}  
    savepath(path, 2);
    
    if (1){  //green
    timercount = savetime(timearray, timercount); 
    turnLeft90(&motorL,&motorR);}
    savepath(path, 3);
    
    
    if (1){  //blue
    timercount = savetime(timearray, timercount); 
    turn180(&motorL,&motorR);}
    savepath(path, 4);
    
    if (1){ //yellow ////NOTE: NEED TO THINK ABOUT THIS ONE &Pink - how will we set the timer value to be exact to the time of return?  
    timercount = savetime(timearray, timercount);
    reversesquare(&motorL,&motorR);
    turnRight90(&motorL,&motorR);
    savepath(path, );  //NEED TO FIX
    }
  
    if (1){ //pink   
    timercount = savetime(timearray, timercount);
    reversesquare(&motorL,&motorR);
    turnLeft90(&motorL,&motorR);    
    } 
    
    if (1){  //orange
    timercount = savetime(timearray, timercount);
    turnRight135(&motorL,&motorR);
    savepath(path, 5); 
    }
    
    if (1){  //light blue
    timercount = savetime(timearray, timercount);
    turnLeft135(&motorL,&motorR);
    savepath(path, 6); 
    }
    

    if (1){ //black maze wall
    
    }      
    
    if (1) { //White Light - return home
        timercount = savetime(timearray, timercount);
        returnhome(path, motorL, motorR);
    
    }
    

    //__delay_ms(1000);
    }
    
}




/************************************
 * High priority interrupt service routine for Timer0
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF)
    {
        timercount++; //increment counter at every overflow
    }
    PIR0bits.TMR0IF=0; 
}

