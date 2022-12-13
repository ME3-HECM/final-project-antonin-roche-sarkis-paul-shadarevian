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
#include "ADC.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//16-bit integer counter value for the number of timer overflows (8-bit integer because the counter value could be high based on the maze)
timercount=0; //each timer count represents 4ms based on the prescaler of timer0
void __interrupt(high_priority) HighISR();

void main(void){    
    
    //INITIALISATION
    //Timer0 & DC Motor Interrupts
    Timer0_init(); 
    Interrupts_init();
    initDCmotorsPWM(199);
    unsigned int PWMcycle = 199;
    // Call the colour init functions
    color_click_init();
    I2C_2_Master_Init();
    
    
    //SETUP OF RGBC COLOUR STRUCTURE
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
    ambient.red = 0;
    ambient.blue = 0;
    ambient.green = 0;
    ambient.clear = 0;  


    
    TRISDbits.TRISD3 = 0; // output pin for the front lights
    LATDbits.LATD3 = 0; // set the lights on 
    TRISFbits.TRISF2 = 1; // set clicker as input for calibration
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin   
    LATDbits.LATD7 = 0;
    TRISDbits.TRISD7 = 0; //output pin
    
    //CHECKING IF BATTERY LEVEL is ADEQUATE
    //Pin RF6 on microcontroller is BAT-VSEBSE
    //Warn the user if the battery is lower than 70% and do not move on as it will affect the performance of the buggy
    //(F6 pin will have a voltage reading of 1/3 of the battery voltage)
    //ADC_getval() is a number from 0 to 255. 0 is full charge reading
    // 40% of this is around 100
    ADC_init();
    while (ADC_getval()<100) {LATDbits.LATD7 = 1;}
    //If battery is less than 40%, The battery gets stuck in an infinite loop and warns
    
    //SETUP OF FRONT AND BACK BUGGY LEDs
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

    motorL.power = 0; 
    motorL.direction = 1; //forward is 1, reverse is 0
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)

    motorR.power = 0;
    motorR.direction = 1; 
    motorR.PWMperiod=PWMcycle; 
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP3 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP4 duty high byte 
    
    
    //TURN COLOUR2CLICK LEDs on
    LATFbits.LATF7 = 1; // blue LED 
    LATGbits.LATG1 = 1; // red LED
    LATAbits.LATA4 = 1; // green LED
    
    
    //CALIBRATION
    //button RF2 and LED RD7 setup
    TRISFbits.TRISF2 = 1; // set clicker as input for calibration
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin  
    LATDbits.LATD7 = 0; //
    TRISDbits.TRISD7 = 0; //output pin
    //button RF3 and LED RH3 setup
    TRISFbits.TRISF3 = 1; // set clicker as input for calibration
    ANSELFbits.ANSELF3 = 0; //turn off analogue input on pin  
    LATHbits.LATH3 = 0; //
    TRISHbits.TRISH3 = 0; //output pin
    
    //Callibrating for the maximum blue, green, red and clear readings (which are under white light)
    while (PORTFbits.RF2); //Wait until button RF2 is pressed and then take color readings
    LATDbits.LATD7 = 1;     
    max.red = color_read_Red();
    max.blue = color_read_Blue();
    max.green = color_read_Green();
    max.clear = color_read_Clear();
    __delay_ms(500);
    LATDbits.LATD7 = 0;
    
    //Callibrating for ambient light readings
    while(PORTFbits.RF3); // wait until button RF3 is pressed and then take ambient readings
    LATHbits.LATH3 = 1;
    ambient.red = color_read_Red();
    ambient.blue = color_read_Blue();
    ambient.green = color_read_Green();
    ambient.clear = color_read_Clear();
    __delay_ms(500);
    LATHbits.LATH3 = 0;
    
    //BEGIN MAIN LOOP (START MOVING)
    while(1){    
  
        
        fullSpeedAhead(&motorL,&motorR, 1); //timer is started at the very end of this function  
        
        reading.clear = (color_read_Clear()-ambient.clear)/(max.clear/1000+1); //division by 1000 ensures result division is not converted to 0        
        
        if (reading.clear < 1200  && reading.clear > 30) { //threshold at which 
                
            savetime(timercount); //store the value of timer indicating for how long robot went fullSpeedAhead & reset timer value        
            savepath(1); //store in path so that it went fullspeedahead so that it can be recalled in the return
    
            smallmovement(&motorL,&motorR, 1); //collide to the wall and correct itself
            __delay_ms(200); // give a few seconds for buggy to stabilise
            smallmovement(&motorL,&motorR, 1); //collide with the wall again    
            __delay_ms(200); //allow enough time for the colour click to adjust and read the change   

            reading.red = (color_read_Red()-ambient.red)/(max.red/1000+1); 
            reading.blue = (color_read_Blue()-ambient.blue)/(max.blue/1000+1);
            reading.green = (color_read_Green()-ambient.green)/(max.green/1000+1);
            reading.clear = (color_read_Clear()-ambient.clear)/(max.clear/1000+1);
            
            //decide on the step based on the colour recognised and carry out that step
            step = decide_color(&reading); 
            smallmovement(&motorL,&motorR, 0); //go back a little to allow device to turn or do any of the required steps
    
            //If colour is reccognised, step will be from 1 to 9
            if (step<=9){carryoutstep(motorL, motorR, &reading, &max, &ambient, step);}
            //Otherwise, colour has not been recognised; try to read again
            else {            
                //collide back into the wall and reread the colour
                square(&motorL,&motorR, 1);
                //keep measuring the colour for some time, and if it's found that at any instant a colour is read, carry out that instruction
                char a = 0;
                while (a<20) {   
                    __delay_ms(30);
                    reading.red = (color_read_Red()-ambient.red)/(max.red/1000+1); 
                    reading.blue = (color_read_Blue()-ambient.blue)/(max.blue/1000+1);
                    reading.green = (color_read_Green()-ambient.green)/(max.green/1000+1);
                    reading.clear = (color_read_Clear()-ambient.clear)/(max.clear/1000+1);
                    if (step == 10) {step = decide_color(&reading);} //if still not recognising a colour, try to recognise again
                    a++;
                    }
                //If colour is now recognised, carry out the required step 
                if (step !=10) {carryoutstep(motorL, motorR, &reading, &max, &ambient, step);}
                //If colour is still not recognised, return home
                else {returnhome(motorL, motorR);}
        
                }
    
            }   

        }

    }
/************************************
 * High priority interrupt service routine for Timer0
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF == 1 & interruptenable == 1)
    {
        timercount++; //increment counter at every overflow
        starttimer0(); //set the initial values to the timer   
    }
    PIR0bits.TMR0IF=0;           
}

