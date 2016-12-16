/**
@file N5110.h

@brief Header file containing member functions and variables

*/

#ifndef N5110_H
#define N5110_H

// Command Bytes - taken from Chris Yan's library
// More information can be found in the display datasheet
// H = 0 - Basic instructions
#define CMD_DC_CLEAR_DISPLAY   0x08
#define CMD_DC_NORMAL_MODE     0x0C
#define CMD_DC_FILL_DISPLAY    0x09
#define CMD_DC_INVERT_VIDEO    0x0D
#define CMD_FS_HORIZONTAL_MODE 0x00
#define CMD_FS_VERTICAL_MODE   0x02
#define CMD_FS_BASIC_MODE      0x00
#define CMD_FS_EXTENDED_MODE   0x01
#define CMD_FS_ACTIVE_MODE     0x00
#define CMD_FS_POWER_DOWN_MODE 0x04
// H = 1 - Extended instructions
#define CMD_TC_TEMP_0          0x04
#define CMD_TC_TEMP_1          0x05
#define CMD_TC_TEMP_2          0x06
#define CMD_TC_TEMP_3          0x07
#define CMD_BI_MUX_24          0x15
#define CMD_BI_MUX_48          0x13
#define CMD_BI_MUX_100         0x10
#define CMD_VOP_6V06           0xB2
#define CMD_VOP_7V38           0xC8

// number of pixels on display
#define WIDTH 84
#define HEIGHT 48
#define BANKS 6

#include "mbed.h"

/**
@brief Library for interfacing with Nokia 5110 LCD display (https://www.sparkfun.com/products/10168) using the hardware SPI on the mbed.
@brief The display is powered from a GPIO pin meaning it can be controlled via software.  The LED backlight is also software-controllable (via PWM pin).
@brief Can print characters and strings to the display using the included 5x7 font.
@brief The library also implements a screen buffer so that individual pixels on the display (84 x 48) can be set, cleared and read.
@brief The library can print primitive shapes (lines, circles, rectangles)
@brief Acknowledgements to Chris Yan's Nokia_5110 Library.

@brief Revision 1.2

@author Craig A. Evans
@date   17th March 2015
 *
 * Example:
 * @code

#include "mbed.h"
#include "N5110.h"

//    VCC,SCE,RST,D/C,MOSI,SCLK,LED
N5110 lcd(p7,p8,p9,p10,p11,p13,p21);
// Can also power (VCC) directly from VOUT (3.3 V) -
// Can give better performance due to current limitation from GPIO pin

int main()
{
    // first need to initialise display
    lcd.init();

    while(1) {

        // these are default settings so not strictly needed
        lcd.normalMode();      // normal colour mode
        lcd.setBrightness(0.5); // put LED backlight on 50%

        // can directly print strings at specified co-ordinates
        lcd.printString("Hello, World!",0,0);

        char buffer[14];  // each character is 6 pixels wide, screen is 84 pixels (84/6 = 14)
        // so can display a string of a maximum 14 characters in length
        // or create formatted strings - ensure they aren't more than 14 characters long
        int temperature = 27;
        int length = sprintf(buffer,"T = %2d C",temperature); // print formatted data to buffer
        // it is important the format specifier ensures the length will fit in the buffer
        if (length <= 14)  // if string will fit on display
            lcd.printString(buffer,0,1);           // display on screen

        float pressure = 1012.3;  // same idea with floats
        length = sprintf(buffer,"P = %.2f mb",pressure);
        if (length <= 14)
            lcd.printString(buffer,0,2);

        // can also print individual characters at specified place
        lcd.printChar('X',5,3);

        // draw a line across the display at y = 40 pixels (origin top-left)
        for (int i = 0; i < WIDTH; i++) {
            lcd.setPixel(i,40);
        }
        // need to refresh display after setting pixels
        lcd.refresh();

        // can also check status of pixels using getPixel(x,y)

        wait(5.0);
        lcd.clear();            // clear display
        lcd.inverseMode();      // invert colours
        lcd.setBrightness(1.0); // put LED backlight on full

        float array[84];

        for (int i = 0; i < 84; i++) {
            array[i] = 0.5 + 0.5*sin(i*2*3.14/84);
        }

        // can also plot graphs - 84 elements only
        // values must be in range 0.0 - 1.0
        lcd.plotArray(array);
        wait(5.0);
        lcd.clear();
        lcd.normalMode();      // normal colour mode back
        lcd.setBrightness(0.5); // put LED backlight on 50%

        // example of drawing lines
        for (int x = 0; x < WIDTH ; x+=10) {
            // x0,y0,x1,y1,type 0-white,1-black,2-dotted
            lcd.drawLine(0,0,x,HEIGHT,2);
        }
        lcd.refresh();   // need to refresh screen after drawing lines

        wait(5.0);
        lcd.clear();

        // example of how to draw circles
        lcd.drawCircle(WIDTH/2,HEIGHT/2,20,1);  // x,y,radius,black fill
        lcd.drawCircle(WIDTH/2,HEIGHT/2,10,2);  // x,y,radius,white fill
        lcd.drawCircle(WIDTH/2,HEIGHT/2,30,0);  // x,y,radius,transparent with outline
        lcd.refresh();   // need to refresh screen after drawing circles

        wait(5.0);
        lcd.clear();

        // example of how to draw rectangles
        //          origin x,y,width,height,type
        lcd.drawRect(10,10,50,30,1);  // filled black rectangle
        lcd.drawRect(15,15,20,10,2);  // filled white rectange (no outline)
        lcd.drawRect(2,2,70,40,0);    // transparent, just outline
        lcd.refresh();   // need to refresh screen after drawing rects


        wait(5.0);
        lcd.clear();

    }
}


 * @endcode
 */
