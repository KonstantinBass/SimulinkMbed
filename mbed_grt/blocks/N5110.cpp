/**
@file N5110.cpp

@brief Member functions implementations

*/
#include "mbed.h"
#include "N5110.h"


N5110::N5110(PinName pwrPin, PinName scePin, PinName rstPin, PinName dcPin, PinName ledPin, SPI *spiBus)
{

    //spi = new SPI(mosiPin,NC,sclkPin); // create new SPI instance and initialise
    //initSPI();
    spi = spiBus;

    // set up pins as required
    led = new PwmOut(ledPin);
    pwr = new DigitalOut(pwrPin);
    sce = new DigitalOut(scePin);
    rst = new DigitalOut(rstPin);
    dc = new DigitalOut(dcPin);

}

// initialise function - powers up and sends the initialisation commands
void N5110::init()
{
    turnOn();     // power up
    wait_ms(10);  // small delay seems to prevent spurious pixels during mbed reset
    reset();      // reset LCD - must be done within 100 ms

    // function set - extended
    sendCommand(0x20 | CMD_FS_ACTIVE_MODE | CMD_FS_HORIZONTAL_MODE | CMD_FS_EXTENDED_MODE);
    // Don't completely understand these parameters - they seem to work as they are
    // Consult the datasheet if you need to change them
    sendCommand(CMD_VOP_7V38);    // operating voltage - these values are from Chris Yan's Library
    sendCommand(CMD_TC_TEMP_2);   // temperature control
    sendCommand(CMD_BI_MUX_48);   // bias

    // function set - basic
    sendCommand(0x20 | CMD_FS_ACTIVE_MODE | CMD_FS_HORIZONTAL_MODE | CMD_FS_BASIC_MODE);
    normalMode();  // normal video mode by default
    sendCommand(CMD_DC_NORMAL_MODE);  // black on white

    // RAM is undefined at power-up so clear
    clearRAM();

}

// sets normal video mode (black on white)
void N5110::normalMode()
{
    sendCommand(CMD_DC_NORMAL_MODE);

}

// sets normal video mode (white on black)
void N5110::inverseMode()
{
    sendCommand(CMD_DC_INVERT_VIDEO);
}

// function to power up the LCD and backlight
void N5110::turnOn()
{
    // set brightness of LED - 0.0 to 1.0 - default is 50%
    setBrightness(0.5);
    pwr->write(1);  // apply power
}

// function to power down LCD
void N5110::turnOff()
{
    setBrightness(0.0);  // turn backlight off
    clearRAM();   // clear RAM to ensure specified current consumption
    // send command to ensure we are in basic mode
    sendCommand(0x20 | CMD_FS_ACTIVE_MODE | CMD_FS_HORIZONTAL_MODE | CMD_FS_BASIC_MODE);
    // clear the display
    sendCommand(CMD_DC_CLEAR_DISPLAY);
    // enter the extended mode and power down
    sendCommand(0x20 | CMD_FS_POWER_DOWN_MODE | CMD_FS_HORIZONTAL_MODE | CMD_FS_EXTENDED_MODE);
    // small delay and then turn off the power pin
    wait_ms(10);
    pwr->write(0);

}

// function to change LED backlight brightness
void N5110::setBrightness(float brightness)
{
    // check whether brightness is within range
    if (brightness < 0.0)
        brightness = 0.0;
    if (brightness > 1.0)
        brightness = 1.0;
    // set PWM duty cycle
    led->write(brightness);
}


// pulse the active low reset line
void N5110::reset()
{
    rst->write(0);  // reset the LCD
    rst->write(1);
}

// function to initialise SPI peripheral
void N5110::initSPI()
{
    //spi->format(8,1);    // 8 bits, Mode 1 - polarity 0, phase 1 - base value of clock is 0, data captured on falling edge/propagated on rising edge
    //spi->frequency(4000000);  // maximum of screen is 4 MHz
}

// send a command to the display
void N5110::sendCommand(unsigned char command)
{
    dc->write(0);  // set DC low for command
    sce->write(0); // set CE low to begin frame
    spi->write(command);  // send command
    dc->write(1);  // turn back to data by default
    sce->write(1); // set CE high to end frame (expected for transmission of single byte)

}

