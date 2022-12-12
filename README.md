# Course project - Mine navigation search and rescue

Hardware used for this code:
- [Buggy][1] from Mikroelectronica
- The [cliker 2][2] from Mikroelectronica with a pic18fk (in our case a pic 18f67k40)
- A [MPlab pickit 4][3] as the in-circuit debugger from microchip
- The [MPlab IDE][4] as the IDE with XC8 compiler
- For all serial communication, [RealTerm][5] was used 
- [Color click][6] from Mikroelectronica
Note: For color detection, a black cover around the color sensor was used 

The code consists of two main elements:
1. Buggy controls, in dc_motor.c, using the PWM module of the PIC chip
1. Color sensing. Color sensor used is the TCS3471 COLOR LIGHT-TO-DIGITAL CONVERTER, used with the color click and buggy decribed above. 


[1]:https://www.mikroe.com/buggy
[2]:https://www.mikroe.com/clicker-2-pic18fk
[3]:https://www.microchip.com/en-us/development-tool/PG164140
[4]:https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide
[5]:https://realterm.sourceforge.io/
[6]:https://www.mikroe.com/color-click