class N5110
{

public:
    /** Create a N5110 object connected to the specified pins
    *
    * @param pwr Pin connected to Vcc on the LCD display (pin 1)
    * @param sce Pin connected to chip enable (pin 3)
    * @param rst Pin connected to reset (pin 4)
    * @param dc  Pin connected to data/command select (pin 5)
    * @param mosi Pin connected to data input (MOSI) (pin 6)
    * @param sclk Pin connected to serial clock (SCLK) (pin 7)
    * @param led Pin connected to LED backlight (must be PWM) (pin 8)
    *
    */
    //N5110(PinName pwrPin, PinName scePin, PinName rstPin, PinName dcPin, PinName mosiPin, PinName sclkPin, PinName ledPin);
    N5110(PinName pwrPin, PinName scePin, PinName rstPin, PinName dcPin, PinName ledPin, SPI *spiBus);

    /** Initialise display
    *
    *   Powers up the display and turns on backlight (50% brightness default).
    *   Sets the display up in horizontal addressing mode and with normal video mode.
    */
    void init();

    /** Turn off
    *
    *   Powers down the display and turns of the backlight.
    *   Needs to be reinitialised before being re-used.
    */
    void turnOff();

    /** Clears
    *
    *   Clears the screen.
    */
    void clear();

    /** Turn on normal video mode (default)
    *  Black on white
    */
    void normalMode();

    /** Turn on inverse video mode (default)
    *  White on black
    */
    void inverseMode();

    /** Set Brightness
    *
    *   Sets brightness of LED backlight.
    *   @param brightness - float in range 0.0 to 1.0
    */
    void setBrightness(float brightness);

    /** Print String
    *
    *   Prints a string of characters to the display. String is cut-off after the 83rd pixel.
    *   @param x - the column number (0 to 83)
    *   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row
    */
    void printString(const char * str,int x,int y);

    /** Print Character
    *
    *   Sends a character to the display.  Printed at the specified location. Character is cut-off after the 83rd pixel.
    *   @param  c - the character to print. Can print ASCII as so printChar('C').
    *   @param x - the column number (0 to 83)
    *   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row
    */
    void printChar(char c,int x,int y);

    /** Set a Pixel
    *
    *   This function sets a pixel in the display. A call to refresh() must be made
    *   to update the display to reflect the change in pixels.
    *   @param  x - the x co-ordinate of the pixel (0 to 83)
    *   @param  y - the y co-ordinate of the pixel (0 to 47)
    */
    void setPixel(int x, int y);

    /** Clear a Pixel
    *
    *   This function clears pixel in the display. A call to refresh() must be made
    *   to update the display to reflect the change in pixels.
    *   @param  x - the x co-ordinate of the pixel (0 to 83)
    *   @param  y - the y co-ordinate of the pixel (0 to 47)
    */
    void clearPixel(int x, int y);

    /** Get a Pixel
    *
    *   This function gets the status of a pixel in the display.
    *   @param  x - the x co-ordinate of the pixel (0 to 83)
    *   @param  y - the y co-ordinate of the pixel (0 to 47)
    *   @returns
    *       0           - pixel is clear
    *       non-zero    - pixel is set
    */
    int getPixel(int x, int y);

    /** Refresh display
    *
    *   This functions refreshes the display to reflect the current data in the buffer.
    */
    void refresh();