// send data to the display at the current XY address
// dc is set to 1 (i.e. data) after sending a command and so should
// be the default mode.
void N5110::sendData(unsigned char data)
{
    sce->write(0);   // set CE low to begin frame
    spi->write(data);
    sce->write(1);  // set CE high to end frame (expected for transmission of single byte)
}

// this function writes 0 to the 504 bytes to clear the RAM
void N5110::clearRAM()
{
    int i;
    sce->write(0);  //set CE low to begin frame
    for(i = 0; i < WIDTH * HEIGHT; i++) { // 48 x 84 bits = 504 bytes
        spi->write(0x00);  // send 0's
    }
    sce->write(1); // set CE high to end frame

}

// function to set the XY address in RAM for subsequenct data write
void N5110::setXYAddress(int x, int y)
{
    if (x>=0 && x<WIDTH && y>=0 && y<HEIGHT) {  // check within range
        sendCommand(0x80 | x);  // send addresses to display with relevant mask
        sendCommand(0x40 | y);
    }
}

// These functions are used to set, clear and get the value of pixels in the display
// Pixels are addressed in the range of 0 to 47 (y) and 0 to 83 (x).  The refresh()
// function must be called after set and clear in order to update the display
void N5110::setPixel(int x, int y)
{
    if (x>=0 && x<WIDTH && y>=0 && y<HEIGHT) {  // check within range
        // calculate bank and shift 1 to required position in the data byte
        buffer[x][y/8] |= (1 << y%8);
    }
}

void N5110::clearPixel(int x, int y)
{
    if (x>=0 && x<WIDTH && y>=0 && y<HEIGHT) {  // check within range
        // calculate bank and shift 1 to required position (using bit clear)
        buffer[x][y/8] &= ~(1 << y%8);
    }
}

int N5110::getPixel(int x, int y)
{
    if (x>=0 && x<WIDTH && y>=0 && y<HEIGHT) {  // check within range
        // return relevant bank and mask required bit
        return (int) buffer[x][y/8] & (1 << y%8);
        // note this does not necessarily return 1 - a non-zero number represents a pixel
    } else {
        return 0;
    }
}

// function to refresh the display
void N5110::refresh()
{
    int i,j;

    setXYAddress(0,0);  // important to set address back to 0,0 before refreshing display
    // address auto increments after printing string, so buffer[0][0] will not coincide
    // with top-left pixel after priting string

    sce->write(0);  //set CE low to begin frame

    for(j = 0; j < BANKS; j++) {  // be careful to use correct order (j,i) for horizontal addressing
        for(i = 0; i < WIDTH; i++) {
            spi->write(buffer[i][j]);  // send buffer
        }
    }
    sce->write(1); // set CE high to end frame

}

// fills the buffer with random bytes.  Can be used to test the display.
// The rand() function isn't seeded so it probably creates the same pattern everytime
void N5110::randomiseBuffer()
{
    int i,j;
    for(j = 0; j < BANKS; j++) {  // be careful to use correct order (j,i) for horizontal addressing
        for(i = 0; i < WIDTH; i++) {
            buffer[i][j] = rand()%256;  // generate random byte
        }
    }

}

// function to print 5x7 font
void N5110::printChar(char c,int x,int y)
{
    if (y>=0 && y<BANKS) {  // check if printing in range of y banks

        for (int i = 0; i < 5 ; i++ ) {
            int pixel_x = x+i;
            if (pixel_x > WIDTH-1)  // ensure pixel isn't outside the buffer size (0 - 83)
                break;
            buffer[pixel_x][y] = font5x7[(c - 32)*5 + i];
            // array is offset by 32 relative to ASCII, each character is 5 pixels wide
        }

        refresh();  // this sends the buffer to the display and sets address (cursor) back to 0,0
    }
}

// function to print string at specified position
void N5110::printString(const char * str,int x,int y)
{
    if (y>=0 && y<BANKS) {  // check if printing in range of y banks

        int n = 0 ; // counter for number of characters in string
        // loop through string and print character
        while(*str) {

            // writes the character bitmap data to the buffer, so that
            // text and pixels can be displayed at the same time
            for (int i = 0; i < 5 ; i++ ) {
                int pixel_x = x+i+n*6;
                if (pixel_x > WIDTH-1) // ensure pixel isn't outside the buffer size (0 - 83)
                    break;
                buffer[pixel_x][y] = font5x7[(*str - 32)*5 + i];
            }

            str++;  // go to next character in string

            n++;    // increment index

        }

        refresh();  // this sends the buffer to the display and sets address (cursor) back to 0,0
    }
}

