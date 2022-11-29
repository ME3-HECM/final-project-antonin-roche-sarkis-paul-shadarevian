#include <xc.h>
#include "dc_motor.h"

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
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //slow decay
    
    while (mL->power != 0 && mR->power != 0) {
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
    
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //fast decay (cast)   
    mL->direction = 0;
    mR->direction = 1;
    
    int setpower = 50;
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(100);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }
    setMotorPWM(mL);    
    setMotorPWM(mR);  
}

//function to make the robot turn right 
void turnRight90(DC_motor *mL, DC_motor *mR)
{   stop(mL, mR);
    
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //fast decay (cast)    
    mL->direction = 1;
    mR->direction = 0;
    
    int setpower = 50;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(100);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    
    }    
    
}

//function to make the robot turn left 
void turnLeft135(DC_motor *mL, DC_motor *mR)
{
    stop(mL, mR);
    
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //fast decay (cast)
    
    mL->direction = 0;
    mR->direction = 1;
    
    int setpower = 50;
    
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(100);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }
 
    setMotorPWM(mL);    
    setMotorPWM(mR);  
   
    
}

//function to make the robot turn right 
void turnRight135(DC_motor *mL, DC_motor *mR)
{ 
    stop(mL, mR);
    
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //fast decay (cast)  
    mL->direction = 1;
    mR->direction = 0;
    
    int setpower = 50;
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(100);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }    
}

void turn180(DC_motor *mL, DC_motor *mR)
{ 
    stop(mL, mR);
    mL->brakemode=1; //slow decay
    mR->brakemode=1; //fast decay (cast)  
    mL->direction = 1;
    mR->direction = 0;
    
    int setpower = 50;
    mL->power = 20;
    mR->power = 20;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(100);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  
    }    
}


//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    mL->brakemode=1; //fast decay
    mR->brakemode=1; //fast decay
    mL->direction = 1;
    mR->direction = 1;
    
    
    int setpower = 50;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  

    setMotorPWM(mL);    
    setMotorPWM(mR);    
    }
}

//function to make the robot go in reverse
void reversesquare(DC_motor *mL, DC_motor *mR)
{
    mL->brakemode=1; //fast decay
    mR->brakemode=1; //fast decay
    mL->direction = 0;
    mR->direction = 0;
    
    int setpower = 50;
    
    while (mL->power != setpower || mR->power != setpower ) {
        __delay_ms(10);
        mL->power++;
        mR->power++;    
        setMotorPWM(mL);    
        setMotorPWM(mR);  

    setMotorPWM(mL);    
    setMotorPWM(mR);    
    }
}