    /** Randomise buffer
    *
    *   This function fills the buffer with random data.  Can be used to test the display.
    *   A call to refresh() must be made to update the display to reflect the change in pixels.
    *   The seed is not set and so the generated pattern will probably be the same each time.
    *   TODO: Randomise the seed - maybe using the noise on the AnalogIn pins.
    */
    void randomiseBuffer();

    /** Plot Array
    *
    *   This function plots a one-dimensional array on the display.
    *   @param array[] - y values of the plot. Values should be normalised in the range 0.0 to 1.0. First 84 plotted.
    */
    void plotArray(float array[]);

    /** Draw Circle
    *
    *   This function draws a circle at the specified origin with specified radius to the display.
    *   Uses the midpoint circle algorithm.
    *   @see http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    *   @param  x0 - x-coordinate of centre
    *   @param  y0 - y-coordinate of centre
    *   @param  radius - radius of circle in pixels
    *   @param  fill - 0 transparent (w/outline), 1 filled black, 2 filled white (wo/outline)
    */
    void drawCircle(int x0,int y0,int radius,int fill);

    /** Draw Line
    *
    *   This function draws a line between the specified points using linear interpolation.
    *   @param  x0 - x-coordinate of first point
    *   @param  y0 - y-coordinate of first point
    *   @param  x1 - x-coordinate of last point
    *   @param  y1 - y-coordinate of last point
    *   @param  type - 0 white,1 black,2 dotted
    */
    void drawLine(int x0,int y0,int x1,int y1,int type);

    /** Draw Rectangle
    *
    *   This function draws a rectangle.
    *   @param  x0 - x-coordinate of origin (top-left)
    *   @param  y0 - y-coordinate of origin (top-left)
    *   @param  width - width of rectangle
    *   @param  height - height of rectangle
    *   @param  fill - 0 transparent (w/outline), 1 filled black, 2 filled white (wo/outline)
    */
    void drawRect(int x0,int y0,int width,int height,int fill);


private:

    void setXYAddress(int x, int y);
    void initSPI();
    void turnOn();
    void reset();
    void clearRAM();
    void clearBuffer();
    void sendCommand(unsigned char command);
    void sendData(unsigned char data);

public:
    unsigned char buffer[84][6];  // screen buffer - the 6 is for the banks - each one is 8 bits;

private:  // private variables
    SPI*    spi;
    PwmOut* led;
    DigitalOut* pwr;
    DigitalOut* sce;
    DigitalOut* rst;
    DigitalOut* dc;

};

