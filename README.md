# Course project - Mine navigation search and rescue

This project is a university module project.
The goal is to familiarise ourselves with simple embedded systems and how to control them. 

Project Aim: A buggy with a color sensor has to go through a maze wher color cards are placed on walls and each color represent an action (turn left, right, ...) and when not detecting a color or detecting white, returning back to the initial starting point. 
For full project overview, see the Project Brief in the repo

Hardware/Software used for this project:
- [Buggy][1] from Mikroelectronica
- The [cliker 2][2] from Mikroelectronica with a pic18fk (in our case a pic 18f67k40)
- A [MPlab pickit 4][3] as the in-circuit debugger from microchip
- The [MPlab IDE][4] as the IDE with XC8 compiler
- For all serial communication, [RealTerm][5] was used 
- [Color click][6] from Mikroelectronica

Note: For color detection, a black cover around the color sensor was used so as to minimise any interference from changes in the area around it, as seen below. <br>
<img src="https://user-images.githubusercontent.com/111131762/207319352-69f9c67b-fe12-4777-8931-8bb905875e90.png" width="100">

# Callibration and Main operation

Then, through testing, it has been observed that the ratio of those normalized colors to the normalised clear is constant for the same calibration: norm_color/norm_clear = constant (this step is done for red, blue and green)
Using this knowledge, finding colors becomes an easier step and can be done through if statements.

For color sensing, there is a calibration step necessary. The inputs to the function classifying the color (decide_color() in color.c) are: <br>
<br>
(curr color val - room color val)/max color val  --> This normalization allows the comparison and detection of colours
<br>
Also, it has been observed through testing that: norm_color/norm_clear = constant; for the same calibration. 
This means that using the normalization then scaling with clear allows for easy color recognition using if statements.
<br>
To find room and max val, follow the two steps below:
1. First put the front of the buggy (the side with the color click) against a wall with a white paper and click on the button RF2 of the clicker 2
2. Then put the buggy at its starting point and click on the button RF3 on the clicker 2 (when calibrating for room, the buggy must be pointed in to a place "empty" => no color card in front) 
<br>
The buggy will start immediately after the room calibration.
<br>
See this video to see this above demonstrated, along with colour detection and some of the special features below: https://youtu.be/Gx8sHAmDlEA 

# Unique features of our buggy mining operation
- **Colour not detected:** While our method of colour calibration allows the colour to be detected reliably, if in any case a colour is not detected (return value of 10 from ‘decide_color()’ function), the buggy will go back and recollide with the wall, make a readings over a small time interval and simultaneously check if any of those readings correspond to a colour in the records (while(a<20) loop in mainfinal.c code).  If yes, the buggy will carry out the corresponding step (carryoutstep() function in dc_motor.c); if not, the buggy will return home and enter sleep mode (to conserve energy).
- **Stabilisation:** Once the buggy detects a colour, it attacks the wall once, pauses to stabilize and then attacks the wall again. Similarly, during return, it will also collide and stabilize with the wall.
- Buggy surrounded LED lights are off during the ongoing journey to minimize the effect of those lights in the colour readings. They however turn on once return has been initiated to signal that the buggy is attempting to return, whilst the colorclick LEDs are turned off as they have been seen to consume the most battery. 
- **Return in Reverse:** The return is done in reverse so that the collision to stabilise the buggy is done with the front part. Doing collisions with the back of the buggy could damage the charging port located at the back.
- **Battery Check:** As performance degrades highly at medium/low battery levels, the buggy will warn the user and will not operate when it is turned on with less than about 40% battery (it will only turn on an LED and keep it on until it’s more than 40%), as shown below.<br>

![low battery warning buggy](https://user-images.githubusercontent.com/111131762/207319460-22a9d1ea-7d34-4d9c-be61-b5fbd6f40faa.png)

# Code Structure

## Main structures and variables
2 main types of structures have been defined, DC Motor and colors. There are 2 structures of type DC Motor (motorL for left motors and MotorR for right motors) and then 3 colors structures (reading for the constantly read values, max for the calibrated values under white light, ambient for the calibrated values under room light).

Main Modules of the PIC used in the module are: Timer0(to time straight path), Timer2 (for motors), ADC (battery check)

An integer 'timercount' is defined which incremenets in the interrupt when Timer0 overflows every 4ms (highest accuracy that could be obtained with the prescalers and 16-bit timer - 16-bit preferred as the initial value could be defined such that the overflow occurs at exactly 4ms) and keeps a record of time the buggy has been going forward. This value is stored in 'timearray' (using savetime() function in dc_motor.c) which is iterated through backwards during return.<br>
<br>
The char 'interruptenable' is assigned either 1 or 0 depending on when timercount should be incremented or not. The timercount needs not to be incremented during turns (we are not counting time in those cases) so it is turned 0 once the buggy has detected a colour reading (in the savetime() function while saving the time it went straight). <br>
<br>
Finally, the 'path' array are stores the number corresponding to the step taken during the ongoing journey (using savepath() function in dc_motor.c) so that it can be recalled during return.

## Code Files
- mainfinal.c code – calls out all initialisations and battery check, followed by calibration and then a while loop instructing the buggy to go straight and initialise the timer within the fullspeedahead() function. Once clear value is detected to be within the range of colours clear values (‘if’ statement in line 143), function is called out to collide to the wall, read the values and decide colour or attempt to reread the colour. Finally, the code contains the interrupt that increments the timer count every 4ms when device is going straight.
- dc_motor.c code – contains initialization, setting of motor functions and calls functions for motor movements. Individual functions were defined for each of 90, 135 and 180 degree turns as calling 45 degree angles iteratively would result in great inaccuracies in the higher 135 and 180 angle turns. This code also contains the function carryoutstep() which calls out the correct motor operations for the colours detected from colour_decide(), as well as the return operations using the timearray and path lists containing records of what steps were initially taken and for how long (only for the straight path).
- i2c.c code – takes care of initializing the communication between PIC and the colour sensor sending out the master/slave protocols 
- Timer0.c code – takes care of initializing the timer in order to record time while going through the maze to then use those values to return at starting point without using the color sensor.
- colour.c code – contains the functions that decides which commands to send to the color sensor as well as reading and returning the color sensor values. The decide_colour() contains the calibrated ranges for the different colours and decides the colours based on those.
- ADC.c code – only contains the initialization and function looking at the pin BAT-VSENSE which is proportional to the battery charge.


[1]:https://www.mikroe.com/buggy
[2]:https://www.mikroe.com/clicker-2-pic18fk
[3]:https://www.microchip.com/en-us/development-tool/PG164140
[4]:https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide
[5]:https://realterm.sourceforge.io/
[6]:https://www.mikroe.com/color-click
