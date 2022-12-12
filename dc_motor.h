#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define mazesteps 100 //this is the upper limit number of steps to reach the end of the maze
#define setpower 20 // this is the max power that the motors should be increased up to

struct colors;

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft90(DC_motor *mL, DC_motor *mR);
void turnRight90(DC_motor *mL, DC_motor *mR);
void turnLeft135(DC_motor *mL, DC_motor *mR);
void turnRight135(DC_motor *mL, DC_motor *mR);
void turn180(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char dir);
void square(DC_motor *mL, DC_motor *mR, char dir);
void smallmovement(DC_motor *mL, DC_motor *mR, char dir);
void savepath(char instruction);
void savetime(int timercount);
void returnhome(DC_motor motorL, DC_motor motorR);
void returnstep(char instruction, DC_motor motorL, DC_motor motorR);
void carryoutstep(DC_motor motorL, DC_motor motorR, struct colors *read, struct colors *mx, struct colors *amb, char step);

//Return - Path memorisation
char path[mazesteps]; // array to store path being followed
int timearray[mazesteps]; //array to store for how long device keeps going full speed ahead
char step; 


signed char timeposition=0; //defines the index of the timearray 
//A pointer is not used in this case as the pointer will always return to its initial position 0 every time the function is called out}
signed char pathposition=0; //defines the index of patharray

extern int timercount;
extern char interruptenable = 0;
#endif
