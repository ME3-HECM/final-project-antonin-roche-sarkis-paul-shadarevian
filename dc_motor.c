#include <xc.h>
#include "dc_motor.h"
#include "timer0.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    
    LATEbits.LATE2 = 0;
    LATEbits.LATE4 = 0;        
    LATCbits.LATC7 = 0;
    LATGbits.LATG6 = 0; 
    
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;        
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;     
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2 Left motor
    RE4PPS=0x06; //CCP2 on RE4 Left motor
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    //Chosen Prescaler
    //For a 10kHz fqz, 1/10000 = 1x10^-4s time period
    //1x10-4/255 = 3.9216x10-7 sec per time count needed (or greater)
    // 3.9216x10-7 = PS/((64x10^6)/4) --> PS = 6.2745 ==> use  1:8 PS
    // at PS=8, Tclock = 5 x 10^-7 ==> time period is at count 200
    
    T2CONbits.CKPS=011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=199; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; //RE2
    CCPR2H=0; //RE4
    CCPR3H=0; //RC7
    CCPR4H=0; //RG6
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time 
        negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually 
void stop(DC_motor *mL, DC_motor *mR)
{
    
    while (mL->power > 0 && mR->power > 0) {
    mL->power--;
    mR->power--;
    __delay_ms(1);
    setMotorPWM(mL);
    setMotorPWM(mR);
    }

}

//function to make the robot turn left 
void turnLeft90(DC_motor *mL, DC_motor *mR)
{   stop(mL, mR);
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != 50 || mR->power != 50 ) {
        __delay_ms(5);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }
    
    __delay_ms(160);
    
    stop(mL, mR);
}

//function to make the robot turn right 
void turnRight90(DC_motor *mL, DC_motor *mR)
{   stop(mL, mR);
    
    mL->direction = 1;
    mR->direction = 0;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power <= 50 && mR->power <= 50 ) {
        __delay_ms(5);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }    
    
    __delay_ms(160);
    
    stop(mL, mR);
}

//function to make the robot turn left 
void turnLeft135(DC_motor *mL, DC_motor *mR)
{
    stop(mL, mR);
    
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power <= 50 || mR->power <= 50 ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }
    
    __delay_ms(500);
    stop(mL, mR);

}

//function to make the robot turn right 
void turnRight135(DC_motor *mL, DC_motor *mR)
{ 
    stop(mL, mR);
    
    mL->direction = 1;
    mR->direction = 0;
   
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power <= 50 || mR->power <= 50) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }

    __delay_ms(500);
    stop(mL, mR);
}

void turn180(DC_motor *mL, DC_motor *mR)
{ 
    stop(mL, mR); 
    mL->direction = 1;
    mR->direction = 0;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power <= 50 || mR->power <= 50 ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }   
    __delay_ms(300);
    stop(mL, mR);
}


//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char dir) // dir = 1 is for forward, 0 is for backward
{
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //slow decay
    mL->direction = dir;
    mR->direction = dir;
    
    //stop(mL, mR); 
    //mL->power = mR->power;
    while (mL->power <= setpower && mR->power <= setpower ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);    
    }
    
    if (dir == 1 && T0CON0bits.T0EN == 0) {starttimer0;} //if robot is not on return, start timer0 to count the time the robot is going straight (after it has reached a targeted setpower above)
    
}

//function to make the robot go forward or in reverse by approx. a square
void square(DC_motor *mL, DC_motor *mR, char dir)
{
    mL->direction = dir;
    mR->direction = dir;
    
    while (mL->power <= 50 || mR->power <= 50 ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  

    setMotorPWM(mL);    
    setMotorPWM(mR);    
    }
    
    __delay_ms(200);
    stop(mL, mR);
}

void smallmovement(DC_motor *mL, DC_motor *mR, char dir)
{
    mL->direction = dir;
    mR->direction = dir;
    
    while (mL->power <= 50 || mR->power <= 50 ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  

    setMotorPWM(mL);    
    setMotorPWM(mR);    
    }
    
    stop(mL, mR);
}

//RETURN
/************************************
 * In order to store the path and allow us to return, we have assigned a value from 1-6 for each of the instructions the list path
1 - FullSpeedAhead            -->the direction will be reversed for the return journey)
2 - TurnRight90 (Red, part of orange)      --> TurnLeft90 is called in the return journey
3 - TurnLeft90 (Green, part of pink)     --> TurnRight90 is called in the return journey
4 - Turn180 (Blue)         --> Turn180 called in return journey
5 - TurnRight135 (Orange)       --> TurnLeft135 called in return journey
6 - TurnLeft135 (Light Blue)       --> TurnLeft135 called in return journey
7 - Square (part of yellow and pink) --> square in the opposite direction

*We have optimised the return path by removing the reverse square (which is not necessary in the return journey) 
**************************************/

void savepath(char path[mazesteps], char instruction) //a pointer cannot be used within the function as this would cause the pointer position to be reset to 0 every time the function is called (not wanted)
{
    path[pathposition] = instruction;
    pathposition++;
}

int savetime(char timearray[mazesteps], int timercount)
{   
    T0CON0bits.T0EN=0; //turn timer off preventing counter from incrementing further 
    timearray[timeposition]=timercount; //store time robot was going straight in the time array
    timeposition++;
    timercount = 0; //reset the timer count
    return timercount;
}

void returnhome(char path[mazesteps], DC_motor motorL, DC_motor motorR, char timearray[mazesteps])
{
    while (pathposition >= 0) {
    
    //case if instruction is fullspeedahead()
    if (path[pathposition--] == 1) {
    
    fullSpeedAhead(&motorL, &motorR, 0);

//method with delay    
//    int y;
//    for(y=0; y<timearray[timeposition--]; y++) {__delay_ms(4);} //continue straight the time that had been saved in timearray 

// This is method with timer    
    starttimer0; //timer instead of delay is a more efficient option
    while(timercount < timearray[timeposition--]); //continue straight until timer reaches the timercount 
    
    T0CON0bits.T0EN=0; //turn timer off
    stop(&motorL, &motorR);//stop once the device has gone straight for the required amount of time
    
    }       
    
    //case if instruction is anything else
    else if (path[pathposition] != 1) {
    returnstep(path[pathposition], motorL, motorR);
    square(&motorL, &motorR, 1);} //this will allow it to go back, collide with the wall and reallign itself with the wall     
    }
}

void returnstep(char instruction, DC_motor motorL, DC_motor motorR) {
    if (instruction == 2) {turnLeft90(&motorL,&motorR);}
    if (instruction == 3) {turnRight90(&motorL,&motorR);}
    if (instruction == 4) {turn180(&motorL,&motorR);}
    if (instruction == 5) {turnLeft135(&motorL,&motorR);}
    if (instruction == 6) {turnRight135(&motorL,&motorR);}           
    if (instruction == 7) {square(&motorL,&motorR, 0);}
}