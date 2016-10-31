
#include <mbed.h>
#include "max6675.h"

//max6675::max6675(PinName _SCLK, PinName _CS, PinName _MISO): spi(NC, _MISO, _SCLK), ncs(_CS){
max6675::max6675(SPI& _SPI, PinName _CS) : spi(_SPI), ncs(_CS) {
	//Pins
	//sclk = _SCLK;
	//cs = _CS;
	//miso = _MISO;
}

float max6675::read_temp() {

	uint16_t tempProbe;

	select();
	
	wait(0.001); // 1 ms

	tempProbe 	=  spi.write(0);
	tempProbe  <<=8;
	tempProbe   |= spi.write(0);

	
	deselect(); 
	
	if (tempProbe & 0x4) {
		// Bit D2 is normally low and goes high when the thermocouple input is open
		return 15;//NAN;
	}
	
	tempProbe >>= 3;
	return tempProbe*0.25;
}

void max6675::select() {
    //Set CS low to start transmission (interrupts conversion)
    ncs = 0;
}

void max6675::deselect() {
    //Set CS high to stop transmission (restarts conversion)
    ncs = 1;
}