// function to clear the screen
void N5110::clear()
{
    clearBuffer();  // clear the buffer then call the refresh function
    refresh();
}

// function to clear the buffer
void N5110::clearBuffer()
{
    int i,j;
    for (i=0; i<WIDTH; i++) {  // loop through the banks and set the buffer to 0
        for (j=0; j<BANKS; j++) {
            buffer[i][j]=0;
        }
    }
}

// function to plot array on display
void N5110::plotArray(float array[])
{

    int i;

    for (i=0; i<WIDTH; i++) {  // loop through array
        // elements are normalised from 0.0 to 1.0, so multiply
        // by 47 to convert to pixel range, and subtract from 47
        // since top-left is 0,0 in the display geometry
        setPixel(i,47 - int(array[i]*47.0));
    }

    refresh();

}

// function to draw circle
void N5110:: drawCircle(int x0,int y0,int radius,int fill)
{
    // from http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius;
    int y = 0;
    int radiusError = 1-x;

    while(x >= y) {

        // if transparent, just draw outline
        if (fill == 0) {
            setPixel( x + x0,  y + y0);
            setPixel(-x + x0,  y + y0);
            setPixel( y + x0,  x + y0);
            setPixel(-y + x0,  x + y0);
            setPixel(-y + x0, -x + y0);
            setPixel( y + x0, -x + y0);
            setPixel( x + x0, -y + y0);
            setPixel(-x + x0, -y + y0);
        } else {  // drawing filled circle, so draw lines between points at same y value

            int type = (fill==1) ? 1:0;  // black or white fill

            drawLine(x+x0,y+y0,-x+x0,y+y0,type);
            drawLine(y+x0,x+y0,-y+x0,x+y0,type);
            drawLine(y+x0,-x+y0,-y+x0,-x+y0,type);
            drawLine(x+x0,-y+y0,-x+x0,-y+y0,type);
        }


        y++;
        if (radiusError<0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x) + 1;
        }
    }

}

void N5110::drawLine(int x0,int y0,int x1,int y1,int type)
{
    int y_range = y1-y0;  // calc range of y and x
    int x_range = x1-x0;
    int start,stop,step;

    // if dotted line, set step to 2, else step is 1
    step = (type==2) ? 2:1;

    // make sure we loop over the largest range to get the most pixels on the display
    // for instance, if drawing a vertical line (x_range = 0), we need to loop down the y pixels
    // or else we'll only end up with 1 pixel in the x column
    if ( abs(x_range) > abs(y_range) ) {

        // ensure we loop from smallest to largest or else for-loop won't run as expected
        start = x1>x0 ? x0:x1;
        stop =  x1>x0 ? x1:x0;

        // loop between x pixels
        for (int x = start; x<= stop ; x+=step) {
            // do linear interpolation
            int y = y0 + (y1-y0)*(x-x0)/(x1-x0);

            if (type == 0)   // if 'white' line, turn off pixel
                clearPixel(x,y);
            else
                setPixel(x,y);  // else if 'black' or 'dotted' turn on pixel
        }
    } else {

        // ensure we loop from smallest to largest or else for-loop won't run as expected
        start = y1>y0 ? y0:y1;
        stop =  y1>y0 ? y1:y0;

        for (int y = start; y<= stop ; y+=step) {
            // do linear interpolation
            int x = x0 + (x1-x0)*(y-y0)/(y1-y0);

            if (type == 0)   // if 'white' line, turn off pixel
                clearPixel(x,y);
            else
                setPixel(x,y);  // else if 'black' or 'dotted' turn on pixel

        }
    }

}

void N5110::drawRect(int x0,int y0,int width,int height,int fill)
{

    if (fill == 0) { // transparent, just outline
        drawLine(x0,y0,x0+width,y0,1);  // top
        drawLine(x0,y0+height,x0+width,y0+height,1);  // bottom
        drawLine(x0,y0,x0,y0+height,1);  // left
        drawLine(x0+width,y0,x0+width,y0+height,1);  // right
    } else { // filled rectangle
        int type = (fill==1) ? 1:0;  // black or white fill
        for (int y = y0; y<= y0+height; y++) {  // loop through rows of rectangle
            drawLine(x0,y,x0+width,y,type);  // draw line across screen
        }
    }

}
