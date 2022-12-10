#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    // Initialise the registers for lighting the LEDs
    TRISFbits.TRISF7 = 0; // blue LED Tris register as output
    TRISGbits.TRISG1 = 0; // red LED Tris register as output
    TRISAbits.TRISA4 = 0; // green LED Tris register as output
    
    LATFbits.LATF7 = 0; // blue LED
    LATGbits.LATG1 = 0; // red LED
    LATAbits.LATA4 = 0; // green LED
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

/*
 * When we want sending a command, send the slave address: 0x52
 * Then send a command (0x80 is 100xxxxx) of type: Repeated byte protocol transaction with address xxxxx
 * Then send the correct data
 * All is done through one buffer, the SSPBUF on the pic that handles the sending of data
 */

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
    // reason why we add 0xA0: datasheet of the clicker board, to send command with auto-implement
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Green LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Blue LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at CLEAR low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Clear LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

char decide_color(colors *mx)
{
    
    
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 2;} //red
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 3;} //green
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 4;} //blue
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 5;} //yellow
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 6;} //pink
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 7;} //orange
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 8;} //light blue
    //if ( < mx->red < && < mx->blue < && < mx->green < && < mx->clear <) {return 9;} //white light
    //else {return 9;} //if no colour is detected, return
}