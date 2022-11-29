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

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){    
    
    signed int degree = -90;

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
    
    while(1){
    
    LATHbits.LATH1=1; //turns on front white and back red LEDs
    LATDbits.LATD3=1; //high brightness of front white LEDs
    LATDbits.LATD4=1; //high brightness of back red LEDs
        
    
    if (){turnRight90(&motorL,&motorR);}  //red
    else if ( && reverse==1 ) {}  //red while going back
    
    if (){turnLeft90(&motorL,&motorR);}  //green
    
    if (){turn180(&motorL,&motorR);}  //blue
    
    if (){ //yellow
        reversesquare(&motorL,&motorR);
        turnRight90(&motorL,&motorR);} 
    
    if (){ //pink   
        reversesquare(&motorL,&motorR);
        turnLeft90(&motorL,&motorR);} 
    
    if (){turnRight135(&motorL,&motorR);}  //orange
    
    if (){turnLeft135(&motorL,&motorR);}  //light blue
    
    if (){ //white
    
    }  
    
    if (){ //black maze wall
    
    }      
    
    //fullSpeedAhead(&motorL,&motorR);
    //__delay_ms(1000);
    }
    
    
    
}
