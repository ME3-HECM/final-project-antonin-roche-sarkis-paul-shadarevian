# Course project - Mine navigation search and rescue

This project is a uni project.
Goal is to familiarise ourselves with simple embedded systems and how to control them. 

Here, a buggy with a color sensor has to go through a maze wher color cards are placed on walls and each color represent an action (turn left, right, ...) and when not detecting a color or detecting white, returning back to the initial starting point. 
For full project overview, see the Project Brief in the repo

Hardware/Software used for this project:
- [Buggy][1] from Mikroelectronica
- The [cliker 2][2] from Mikroelectronica with a pic18fk (in our case a pic 18f67k40)
- A [MPlab pickit 4][3] as the in-circuit debugger from microchip
- The [MPlab IDE][4] as the IDE with XC8 compiler
- For all serial communication, [RealTerm][5] was used 
- [Color click][6] from Mikroelectronica
Note: For color detection, a black cover around the color sensor was used 

The code consists of two main elements:
1. Buggy controls, in dc_motor.c, using the PWM module of the PIC. The motor controls are in dc_motor.c. This file also contains  
1. Color sensing, communicating with the sensor using the I2C module (see i2c.c)

The system will not start until the battery is almost full for calibration purposes (calibrations were done with high battery). ADC.c is only looking at the continuous value from the battery charge.

timer0.c takes care of initializing the timer in order to record time while going through the maze to then use those values to return at starting point wihtout using the color sensor. 

i2c.c takes care of initializing the communication between pic and the color sensor sending out the master/slave protocols

color.c decides which commands to send to the color sensor as well as reading and returning the sensor values. This file also contain the calibration for the differebt colors

For color sensing, there is a calibration step necessary. The inputs to the function calssifying the color are: (curr color val - room color val)/max color val

To find room and max val, follow the two steps below:
2. First put the front of the buggy (the side with the color click) against a wall with a white paper and click on the button RF2 of the clicker 2
2. Then put the buggy at its starting point and aim the front of the buggy towards the first color card and click on the button RF3 on the clicker 2 (when calibrating for room, the buggy must be pointed in to a place "empty" => no color card in front) 

The buggy will start immediatly after the room calibration


[1]:https://www.mikroe.com/buggy
[2]:https://www.mikroe.com/clicker-2-pic18fk
[3]:https://www.microchip.com/en-us/development-tool/PG164140
[4]:https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide
[5]:https://realterm.sourceforge.io/
[6]:https://www.mikroe.com/color-click
