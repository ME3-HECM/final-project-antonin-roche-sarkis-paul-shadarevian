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

timercount=0; //each timer count represents 4ms based on the prescaler of timer0
void __interrupt(high_priority) HighISR();

void main(void){    
    
    Timer0_init(); 
    Interrupts_init();
    initDCmotorsPWM(199);
    color_click_init();
    unsigned int PWMcycle = 199;
    
    
    // Call the init functions
    color_click_init();
    I2C_2_Master_Init();
    
    //Setup of RGBC Colours Structure
    struct colors reading, max, ambient; 		//declare three color structures
    //1 for the readings and 1 for the max values that have been configured and 1 containing the readings under ambient  

    reading.red = 0;
    reading.blue = 0;
    reading.green = 0;
    reading.clear = 0;
    
    max.red = 0;
    max.blue = 0;
    max.green = 0;
    max.clear = 0;  
    
    LATFbits.LATF7 = 1; // blue LED 
    LATGbits.LATG1 = 1; // red LED
    LATAbits.LATA4 = 1; // green LED
    
    TRISDbits.TRISD3 = 0; // output pin for the front lights
    LATDbits.LATD3 = 0; // set the lights on 
    
    TRISFbits.TRISF2 = 1; // set clicker as input for calibration
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin  
    
    TRISDbits.TRISD7 = 0; //output pin
    
    
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
    
    //Setup of Motors Structure
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
    
    
    //Return - Path memorisation
    char path[mazesteps]; // array to store path being followed
    char timearray[mazesteps]; //array to store for how long device keeps going full speed ahead
    
    //16-bit integer counter value for the number of timer overflows (16-bit integer because the counter value could be high based on the maze)
    
    while(PORTFbits.RF3); // wait until button RF3 is pressed and then take ambient readings
    
    ambient.red = color_read_Red();
    ambient.blue = color_read_Blue();
    ambient.green = color_read_Green();
    ambient.clear = color_read_Clear();
    
    while(1){
        
    
    //LATHbits.LATH1=1; //turns on front white and back red LEDs
    //LATDbits.LATD3=1; //high brightness of front white LEDs
    //LATDbits.LATD4=1; //high brightness of back red LEDs
        
  
    fullSpeedAhead(&motorL,&motorR, 1); //timer is started at the very end of this function  
    savepath(path, 1); //store in path so that it can be recalled in the return
    
    
//            while (!PORTFbits.RF2){
//            
//            max_r = color_read_Red()*1000;
//            max_b = color_read_Blue()*1000;
//            max_g = color_read_Green();
//            max_c = color_read_Clear();
//            LATDbits.LATD7 = 1;
//        }
        

    reading.clear = (color_read_Clear()-ambient.clear)/(max.clear/1000); //division by 100 ensures result division is not converted to 0
    
    if (1200 > reading.clear > 30) { //threshold at which 
    
     timercount = savetime(timearray, timercount); //store the value of timer indicating for how long robot went fullSpeedAhead & reset timer value        
        
    smallmovement(&motorL,&motorR, 1); //collide to the wall and correct itself
    
    __delay_ms(200); //allow enough time for the colour click to adjust and read the change
    
    reading.red = (color_read_Red()-ambient.red)/(max.red/1000); 
    reading.blue = (color_read_Blue()-ambient.blue)/(max.blue/1000);
    reading.green = (color_read_Green()-ambient.green)/(max.green/1000);
    
    square(&motorL,&motorR, 0); //go back a little to allow device to turn     
    
    if (decide_color(&reading) == 2){ //red
    turnRight90(&motorL,&motorR);}  
    savepath(path, 2);
    
    if (decide_color(&reading) == 3){  //green
    turnLeft90(&motorL,&motorR);
    savepath(path, 3);}    
    
    if (decide_color(&reading) == 4){ //blue
    turn180(&motorL,&motorR);
    savepath(path, 4);}
    
    if (decide_color(&reading) == 5){ //yellow ////NOTE: NEED TO THINK ABOUT THIS ONE &Pink - how will we set the timer value to be exact to the time of return?  
    square(&motorL,&motorR, 0); // reverse 1 square
    turnRight90(&motorL,&motorR);
    savepath(path, 2);  //NEED TO FIX
        }
  
    if (decide_color(&reading) == 6){ //pink   
    square(&motorL,&motorR, 0);
    turnLeft90(&motorL,&motorR);    
    savepath(path, 3);  //NEED TO FIX
        } 
    
    if (decide_color(&reading) == 7){  //orange
    turnRight135(&motorL,&motorR);
    savepath(path, 5); 
        }
    
    if (decide_color(&reading) == 8){  //light blue
    turnLeft135(&motorL,&motorR);
    savepath(path, 6); 
        }
    
    if (1){ //black maze wall 
        }      
    
    if (decide_color(&reading) == 9) { //White Light or Colour unrecognised - return home 
        returnhome(path, motorL, motorR, timearray);
        }

    
    }
    
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