const unsigned char font5x7[480] = {
    0x00, 0x00, 0x00, 0x00, 0x00,// (space)
    0x00, 0x00, 0x5F, 0x00, 0x00,// !
    0x00, 0x07, 0x00, 0x07, 0x00,// "
    0x14, 0x7F, 0x14, 0x7F, 0x14,// #
    0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
    0x23, 0x13, 0x08, 0x64, 0x62,// %
    0x36, 0x49, 0x55, 0x22, 0x50,// &
    0x00, 0x05, 0x03, 0x00, 0x00,// '
    0x00, 0x1C, 0x22, 0x41, 0x00,// (
    0x00, 0x41, 0x22, 0x1C, 0x00,// )
    0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
    0x08, 0x08, 0x3E, 0x08, 0x08,// +
    0x00, 0x50, 0x30, 0x00, 0x00,// ,
    0x08, 0x08, 0x08, 0x08, 0x08,// -
    0x00, 0x60, 0x60, 0x00, 0x00,// .
    0x20, 0x10, 0x08, 0x04, 0x02,// /
    0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
    0x00, 0x42, 0x7F, 0x40, 0x00,// 1
    0x42, 0x61, 0x51, 0x49, 0x46,// 2
    0x21, 0x41, 0x45, 0x4B, 0x31,// 3
    0x18, 0x14, 0x12, 0x7F, 0x10,// 4
    0x27, 0x45, 0x45, 0x45, 0x39,// 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
    0x01, 0x71, 0x09, 0x05, 0x03,// 7
    0x36, 0x49, 0x49, 0x49, 0x36,// 8
    0x06, 0x49, 0x49, 0x29, 0x1E,// 9
    0x00, 0x36, 0x36, 0x00, 0x00,// :
    0x00, 0x56, 0x36, 0x00, 0x00,// ;
    0x00, 0x08, 0x14, 0x22, 0x41,// <
    0x14, 0x14, 0x14, 0x14, 0x14,// =
    0x41, 0x22, 0x14, 0x08, 0x00,// >
    0x02, 0x01, 0x51, 0x09, 0x06,// ?
    0x32, 0x49, 0x79, 0x41, 0x3E,// @
    0x7E, 0x11, 0x11, 0x11, 0x7E,// A
    0x7F, 0x49, 0x49, 0x49, 0x36,// B
    0x3E, 0x41, 0x41, 0x41, 0x22,// C
    0x7F, 0x41, 0x41, 0x22, 0x1C,// D
    0x7F, 0x49, 0x49, 0x49, 0x41,// E
    0x7F, 0x09, 0x09, 0x01, 0x01,// F
    0x3E, 0x41, 0x41, 0x51, 0x32,// G
    0x7F, 0x08, 0x08, 0x08, 0x7F,// H
    0x00, 0x41, 0x7F, 0x41, 0x00,// I
    0x20, 0x40, 0x41, 0x3F, 0x01,// J
    0x7F, 0x08, 0x14, 0x22, 0x41,// K
    0x7F, 0x40, 0x40, 0x40, 0x40,// L
    0x7F, 0x02, 0x04, 0x02, 0x7F,// M
    0x7F, 0x04, 0x08, 0x10, 0x7F,// N
    0x3E, 0x41, 0x41, 0x41, 0x3E,// O
    0x7F, 0x09, 0x09, 0x09, 0x06,// P
    0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
    0x7F, 0x09, 0x19, 0x29, 0x46,// R
    0x46, 0x49, 0x49, 0x49, 0x31,// S
    0x01, 0x01, 0x7F, 0x01, 0x01,// T
    0x3F, 0x40, 0x40, 0x40, 0x3F,// U
    0x1F, 0x20, 0x40, 0x20, 0x1F,// V
    0x7F, 0x20, 0x18, 0x20, 0x7F,// W
    0x63, 0x14, 0x08, 0x14, 0x63,// X
    0x03, 0x04, 0x78, 0x04, 0x03,// Y
    0x61, 0x51, 0x49, 0x45, 0x43,// Z
    0x00, 0x00, 0x7F, 0x41, 0x41,// [
    0x02, 0x04, 0x08, 0x10, 0x20,// "\"
    0x41, 0x41, 0x7F, 0x00, 0x00,// ]
    0x04, 0x02, 0x01, 0x02, 0x04,// ^
    0x40, 0x40, 0x40, 0x40, 0x40,// _
    0x00, 0x01, 0x02, 0x04, 0x00,// `
    0x20, 0x54, 0x54, 0x54, 0x78,// a
    0x7F, 0x48, 0x44, 0x44, 0x38,// b
    0x38, 0x44, 0x44, 0x44, 0x20,// c
    0x38, 0x44, 0x44, 0x48, 0x7F,// d
    0x38, 0x54, 0x54, 0x54, 0x18,// e
    0x08, 0x7E, 0x09, 0x01, 0x02,// f
    0x08, 0x14, 0x54, 0x54, 0x3C,// g
    0x7F, 0x08, 0x04, 0x04, 0x78,// h
    0x00, 0x44, 0x7D, 0x40, 0x00,// i
    0x20, 0x40, 0x44, 0x3D, 0x00,// j
    0x00, 0x7F, 0x10, 0x28, 0x44,// k
    0x00, 0x41, 0x7F, 0x40, 0x00,// l
    0x7C, 0x04, 0x18, 0x04, 0x78,// m
    0x7C, 0x08, 0x04, 0x04, 0x78,// n
    0x38, 0x44, 0x44, 0x44, 0x38,// o
    0x7C, 0x14, 0x14, 0x14, 0x08,// p
    0x08, 0x14, 0x14, 0x18, 0x7C,// q
    0x7C, 0x08, 0x04, 0x04, 0x08,// r
    0x48, 0x54, 0x54, 0x54, 0x20,// s
    0x04, 0x3F, 0x44, 0x40, 0x20,// t
    0x3C, 0x40, 0x40, 0x20, 0x7C,// u
    0x1C, 0x20, 0x40, 0x20, 0x1C,// v
    0x3C, 0x40, 0x30, 0x40, 0x3C,// w
    0x44, 0x28, 0x10, 0x28, 0x44,// x
    0x0C, 0x50, 0x50, 0x50, 0x3C,// y
    0x44, 0x64, 0x54, 0x4C, 0x44,// z
    0x00, 0x08, 0x36, 0x41, 0x00,// {
    0x00, 0x00, 0x7F, 0x00, 0x00,// |
    0x00, 0x41, 0x36, 0x08, 0x00,// }
    0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
    0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};

#endif
